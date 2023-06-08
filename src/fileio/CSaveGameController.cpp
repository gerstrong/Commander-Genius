/*
 * CSaveGameController.cpp
 *
 *  Created on: 13.08.2009
 *      Author		: Caitlin Shaw
 *      Adaptations : Gerstrong
 *  Routines for handling save&load of savegames
 */

#include <base/interface/FindFile.h>
#include <base/GsEventContainer.h>
#include "CSaveGameController.h"
#include "engine/core/CBehaviorEngine.h"

#include <ctime>
#include <base/interface/property_tree/property_tree.h>
#include <base/interface/property_tree/xml_parser.h>

#include "fileio/KeenFiles.h"


void sgrle_initdecompression(void);
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
char sgrle_decompressV2(FILE *fp, unsigned char *ptr, unsigned long nbytes);
void sgrle_decompressV1(FILE *fp, unsigned char *ptr, unsigned long nbytes);

using namespace std;

// Initialization Routines
CSaveGameController::CSaveGameController()
{
    auto &keenFiles = gKeenFiles;

    setGameDirectory(keenFiles.gameDir);
    setEpisode(gBehaviorEngine.getEpisode());
}

void CSaveGameController::setGameDirectory(const std::string& game_directory)
{
    m_savedir = JoinPaths("save", game_directory);
}

void CSaveGameController::setEpisode(const int Episode)
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

bool CSaveGameController::hasQuickloadGame()
{
    const std::string saveXMLfile = "cksaveQuick.cx" + itoa(m_Episode);
    const auto stateXMLfilename = JoinPaths(m_savedir, saveXMLfile);
    const bool exists = IsFileAvailable(stateXMLfilename);
    return exists;
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
    {
        list.clear();
    }

    //Get the list of ".ck?" and ".cx?" files
    StateFileListFiller sfilelist;
    gLogging.ftextOut("Reading savegames from \"%s\".", m_savedir.c_str());
    gLogging << CLogFile::endl;
    FindFiles(sfilelist, m_savedir, false, FM_REG);

    for( const std::string &filename : sfilelist.list )
    {
        std::string buf = filename.substr(filename.size()-1);

        const int foundEp = atoi(buf.c_str());

        // Check if the file fits to this episode
        if(foundEp == m_Episode)
        {
            int pos = getSlotNumber(filename);

            // Save number start with "1". Zero is not allowed
            if(pos <= 0)
                continue;

            if(pos > gSaveGameController.getMaxNumofSaveSlots())
                continue;

            const std::string ext = getExtension(filename);

            if(ext == "ck")
                buf = getSlotName(filename);
            else if(ext == "cx")
                buf = getSlotNameXML(filename);

            const auto index = pos-1;

            if(index >= int(list.size()))
                list.resize(index+1);

            list.at(index) = buf;
        }
    }

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

// this is seperated out of game_load for modularity because menumanager.c
// also uses it, in it's save-game "preview" menu on the load game screen
void CSaveGameController::readOldHeader(FILE *fp, gs_byte *episode, gs_byte *level, gs_byte *lives, gs_byte *num_players)
{
    fseek(fp, SG_HEADERSIZE, SEEK_SET);		// skip past the CKSAVE%c
    *episode = fgetc(fp);
    *level = fgetc(fp);
    *lives = fgetc(fp);
    *num_players = fgetc(fp);
}

/* --- Functions for older savegames END --- */

#include <algorithm>

bool is_number(const std::string &s)
{
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

// From judging the filename it tells you at what position this slot was saved!
Uint32 CSaveGameController::getSlotNumber(const std::string &filename)
{
    const auto ckSavePos = filename.find("cksave");

    // Filename must contain "cksave"
    if(ckSavePos == filename.npos)
        return -1;

    const int pos = ckSavePos + strlen("cksave");

    std::string buf = filename.substr(pos);
    buf = buf.substr(0, buf.size()-sizeof(".ck"));

    if(!is_number(buf))
    {
        return -2;
    }

    return atoi(buf.c_str());
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
    using GsKit::ptree;
    ptree pt;

    m_stateXMLfilename = filename;

    if(!gSaveGameController.loadXMLTree(pt))
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

void CSaveGameController::prepareSaveGameQuick()
{
    m_stateXMLfilename = m_savedir + "/cksaveQuick.cx"+itoa(m_Episode);

    m_statename = "Quicksave";
    m_datablock.clear();

    m_offset = 0;

    gEventManager.add( new SaveGameEvent() );
}

// This method is called by the menu. It assures that the
// PlayGame instance will call save() and get the right data.
void CSaveGameController::prepareSaveGame( int SaveSlot, const std::string &Name )
{
    m_statefilename =  m_savedir + "/cksave"+itoa(SaveSlot)+".ck"+itoa(m_Episode);
    m_stateXMLfilename = m_savedir + "/cksave"+itoa(SaveSlot)+".cx"+itoa(m_Episode);

    m_statename = Name;
    m_datablock.clear();

    m_offset = 0;

    gEventManager.add( new SaveGameEvent() );
}


// This method is called by the menu. It assures that the
// PlayGame instance will call load() and get the right data.
bool CSaveGameController::prepareLoadGame(const int SaveSlot)
{
    const std::string savefile = "cksave" + itoa(SaveSlot) + ".ck"+itoa(m_Episode);
    m_statefilename = JoinPaths(m_savedir, savefile);

    const std::string saveXMLfile = "cksave" + itoa(SaveSlot) + ".cx"+itoa(m_Episode);
    m_stateXMLfilename = JoinPaths(m_savedir, saveXMLfile);

    gLogging << "Trying to load legacy savegame file: " << m_statefilename << CLogFile::endl;
    gLogging << "Trying to load xml savegame file: " << m_stateXMLfilename << CLogFile::endl;

    m_datablock.clear();

    gEventManager.add( new LoadGameEvent() );

    return true;
}

bool CSaveGameController::prepareLoadGameQuick()
{
    const std::string saveXMLfile = "cksaveQuick.cx"+itoa(m_Episode);
    m_stateXMLfilename = JoinPaths(m_savedir, saveXMLfile);

    gLogging << "Trying to load quicksave file: " << m_stateXMLfilename << CLogFile::endl;

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
    {
        StateFile.get();
    }

    while(!StateFile.eof())
    {   // read it everything in
        m_datablock.push_back(StateFile.get());
    }

    // TODO: Decompression has still to be done!

    // Now write all the data to the file
    StateFile.close();

    // Done!
    gLogging.textOut("File \""+ fullpath +"\" was successfully loaded. Size: "+itoa(m_datablock.size())+"\n");
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
    std::vector<gs_byte>::iterator pos = m_datablock.begin();
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
    gLogging.textOut("File \""+ fullpath +"\" was successfully saved. Size: "+itoa(size)+"\n");
    m_statefilename.clear();
    m_statename.clear();

    return true;
}



bool CSaveGameController::saveXMLTree(GsKit::ptree &pt)
{
    // Write the xml-file
    using GsKit::ptree;

    // The savegame internal name
    pt.put("GameState.<xmlattr>.name", m_statename);

#ifdef BOOST_ENABLED
    std::ofstream StateFile;
    bool open = OpenGameFileW( StateFile, m_stateXMLfilename, std::ofstream::binary );

    if (!open)
    {
        std::string fullpath = GetFullFileName(m_stateXMLfilename);
        gLogging.textOut("Error saving \"" + fullpath + "\". Please check the status of that path.\n" );
        return false;
    }

    write_xml( StateFile, pt );
#else
    write_xml(m_stateXMLfilename, pt);
#endif

    return true;
}


bool CSaveGameController::loadXMLTree(GsKit::ptree &pt)
{
    // load the xml-file
    using GsKit::ptree;

    std::ifstream StateFile;
    bool open = OpenGameFileR( StateFile, m_stateXMLfilename, std::ofstream::binary );

    if (!open)
        return false;

    read_xml( m_stateXMLfilename, pt );

    return true;
}





// Adds data of size to the main data block
void CSaveGameController::addData(gs_byte *data, Uint32 size)
{
    for(Uint32 i=0 ; i<sizeof(Uint32) ; i++ )
    {
        Uint32 datasize;
        datasize = size & ( 0xFF<<(i*8) );
        datasize >>= (i*8);
        m_datablock.push_back( static_cast<gs_byte>(datasize) );
    }
    for(Uint32 i=0 ; i<size ; i++ )
        m_datablock.push_back(data[i]);
}

// Read data of size from the main data block
bool CSaveGameController::readDataBlock(gs_byte *data)
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
