/*
 * CSavedGame.cpp
 *
 *  Created on: 13.08.2009
 *      Author:      Caitlin Shaw
 *      Adaptation : Gerstrong
 *  Routines for handling save&load of savegames
 */

#include "../keen.h"
#include "../externals.h"
#include "../game.h"
#include "../CLogFile.h"
#include "../graphics/CGfxEngine.h"
#include "../FindFile.h"
#include "../CLogFile.h"

#include "CSavedGame.h"

#define SG_HEADERSIZE			7
#define SAVEGAMEVERSION 		'6'

void sgrle_initdecompression(void);
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
char sgrle_decompress(FILE *fp, unsigned char *ptr, unsigned long nbytes);

void initgame(stLevelControl *p_levelcontrol);

// Initialization Routines
CSavedGame::CSavedGame() {
	m_Command = NONE;
}

void CSavedGame::setEpisode(char Episode){
	m_Episode = Episode;
}

// Retrieves the data size of the next block
Uint32 CSavedGame::getDataSize(std::ifstream &StateFile) {
	Uint32 size=0;
	for(Uint32 i=0 ; i<sizeof(Uint32) ; i++) {
		size += StateFile.get() << (i*8);
	}
	return size;
}

// Read the data of size and stores it in the buffer
void CSavedGame::readData(char *buffer, Uint32 size, std::ifstream &StateFile) {
	for(Uint32 i=0 ; i<size ; i++) {
		buffer[i] = StateFile.get();
	}
}


// Adds data of size to the main data block
void CSavedGame::addData(uchar *data, Uint32 size) {
	for(Uint32 i=0 ; i<sizeof(Uint32) ; i++ )
		m_datablock.push_back((size>>(i*8))&0xFF);
	for(Uint32 i=0 ; i<size ; i++ )
		m_datablock.push_back(data[i]);
}

// Used here for filtering the filetypes
struct StateFileListFiller {
	std::set<std::string> list;

	bool operator() (const std::string& filename) {
		std::string ext = GetFileExtension(filename);
		if (stringcaseequal(ext, "ck1") ||
			stringcaseequal(ext, "ck2") ||
			stringcaseequal(ext, "ck3") )
			list.insert(filename);

		return true;
	}
};
// This method returns the the list of files we can use for the menu
// It will filter by episode and sort the list by number of slot
// It also takes care of the slotname resolution
std::vector<std::string> CSavedGame::getSlotList()
{
	std::vector<std::string> filelist;
	std::string buf;

	//Get the list of ".ck?" files
	StateFileListFiller sfilelist;
	FindFiles(sfilelist, "", false, FM_REG);

	std::set<std::string>::iterator i;
	Uint32 slot_counter = 1;
	for( i=sfilelist.list.begin() ; i!=sfilelist.list.end() ; i++ )
	{
		buf = i->substr(i->size()-1);

		// Check if the file fits to this episode
		if(atoi(buf) == m_Episode)
		{
			buf = getSlotName(*i);

			while(getSlotNumber(*i) != slot_counter)
			{
				filelist.push_back("");
				slot_counter++;
			}

			filelist.push_back(buf);
			slot_counter++;
		}
	}

	return filelist;
}

// From judging the filename it tells you at what position this slot was saved!
Uint32 CSavedGame::getSlotNumber(const std::string &filename)
{
	int pos = filename.find("cksave") + strlen("cksave");
	std::string buf = filename.substr(pos);
	buf = buf.substr(0, buf.size()-sizeof(".ck"));

	return atoi(buf);
}

// This method returns the name of the slot
std::string CSavedGame::getSlotName(const std::string &filename)
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
		char *buf;
		buf = new char[size+1];
		readData( buf, size, StateFile);
		buf[size] = '\0';
		SlotName = buf;
		delete buf;
	}

	StateFile.close();

	return SlotName;
}

// This method is called by the menu. It assures that the
// PlayGame instance will call save() and get the right data.
bool CSavedGame::prepareSaveGame( int SaveSlot, const std::string &Name)
{
	m_statefilename = "cksave"+itoa(SaveSlot)+".ck"+itoa(m_Episode);
	m_statename = Name;

	// This will make the CPlayGame instance call save
	m_Command = SAVE;

	return true;
}

// This function writes all the data from the CPlayGame and CMenu Instances to a file,
// closes it and flushes the data block.
bool CSavedGame::save()
{
	std::ofstream StateFile;
	std::string fullpath = GetFullFileName(m_statefilename);
	OpenGameFileW( StateFile, m_statefilename, std::ofstream::binary );

    if (!StateFile.is_open()) {
    	g_pLogFile->textOut("Error saving \"" + fullpath + "\". Please check the status of that path.\n" );
    	return false;
    }

	// Save the name of the gamestate (not the filename)
    // NOTE: This part stays uncompressed in future, so the list of the menu is generated
    // in no time!
	addData( (uchar*)m_statename.c_str(), m_statename.size() );

	// TODO: No go! This data must be written as header...

	// TODO: Compression has still to be done!

	// Convert everything to a primitive data structure
	// First pass the header, which is only version,
	// sizeofname and name of slot itself
	Uint32 offset = 0;
	Uint32 size = sizeof(SAVEGAMEVERSION)
				+ sizeof(char)
				+ m_statename.size()*sizeof(char);

	size += m_datablock.size();
	// Headersize + Datablock size
	char *primitive_buffer = new char[size];

	// Write the header
	primitive_buffer[offset++] = SAVEGAMEVERSION;
	primitive_buffer[offset++] = m_statename.size();

	for( Uint32 i=0; i<m_statename.size() ; i++ ){
		primitive_buffer[offset++] = m_statename[i];
	}

	// Write the collected data block
	std::vector<char>::iterator pos = m_datablock.begin();
	for( Uint32 i=0; i<m_datablock.size() ; i++ ){
		primitive_buffer[offset++] = *pos;
		pos++;
	}

	// Now write all the data to the file
    StateFile.write( primitive_buffer, size );
	StateFile.close();

	m_datablock.clear();

	// Done!
	g_pLogFile->textOut("File \""+ fullpath +"\" was sucessfully saved. Size: "+itoa(size)+"\n");
	m_Command = NONE;
	m_statefilename.clear();
	m_statename.clear();

	return true;

	// Depreciated Code
	/*FILE *fp;
	 std::string fname;
	 
	 fname = "";
	 fname += DataDirectory;
	 fname += "/ep";
	 fname += mp_levelcontrol->episode + '0';
	 fname += "save";
	 fname += slot+'0';
	 fname += ".dat";
	 
	 g_pLogFile->textOut("Saving game...\n");
	 fp = OpenGameFile(fname, "wb");
	 if (!fp) return 1;*/

	 /*fprintf(fp, "CKSAVE%c", SAVEGAMEVERSION);
	 fputc(mp_levelcontrol->episode, fp);
	 fputc(mp_levelcontrol->curlevel, fp);
	 //fputc(player[0].inventory.lives, fp);
	 fputc(numplayers, fp);
	 fputc(primaryplayer, fp);
	 fputc(name, fp);
	 
	 sgrle_compress(fp, (unsigned char *) mp_levelcontrol, sizeof(*mp_levelcontrol));
	 sgrle_compress(fp, (unsigned char *)&scroll_x, sizeof(scroll_x));
	 sgrle_compress(fp, (unsigned char *)&scrollx_buf, sizeof(scrollx_buf));
	 sgrle_compress(fp, (unsigned char *)&scrollpix, sizeof(scrollpix));
	 sgrle_compress(fp, (unsigned char *)&mapx, sizeof(mapx));
	 sgrle_compress(fp, (unsigned char *)&mapxstripepos, sizeof(mapxstripepos));
	 
	 sgrle_compress(fp, (unsigned char *)&scroll_y, sizeof(scroll_y));
	 sgrle_compress(fp, (unsigned char *)&scrolly_buf, sizeof(scrolly_buf));
	 sgrle_compress(fp, (unsigned char *)&scrollpixy, sizeof(scrollpixy));
	 sgrle_compress(fp, (unsigned char *)&mapy, sizeof(mapy));
	 sgrle_compress(fp, (unsigned char *)&mapystripepos, sizeof(mapystripepos));
	 
	 sgrle_compress(fp, (unsigned char *)&max_scroll_x, sizeof(max_scroll_x));
	 sgrle_compress(fp, (unsigned char *)&max_scroll_y, sizeof(max_scroll_y));
	 
	 sgrle_compress(fp, (unsigned char *)&map, sizeof(map));
	 
	 fputi(highest_objslot, fp);
	 //sgrle_compress(fp, (unsigned char *)&objects[0], sizeof(objects));
	 sgrle_compress(fp, (unsigned char *)&tiles[0], sizeof(tiles));
	 
	 for(unsigned i=0;i<numplayers;i++)
	 //sgrle_compress(fp, (unsigned char *)&player[i], sizeof(player[i]));
	 
	 // save state of partially-opened doors
	 CSprite **sprites = &g_pGfxEngine->Sprite[0];
	 
	 fputc(sprites[DOOR_YELLOW_SPRITE]->getWidth(), fp);
	 fputc(sprites[DOOR_RED_SPRITE]->getWidth(), fp);
	 fputc(sprites[DOOR_GREEN_SPRITE]->getWidth(), fp);
	 fputc(sprites[DOOR_BLUE_SPRITE]->getWidth(), fp);
	 
	 fclose(fp);*/
}


bool CSavedGame::IsValidSaveGame(std::string fname)
{
	/*FILE *fp;
	unsigned int i;
	const char *verify = "CKSAVE";
	fp = OpenGameFile(fname, "rb");
	if (!fp) return false;
	
	for(i=0;i<strlen(verify);i++)
	{
		if (fgetc(fp) != verify[i])
		{
			fclose(fp);
			return false;
		}
	}
	if (fgetc(fp) != SAVEGAMEVERSION)
	{
		fclose(fp);
		return false;
	}
	fclose(fp);*/
	return true;
}

// this is seperated out of game_load for modularity because menumanager.c
// also uses it, in it's save-game "preview" menu on the load game screen
void CSavedGame::readHeader(FILE *fp, uchar *episode, uchar *level, uchar *num_players, uchar *primary_player, uchar *name)
{
	fseek(fp, SG_HEADERSIZE, SEEK_SET);		// skip past the CKSAVE%c
	*episode = fgetc(fp);
	*level = fgetc(fp);
	*num_players = fgetc(fp);
	*primary_player = fgetc(fp);
	*name = fgetc(fp);
}

/*void CSavedGame::loadGame(int slot)
{
	/*FILE *fp;
	 std::string fname;
	 unsigned char episode, level, lives;
	 unsigned int i;
	 
	 fname = "";
	 fname += DataDirectory;
	 fname += "/ep";
	 fname += mp_levelcontrol->episode + '0';
	 fname += "save";
	 fname += slot+'0';
	 fname += ".dat";
	 
	 if (!IsValidSaveGame(fname))
	 {
	 g_pLogFile->ftextOut("%s is not a valid save-game.\n", fname.c_str());
	 return false;
	 }
	 
	 g_pLogFile->ftextOut("Loading game from file %s\n", fname.c_str());
	 fp = OpenGameFile(fname, "rb");
	 if (!fp) { g_pLogFile->ftextOut("unable to open %s\n",fname.c_str()); return 1; }
	 
	 readHeader(fp, &episode, &level, &lives, &numplayers);
	 
	 mp_levelcontrol->episode = episode;
	 mp_levelcontrol->curlevel = level;
	 //player[0].inventory.lives = lives;
	 
	 g_pLogFile->ftextOut("game_load: restoring structures...\n");
	 primaryplayer = fgetc(fp);
	 
	 sgrle_compress(fp, (unsigned char *) mp_levelcontrol, sizeof(*mp_levelcontrol));
	 
	 // note that we don't have to load the LEVEL, because the state
	 // of the map is already saved inside the save-game.
	 sgrle_initdecompression();
	 if (sgrle_decompress(fp, (unsigned char *) mp_levelcontrol, sizeof(*mp_levelcontrol))) return 1;
	 
	 if (sgrle_decompress(fp, (unsigned char *)&scroll_x, sizeof(scroll_x))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&scrollx_buf, sizeof(scrollx_buf))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&scrollpix, sizeof(scrollpix))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&mapx, sizeof(mapx))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&mapxstripepos, sizeof(mapxstripepos))) return 1;
	 
	 if (sgrle_decompress(fp, (unsigned char *)&scroll_y, sizeof(scroll_y))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&scrolly_buf, sizeof(scrolly_buf))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&scrollpixy, sizeof(scrollpixy))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&mapy, sizeof(mapy))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&mapystripepos, sizeof(mapystripepos))) return 1;
	 
	 if (sgrle_decompress(fp, (unsigned char *)&max_scroll_x, sizeof(max_scroll_x))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&max_scroll_y, sizeof(max_scroll_y))) return 1;
	 
	 if (sgrle_decompress(fp, (unsigned char *)&map, sizeof(map))) return 1;
	 
	 highest_objslot = fgeti(fp);
	 //if (sgrle_decompress(fp, (unsigned char *)&objects[0], sizeof(objects))) return 1;
	 if (sgrle_decompress(fp, (unsigned char *)&tiles[0], sizeof(tiles))) return 1;
	 
	 for(i=0;i<numplayers;i++)
	 {
	 //if (sgrle_decompress(fp, (unsigned char *)&player[i], sizeof(player[i]))) return 1;
	 }
	 
	 CSprite **sprites = &g_pGfxEngine->Sprite[0];
	 sprites[DOOR_YELLOW_SPRITE]->setHeight(fgetc(fp));
	 sprites[DOOR_RED_SPRITE]->setHeight(fgetc(fp));
	 sprites[DOOR_GREEN_SPRITE]->setHeight(fgetc(fp));
	 sprites[DOOR_BLUE_SPRITE]->setHeight(fgetc(fp));
	 
	 fclose(fp);
	 
	 g_pLogFile->ftextOut("Structures restored: map size: %d,%d\n", map.xsize, map.ysize);
	 g_pLogFile->ftextOut("Load game OK\n");
}*/

CSavedGame::~CSavedGame() {
	// TODO Auto-generated destructor stub
}

