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

	m_offset = 0;

	return true;
}

// This method is called by the menu. It assures that the
// PlayGame instance will call load() and get the right data.
bool CSavedGame::prepareLoadGame(int SaveSlot)
{
	m_statefilename = "cksave"+itoa(SaveSlot)+".ck"+itoa(m_Episode);

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

    m_datablock.clear();	// empty the datablock and...
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
	std::vector<char>::iterator pos = m_datablock.begin();
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

CSavedGame::~CSavedGame() {
	// TODO Auto-generated destructor stub
}

