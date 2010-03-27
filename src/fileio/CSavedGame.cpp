/*
 * CSavedGame.cpp
 *
 *  Created on: 13.08.2009
 *      Author:      Caitlin Shaw
 *      Adaptation : Gerstrong
 *  Routines for handling save&load of savegames
 */

#include "../FindFile.h"

#include "CSavedGame.h"
#include "Oldsavegamestructs.h"

#define SG_HEADERSIZE			7
#define SAVEGAMEVERSION 		'6'

void sgrle_initdecompression(void);
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
char sgrle_decompress(FILE *fp, unsigned char *ptr, unsigned long nbytes);

// Initialization Routines
CSavedGame::CSavedGame() {
	m_Command = NONE;
	m_offset = 0;
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
	for( i=sfilelist.list.begin() ; i!=sfilelist.list.end() ; i++ )
	{
		buf = i->substr(i->size()-1);

		// Check if the file fits to this episode
		if(atoi(buf) == m_Episode)
		{
			Uint32 pos = getSlotNumber(*i)-1;
			buf = getSlotName(*i);

			if(pos+1 > filelist.size())
				filelist.resize(pos+1, "");

			filelist.at(pos) = buf;
		}
	}

	return filelist;
}

/* --- Functions for older savegames START --- */

// This function converts savegame all files from old versions of CG to the new format
void CSavedGame::convertAllOldFormats()
{
	for(size_t slot=0 ; slot<=9 ; slot++ )
		convertOldFormat(slot);
}

// Converts one old savegame file to the new format...
bool CSavedGame::convertOldFormat(size_t slot)
{
	// TODO: Old CG 0.3.0.4 Code Handle with care
	FILE *fp;
	std::string fname;
	unsigned char episode, level, lives, numplayers;
	//unsigned int i;
	OldSaveGameFormat old;

	fname = "ep";
	fname += m_Episode + '0';
	fname += "save";
	fname += slot+'0';
	fname += ".dat";

	if ( !IsOldButValidSaveGame(fname) )
	{
		g_pLogFile->ftextOut("%s is not a valid save-game.\n", fname.c_str());
		return false;
	}

	g_pLogFile->ftextOut("Loading game from file %s\n", fname.c_str());
	fp = OpenGameFile(fname, "rb");
	if (!fp) { g_pLogFile->ftextOut("unable to open %s\n",fname.c_str()); return false; }

	readOldHeader(fp, &episode, &level, &lives, &numplayers);

	g_pLogFile->ftextOut("game_load: restoring structures...\n");
	/*primaryplayer =*/ fgetc(fp); // primary player doesn't exist anymore! Jump that!

	sgrle_compress(fp, (unsigned char *) &old.LevelControl, sizeof(old.LevelControl));

	// note that we don't have to load the LEVEL, because the state
	// of the map is already saved inside the save-game.
	sgrle_initdecompression();
	if (sgrle_decompress(fp, (unsigned char *) &old.LevelControl, sizeof(old.LevelControl))) return false;

	/*if (sgrle_decompress(fp, (unsigned char *)&scroll_x, sizeof(scroll_x))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&scrollx_buf, sizeof(scrollx_buf))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&scrollpix, sizeof(scrollpix))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&mapx, sizeof(mapx))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&mapxstripepos, sizeof(mapxstripepos))) return false;

	if (sgrle_decompress(fp, (unsigned char *)&scroll_y, sizeof(scroll_y))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&scrolly_buf, sizeof(scrolly_buf))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&scrollpixy, sizeof(scrollpixy))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&mapy, sizeof(mapy))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&mapystripepos, sizeof(mapystripepos))) return false;

	if (sgrle_decompress(fp, (unsigned char *)&max_scroll_x, sizeof(max_scroll_x))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&max_scroll_y, sizeof(max_scroll_y))) return false;

	if (sgrle_decompress(fp, (unsigned char *)&map, sizeof(map))) return false;

	highest_objslot = fgeti(fp);
	if (sgrle_decompress(fp, (unsigned char *)&objects[0], sizeof(objects))) return false;
	if (sgrle_decompress(fp, (unsigned char *)&tiles[0], sizeof(tiles))) return false;

	for(i=0;i<numplayers;i++)
	{
		if (sgrle_decompress(fp, (unsigned char *)&player[i], sizeof(player[i]))) return false;
	}

	CSprite **sprites = &g_pGfxEngine->Sprite[0];
	sprites[DOOR_YELLOW_SPRITE]->setHeight(fgetc(fp));
	sprites[DOOR_RED_SPRITE]->setHeight(fgetc(fp));
	sprites[DOOR_GREEN_SPRITE]->setHeight(fgetc(fp));
	sprites[DOOR_BLUE_SPRITE]->setHeight(fgetc(fp));

	fclose(fp);

	g_pLogFile->ftextOut("Structures restored: map size: %d,%d\n", map.xsize, map.ysize);
	g_pLogFile->ftextOut("Load game OK\n");*/
	return true;
}

char CSavedGame::IsOldButValidSaveGame(std::string fname)
{
FILE *fp;
unsigned int i;
const char *verify = "CKSAVE";
	fp = OpenGameFile(fname, "rb");
	if (!fp) return 0;

	for(i=0;i<strlen(verify);i++)
	{
		if (fgetc(fp) != verify[i])
		{
			fclose(fp);
			return 0;
		}
	}
	if (fgetc(fp) != SAVEGAMEVERSION)
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

// this is seperated out of game_load for modularity because menumanager.c
// also uses it, in it's save-game "preview" menu on the load game screen
void CSavedGame::readOldHeader(FILE *fp, uchar *episode, uchar *level, uchar *lives, uchar *num_players)
{
	fseek(fp, SG_HEADERSIZE, SEEK_SET);		// skip past the CKSAVE%c
	*episode = fgetc(fp);
	*level = fgetc(fp);
	*lives = fgetc(fp);
	*num_players = fgetc(fp);
}

/* --- Functions for older savegames END --- */

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
		delete [] buf;
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
	m_datablock.clear();

	// This will make the CPlayGame instance call save
	m_Command = SAVE;

	m_offset = 0;

	return true;
}

// This method is called by the menu. It assures that the
// PlayGame instance will call load() and get the right data.
bool CSavedGame::prepareLoadGame(int SaveSlot)
{
	m_statefilename = "cksave"+itoa(SaveSlot)+".ck"+itoa(m_Episode);
    m_datablock.clear();

	// This will make the CPlayGame instance call save
	m_Command = LOAD;

	return true;
}

bool CSavedGame::load()
{
	Uint32 size;
	std::ifstream StateFile;
	std::string fullpath = GetFullFileName(m_statefilename);
	OpenGameFileR( StateFile, m_statefilename, std::ofstream::binary );

    if (!StateFile.is_open()) {
    	g_pLogFile->textOut("Error loading \"" + fullpath + "\". Please check the status of that file.\n" );
    	return false;
    }

    // Skip the header as we already chose the game
    StateFile.get(); // Skip the version info
    size = StateFile.get(); // get the size of the slotname and...
    for(Uint32 i=0 ; i<size ; i++)	// skip that name string
    	StateFile.get();

    while(!StateFile.eof()) // read it everything in
    {
    	m_datablock.push_back(StateFile.get());
    }

	// TODO: Decompression has still to be done!

	// Now write all the data to the file
	StateFile.close();

	// Done!
	g_pLogFile->textOut("File \""+ fullpath +"\" was sucessfully loaded. Size: "+itoa(m_datablock.size())+"\n");
	m_Command = NONE;
	m_offset = 0;
	m_statefilename.clear();
	m_statename.clear();

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
	std::vector<uchar>::iterator pos = m_datablock.begin();
	for( Uint32 i=0; i<m_datablock.size() ; i++ ){
		primitive_buffer[offset++] = *pos;
		pos++;
	}

	// TODO: Compression has still to be done!

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
}

// Adds data of size to the main data block
void CSavedGame::addData(uchar *data, Uint32 size) {
	for(Uint32 i=0 ; i<sizeof(Uint32) ; i++ )
	{
		Uint32 datasize;
		datasize = size&( 0xFF<<(i*8) );
		datasize >>= (i*8);
		m_datablock.push_back( static_cast<uchar>(datasize) );
	}
	for(Uint32 i=0 ; i<size ; i++ )
		m_datablock.push_back(data[i]);
}

// Read data of size from the main data block
void CSavedGame::readDataBlock(uchar *data) {
	Uint32 datasize=0;
	memcpy(&datasize, &m_datablock[m_offset], sizeof(Uint32));
	m_offset += sizeof(Uint32);

	memcpy(data, &m_datablock[m_offset], datasize);
	m_offset += datasize;
}

CSavedGame::~CSavedGame() {
	// TODO Auto-generated destructor stub
}
