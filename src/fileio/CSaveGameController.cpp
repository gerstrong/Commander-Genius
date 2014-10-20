/*
 * CSaveGameController.cpp
 *
 *  Created on: 13.08.2009
 *      Author		: Caitlin Shaw
 *      Adaptations : Gerstrong
 *  Routines for handling save&load of savegames
 */

#include <base/utils/FindFile.h>
#include "CSaveGameController.h"
#include "engine/core/CBehaviorEngine.h"
#include <ctime>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/version.hpp>

#include "fileio/KeenFiles.h"


void sgrle_initdecompression(void);
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
char sgrle_decompressV2(FILE *fp, unsigned char *ptr, unsigned long nbytes);
void sgrle_decompressV1(FILE *fp, unsigned char *ptr, unsigned long nbytes);

using namespace std;

// Initialization Routines
CSaveGameController::CSaveGameController() :
m_offset(0)
{
    CResource &keenFiles = gKeenFiles;

    setGameDirectory(keenFiles.gameDir);
	setEpisode(g_pBehaviorEngine->getEpisode());
}

void CSaveGameController::setGameDirectory(const std::string& game_directory)
{
	m_savedir = JoinPaths("save", game_directory);
}

void CSaveGameController::setEpisode(char Episode)
{	m_Episode = Episode;	}

void CSaveGameController::setLevel(int Level)
{	m_Level = Level;	}

// Retrieves the data size of the next block
Uint32 CSaveGameController::getDataSize(std::ifstream &StateFile)
{
	Uint32 size=0;
	for(Uint32 i=0 ; i<sizeof(Uint32) ; i++)
	{
		size += StateFile.get() << (i*8);
	}
	return size;
}

// Return a string that just says empty
std::string CSaveGameController::getUnnamedSlotName()
{
	std::string text;
	time_t rawtime;
  	struct tm * timeinfo;

   	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	if(m_Level == 80)
		text = "MAP";
	else
		text = "L" + itoa(m_Level);
	text += "-";
	text += asctime (timeinfo);
	return text;
}

// Read the data of size and stores it in the buffer
void CSaveGameController::readData(char *buffer, Uint32 size, std::ifstream &StateFile)
{
	for(Uint32 i=0 ; i<size ; i++) {
		buffer[i] = StateFile.get();
	}
}

// Used here for filtering the filetypes
struct StateFileListFiller
{
	std::set<std::string> list;

    bool operator() (const std::string& filename)
    {
		std::string ext = GetFileExtension(filename);

        bool canBeadded = false;

        for(int ep = 1 ; ep<=6 ; ep++)
        {
            canBeadded |= stringcaseequal(ext, "ck" + itoa(ep));
            canBeadded |= stringcaseequal(ext, "cx" + itoa(ep));
        }

        if ( canBeadded )
			list.insert(filename);

		return true;
	}
};
// This method returns the the list of files we can use for the menu
// It will filter by episode and sort the list by number of slot
// It also takes care of the slotname resolution
bool CSaveGameController::readSlotList(std::vector<std::string> &list)
{
    if(!list.empty())
        list.clear();

    //Get the list of ".ck?" and ".cx?" files
	StateFileListFiller sfilelist;
    gLogging.ftextOut("Reading savegames from \"%s\"", m_savedir.c_str());
	FindFiles(sfilelist, m_savedir, false, FM_REG);

    for( const std::string &filename : sfilelist.list )
    {
        std::string buf = filename.substr(filename.size()-1);

        const int foundEp = atoi(buf.c_str());

		// Check if the file fits to this episode
        if(foundEp == m_Episode)
		{
            Uint32 pos = getSlotNumber(filename)-1;

            const std::string ext = getExtension(filename);

            if(ext == "ck")
                buf = getSlotName(filename);
            else if(ext == "cx")
                buf = getSlotNameXML(filename);

            if(pos+1 > list.size())
                list.resize(pos+1);

            list.at(pos) = buf;
		}
	}

    gLogging.ftextOut("Done");

    return !list.empty();
}

/* --- Functions for older savegames START --- */

bool CSaveGameController::IsOldSGVersion5(const std::string& fname)
{
	const char *verify = "CKSAVE";
	FILE* fp = OpenGameFile(fname, "rb");
	if (!fp) return false;

	for(size_t i=0; i < strlen(verify); i++)
	{
		char c = fgetc(fp);
		if (c != verify[i])
		{
			fclose(fp);
			return false;
		}
		printf("%c", c);
	}
	if (fgetc(fp) != OLDSAVEGAMEVERSION5)
	{
		fclose(fp);
		return false;
	}
	fclose(fp);
	return true;
}

bool CSaveGameController::IsOldSGVersion4(const std::string& fname)
{
	FILE* fp = OpenGameFile(fname, "rb");
	if (!fp) return false;

	if (fgetc(fp) != 'S') { fclose(fp); return false; }
	if (fgetc(fp) != OLDSAVEGAMEVERSION4) { fclose(fp); return false; }

	fclose(fp);
	return true;
}

/**
 * This to find the proper old save game version
 */
int CSaveGameController::getOldSGVersion(const std::string& fname)
{
	if(IsOldSGVersion5(fname))
		return 5;
	if(IsOldSGVersion4(fname))
		return 4;
	else
		return 0;
}

// This function converts savegame all files from old versions of CG to the new format
void CSaveGameController::convertAllOldFormats()
{
	for(size_t slot=0 ; slot<=9 ; slot++ )
		convertOldFormat(slot);
}

/**
 * This function loads the savegame of the 5th version
 */
bool CSaveGameController::loadSaveGameVersion5(const std::string &fname, OldSaveGameFormatV5& old)
{
	FILE *fp;
	unsigned char episode, level, lives, numplayers;

	gLogging.ftextOut("Loading game from file %s\n", fname.c_str());
	fp = OpenGameFile(fname, "rb");
	if (!fp) { gLogging.ftextOut("unable to open %s\n",fname.c_str()); return false; }

	readOldHeader(fp, &episode, &level, &lives, &numplayers);

	gLogging.ftextOut("game_load: restoring structures...\n");
	/*primaryplayer =*/ fgetc(fp); // primary player doesn't exist anymore! Jump that!

	sgrle_compress(fp, (unsigned char *) &old.LevelControl, sizeof(old.LevelControl));

	// note that we don't have to load the LEVEL, because the state
	// of the map is already saved inside the save-game.
	sgrle_initdecompression();
	if (sgrle_decompressV2(fp, (unsigned char *) &old.LevelControl, sizeof(old.LevelControl))) return false;

	if (sgrle_decompressV2(fp, (unsigned char *)&old.scroll_x, sizeof(old.scroll_x))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.scrollx_buf, sizeof(old.scrollx_buf))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.scrollpix, sizeof(old.scrollpix))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.mapx, sizeof(old.mapx))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.mapxstripepos, sizeof(old.mapxstripepos))) return false;

	if (sgrle_decompressV2(fp, (unsigned char *)&old.scroll_y, sizeof(old.scroll_y))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.scrolly_buf, sizeof(old.scrolly_buf))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.scrollpixy, sizeof(old.scrollpixy))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.mapy, sizeof(old.mapy))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.mapystripepos, sizeof(old.mapystripepos))) return false;

	if (sgrle_decompressV2(fp, (unsigned char *)&old.max_scroll_x, sizeof(old.max_scroll_x))) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)&old.max_scroll_y, sizeof(old.max_scroll_y))) return false;

	if (sgrle_decompressV2(fp, (unsigned char *)&old.map, sizeof(old.map))) return false;

	unsigned char *tempbuf;

	tempbuf = new unsigned char[22624];

	/*highest_objslot = */fgetc(fp); fgetc(fp); // Not used anymore since objects are held in an vector.
	if (sgrle_decompressV2(fp, (unsigned char *)tempbuf, 22624)) return false;
	if (sgrle_decompressV2(fp, (unsigned char *)tempbuf, 9612)) return false;

	delete [] tempbuf;

	if (sgrle_decompressV2(fp, (unsigned char *)&old.Player, sizeof(old.Player))) return false;

	fclose(fp);

	return true;
}

bool CSaveGameController::loadSaveGameVersion4(const std::string &fname, OldSaveGameFormatV4& old)
{
	FILE *fp;
	//unsigned char episode, level, lives;
	unsigned int numplayers;

	gLogging.ftextOut("Loading game from file %s\n", fname.c_str());
	fp = OpenGameFile(fname, "rb");
	if (!fp) { gLogging.ftextOut("unable to open %s\n",fname.c_str()); return false; }

	gLogging.ftextOut("game_load: restoring structures...\n");
	if (fgetc(fp) != 'S') { fclose(fp); return false; }
	if (fgetc(fp) != OLDSAVEGAMEVERSION4) { fclose(fp); return false; }
	fgetc(fp);

	// load all structures from the file
	sgrle_initdecompression();
	sgrle_decompressV1(fp, (unsigned char *)&numplayers, sizeof(numplayers));
	sgrle_decompressV1(fp, (unsigned char *)&old.LevelControl, sizeof(old.LevelControl));
	sgrle_decompressV1(fp, (unsigned char *)&old.scroll_x, sizeof(old.scroll_x));
	sgrle_decompressV1(fp, (unsigned char *)&old.scroll_y, sizeof(old.scroll_y));
	sgrle_decompressV1(fp, (unsigned char *)&old.max_scroll_x, sizeof(old.max_scroll_x));
	sgrle_decompressV1(fp, (unsigned char *)&old.max_scroll_y, sizeof(old.max_scroll_y));
	sgrle_decompressV1(fp, (unsigned char *)&old.map, sizeof(old.map));

	//initgame( &(pCKP->Control.levelcontrol) ); // reset scroll
	//drawmap();
	//for(i=0;i<scrx;i++) map_scroll_right();
	//for(i=0;i<scry;i++) map_scroll_down();

	sgrle_decompressV1(fp, (unsigned char *)&old.Player, sizeof(old.Player));

	//sgrle_decompress(fp, (unsigned char *)&objects[0], sizeof(objects));
	//sgrle_decompress(fp, (unsigned char *)&tiles[0], sizeof(tiles));

	fclose(fp);

	return true;
}

// Converts one old savegame file to the new format...
bool CSaveGameController::convertOldFormat(size_t slot)
{
	// TODO: Old CG 0.3.0.4 Code Handle with care
	std::string fname;
	int version;

	fname = "ep";
	fname += itoa(m_Episode);
	fname += "save";
	fname += itoa(slot);
	fname += ".dat";

	if ( (version = getOldSGVersion(fname)) == 0 )
		return false;

	size_t newslot = slot;
	while(Fileexists(newslot))
		newslot++;

	prepareSaveGame(newslot, "oldsave"+itoa(slot));

	if(alreadyExits())
	{
		gLogging.textOut("You already have \""+m_statefilename+"\". If you want to export an old savegame erase it, or erase the old savegame if it's already exported!" );
		return false;
	}

	if(version == 5)
	{
		OldSaveGameFormatV5 old;

		if(!loadSaveGameVersion5(fname, old)) return false;

		// Rename the old save game to the extension bak, so it won't be converted again
		std::string newfname = fname.substr(0,fname.size()-3) + "bak";
		Rename(fname, newfname);

		//
		// Now let's save it into a new format
		//
		/// Save the Game in the CSaveGameController object
		// store the episode, level and difficulty
		encodeData(old.LevelControl.episode);
		encodeData(old.LevelControl.curlevel);
		encodeData(old.LevelControl.hardmode ? 2 : 1);

		// Also the last checkpoint is stored. This is the level entered from map
		// in Commander Keen games
		encodeData(false); // No checkpoint set
		encodeData(0); // Checkpoint X set to zero
		encodeData(0); // Checkpoint Y set to zero
		encodeData(old.LevelControl.dark);

		// Save number of Players
		encodeData(1);

		// Now save the inventory of every player
		encodeData(old.Player.x);
		encodeData(old.Player.y);
		encodeData(old.Player.blockedd);
		encodeData(old.Player.blockedu);
		encodeData(old.Player.blockedl);
		encodeData(old.Player.blockedr);
		encodeData(old.Player.inventory);

		// save the number of objects on screen.
		encodeData(0);

		// Save the map_data as it is left
		encodeData(old.map.xsize);
		encodeData(old.map.ysize);

		word *mapdata = new word[old.map.xsize*old.map.ysize];
		for( size_t x=0 ; x<old.map.xsize ; x++ )
		{
			for( size_t y=0 ; y<old.map.ysize ; y++ )
			{
				mapdata[y*old.map.xsize+x] = old.map.mapdata[x][y];
			}
		}
		addData( (byte*)(mapdata), 2*old.map.xsize*old.map.ysize );

		delete [] mapdata;

		// store completed levels
		addData( (byte*)(old.LevelControl.levels_completed), MAX_LEVELS_VORTICON );

		gLogging.ftextOut("Structures restored: map size: %d,%d and saved\n", old.map.xsize, old.map.ysize);
	}
	else if(version == 4)
	{
		OldSaveGameFormatV4 old;

		if(!loadSaveGameVersion4(fname, old)) return false;

		// Rename the old save game to the extension bak, so it won't be converted again
		std::string newfname = fname.substr(0,fname.size()-3) + "bak";
		Rename(fname, newfname);

		//
		// Now let's save it into a new format
		//
		/// Save the Game in the CSaveGameController object
		// store the episode, level and difficulty
		encodeData(old.LevelControl.episode);
		encodeData(old.LevelControl.curlevel);
		encodeData(old.LevelControl.hardmode ? 2 : 1);

		// Also the last checkpoint is stored. This is the level entered from map
		// in Commander Keen games
		encodeData(false); // No checkpoint set
		encodeData(0); // Checkpoint X set to zero
		encodeData(0); // Checkpoint Y set to zero
		encodeData(old.LevelControl.dark);

		// Save number of Players
		encodeData(1);

		// Now save the inventory of every player
		encodeData(old.Player.x);
		encodeData(old.Player.y);
		encodeData(old.Player.blockedd);
		encodeData(old.Player.blockedu);
		encodeData(old.Player.blockedl);
		encodeData(old.Player.blockedr);
		encodeData(old.Player.inventory);

		// save the number of objects on screen.
		encodeData(0);

		// Save the map_data as it is left
		encodeData(old.map.xsize);
		encodeData(old.map.ysize);

		word *mapdata = new word[old.map.xsize*old.map.ysize];
		for( size_t x=0 ; x<old.map.xsize ; x++ )
		{
			for( size_t y=0 ; y<old.map.ysize ; y++ )
			{
				mapdata[y*old.map.xsize+x] = old.map.mapdata[x][y];
			}
		}
		addData( (byte*)(mapdata), 2*old.map.xsize*old.map.ysize );

		delete [] mapdata;

		// store completed levels
		addData( (byte*)(old.LevelControl.levels_completed), MAX_LEVELS_VORTICON );

		gLogging.ftextOut("Structures restored: map size: %d,%d and saved\n", old.map.xsize, old.map.ysize);
	}
	else
	{
		gLogging.ftextOut("Sorry, but the old save game format is unknown\n");
		return false;
	}

	save();

	gLogging.ftextOut("The old savegame has been converted successfully OK\n");

	return true;
}


// this is seperated out of game_load for modularity because menumanager.c
// also uses it, in it's save-game "preview" menu on the load game screen
void CSaveGameController::readOldHeader(FILE *fp, byte *episode, byte *level, byte *lives, byte *num_players)
{
	fseek(fp, SG_HEADERSIZE, SEEK_SET);		// skip past the CKSAVE%c
	*episode = fgetc(fp);
	*level = fgetc(fp);
	*lives = fgetc(fp);
	*num_players = fgetc(fp);
}

/* --- Functions for older savegames END --- */

// From judging the filename it tells you at what position this slot was saved!
Uint32 CSaveGameController::getSlotNumber(const std::string &filename)
{
	int pos = filename.find("cksave") + strlen("cksave");
	std::string buf = filename.substr(pos);
	buf = buf.substr(0, buf.size()-sizeof(".ck"));

	return atoi(buf);
}

std::string CSaveGameController::getExtension(const std::string &filename)
{
    int pos = filename.find("cksave") + strlen("cksave");
    std::string buf = filename.substr(pos);
    buf = buf.substr(buf.find(".")+1);
    buf = buf.substr(0, buf.size()-1);

    return buf;
}

// This method returns the name of the slot
std::string CSaveGameController::getSlotName(const std::string &filename)
{
    char version;
    std::ifstream StateFile;
    std::string SlotName;
    OpenGameFileR( StateFile, filename, std::ofstream::binary );

    // Check Savegame version
    version = StateFile.get();

    if(version != SAVEGAMEVERSION)
    {
        SlotName = "- File Incompatible -";
    }
    else
    {
        // read the slot name
        Uint32 size = StateFile.get();
        std::vector<char> buf(size + 1);
        readData( &buf[0], size, StateFile);
        buf[size] = '\0';
        SlotName = &buf[0];
    }

    StateFile.close();

    return SlotName;
}


// Same but as XML Version
std::string CSaveGameController::getSlotNameXML(const std::string &filename)
{
    /// Create tree
    using boost::property_tree::ptree;
    ptree pt;

    CSaveGameController &savedGame = *(gpSaveGameController);

    m_stateXMLfilename = filename;

    if(!savedGame.loadXMLTree(pt))
        return "";

    /// Load the nodes and retrieve the data as needed
    return pt.get<std::string>("GameState.<xmlattr>.name", "unknown");
}

/**
 * \brief checks if the file of the chosen slot exits
 * \param SaveSlot 	Slot where to check for the file
 * \return true if it exists, else false
 */
bool CSaveGameController::Fileexists( int SaveSlot )
{
	std::string filename = m_savedir + "/cksave"+itoa(SaveSlot)+".ck"+itoa(m_Episode);
	return IsFileAvailable(filename);
}

// This method is called by the menu. It assures that the
// PlayGame instance will call save() and get the right data.
bool CSaveGameController::prepareSaveGame( int SaveSlot, const std::string &Name )
{
	m_statefilename =  m_savedir + "/cksave"+itoa(SaveSlot)+".ck"+itoa(m_Episode);

    m_stateXMLfilename = m_savedir + "/cksave"+itoa(SaveSlot)+".cx"+itoa(m_Episode);

	m_statename = Name;
	m_datablock.clear();

	m_offset = 0;

	gEventManager.add( new SaveGameEvent() );

	return true;
}

// This method is called by the menu. It assures that the
// PlayGame instance will call load() and get the right data.
bool CSaveGameController::prepareLoadGame(int SaveSlot)
{
    const std::string savefile = "cksave" + itoa(SaveSlot) + ".ck"+itoa(m_Episode);
    m_statefilename = JoinPaths(m_savedir, savefile);

    const std::string saveXMLfile = "cksave" + itoa(SaveSlot) + ".cx"+itoa(m_Episode);
    m_stateXMLfilename = JoinPaths(m_savedir, saveXMLfile);

    m_datablock.clear();

    gEventManager.add( new LoadGameEvent() );

	return true;
}


bool CSaveGameController::load()
{
	Uint32 size;
	std::ifstream StateFile;
	std::string fullpath = GetFullFileName(m_statefilename);
	OpenGameFileR( StateFile, m_statefilename, std::ofstream::binary );

    if (!StateFile.is_open())
    {
    	gLogging.textOut("Error loading \"" + fullpath + "\". Please check the status of that file.\n" );
    	return false;
    }

    // Skip the header as we already chose the game
    StateFile.get(); // Skip the version info
    size = StateFile.get(); // get the size of the slotname and...
    for(Uint32 i=0 ; i<size ; i++)	// skip that name string
    	StateFile.get();

    while(!StateFile.eof()) // read it everything in
    	m_datablock.push_back(StateFile.get());

	// TODO: Decompression has still to be done!

	// Now write all the data to the file
	StateFile.close();

	// Done!
	gLogging.textOut("File \""+ fullpath +"\" was sucessfully loaded. Size: "+itoa(m_datablock.size())+"\n");
	m_offset = 0;
	m_statefilename.clear();
	m_statename.clear();

	return true;
}

// This function checks if the file we want to read or save already exists
bool CSaveGameController::alreadyExits()
{
	std::ifstream StateFile;
	std::string fullpath = GetFullFileName(m_statefilename);
	OpenGameFileR( StateFile, m_statefilename, std::ofstream::binary );

    if (!StateFile.is_open())
    	return false;
    else
    {
    	StateFile.close();
    	return true;
    }
}

// This function writes all the data from the CPlayGame and CMenu Instances to a file,
// closes it and flushes the data block.
bool CSaveGameController::save()
{
	std::ofstream StateFile;
	std::string fullpath = GetFullFileName(m_statefilename);
	bool open = OpenGameFileW( StateFile, m_statefilename, std::ofstream::binary );

    if (!open)
    {
    	gLogging.textOut("Error saving \"" + fullpath + "\". Please check the status of that path.\n" );
    	return false;
    }

	// Convert everything to a primitive data structure
	// First pass the header, which is only version,
	// sizeofname and name of slot itself
	Uint32 offset = 0;
	Uint32 size = sizeof(SAVEGAMEVERSION)
				+ sizeof(char)
				+ m_statename.size()*sizeof(char);

	size += m_datablock.size();
	// Headersize + Datablock size
	std::vector<char> primitive_buffer(size);

	// Write the header
	primitive_buffer[offset++] = SAVEGAMEVERSION;
	primitive_buffer[offset++] = m_statename.size();

	for( Uint32 i=0; i<m_statename.size() ; i++ )
	{
		primitive_buffer[offset++] = m_statename[i];
	}

	// Write the collected data block
	std::vector<byte>::iterator pos = m_datablock.begin();
	for( size_t i=0; i<m_datablock.size() ; i++ )
	{
		primitive_buffer[offset++] = *pos;
		pos++;
	}

	// TODO: Compression has still to be done!

	// Now write all the data to the file
    StateFile.write( &primitive_buffer[0], size );
	StateFile.close();

	m_datablock.clear();

	// Done!
	gLogging.textOut("File \""+ fullpath +"\" was sucessfully saved. Size: "+itoa(size)+"\n");
	m_statefilename.clear();
	m_statename.clear();

	return true;
}



bool CSaveGameController::saveXMLTree(boost::property_tree::ptree &pt)
{
    // Write the xml-file
    using boost::property_tree::ptree;
#if BOOST_VERSION >= 105600
    boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
#else
    boost::property_tree::xml_writer_settings<char> settings('\t', 1);
#endif

    // The savegame internal name
    pt.put("GameState.<xmlattr>.name", m_statename);

    std::ofstream StateFile;
    bool open = OpenGameFileW( StateFile, m_stateXMLfilename, std::ofstream::binary );

    if (!open)
    {
        std::string fullpath = GetFullFileName(m_stateXMLfilename);
        gLogging.textOut("Error saving \"" + fullpath + "\". Please check the status of that path.\n" );
        return false;
    }

    write_xml( StateFile, pt, settings );

    return true;
}


bool CSaveGameController::loadXMLTree(boost::property_tree::ptree &pt)
{
    // load the xml-file
    using boost::property_tree::ptree;

    std::ifstream StateFile;
    bool open = OpenGameFileR( StateFile, m_stateXMLfilename, std::ofstream::binary );

    if (!open)
        return false;

    read_xml( StateFile, pt );

    return true;
}





// Adds data of size to the main data block
void CSaveGameController::addData(byte *data, Uint32 size)
{
	for(Uint32 i=0 ; i<sizeof(Uint32) ; i++ )
	{
		Uint32 datasize;
		datasize = size & ( 0xFF<<(i*8) );
		datasize >>= (i*8);
		m_datablock.push_back( static_cast<byte>(datasize) );
	}
	for(Uint32 i=0 ; i<size ; i++ )
		m_datablock.push_back(data[i]);
}

// Read data of size from the main data block
bool CSaveGameController::readDataBlock(byte *data)
{
    if(m_offset+sizeof(Uint32) > m_datablock.size())
	    return false;

	Uint32 datasize=0;
	memcpy(&datasize, &m_datablock[m_offset], sizeof(Uint32) );
	m_offset += sizeof(Uint32);

    if(datasize > m_datablock.size()-m_offset)
        return false;

    if(datasize > 0)
		memcpy(data, &m_datablock[m_offset], datasize);

	m_offset += datasize;
	return true;
}
