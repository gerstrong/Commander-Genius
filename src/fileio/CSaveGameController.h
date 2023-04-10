/*
 * CSaveGameController.h
 *
 *  Created on: 13.08.2009
 *      Author: gerstrong
 */

#ifndef CSAVEDGAME_H_
#define CSAVEDGAME_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>

#include <base/GsLogging.h>
#include <base/GsEvent.h>
#include <base/interface/StringUtils.h>
#include <base/TypeDefinitions.h>
#include "Oldsavegamestructs.h"

#include <base/Singleton.h>


#define SG_HEADERSIZE			7
#define SAVEGAMEVERSION 		'6'
#define OLDSAVEGAMEVERSION5		'5'
#define OLDSAVEGAMEVERSION4		'4'


#define gSaveGameController CSaveGameController::get()


class CSaveGameController : public GsSingleton<CSaveGameController>
{
public:

	// Initialization
	CSaveGameController();

	// Setters
	void setGameDirectory(const std::string& game_directory);
    void setEpisode(const int Episode);
	void setLevel(int Level);

    bool readSlotList(std::vector<std::string> &list);

    //bool convertOldFormat(size_t slot);
    //void convertAllOldFormats();
	void readOldHeader(FILE *fp, gs_byte *episode, gs_byte *level, gs_byte *lives, gs_byte *num_players);
	Uint32 getSlotNumber(const std::string &filename);
    std::string getExtension(const std::string &filename);
	std::string getSlotName(const std::string &filename);
    std::string getSlotNameXML(const std::string &filename);
	Uint32 getDataSize(std::ifstream &StateFile);
	void readData(char *buffer, Uint32 size, std::ifstream &StateFile);

	bool Fileexists( int SaveSlot );
    void prepareSaveGameQuick();
    void prepareSaveGame( int SaveSlot, const std::string &Name);
    bool prepareLoadGame(const int SaveSlot );
    bool prepareLoadGameQuick();

    bool saveXMLTree(GsKit::ptree &pt);
    bool loadXMLTree(GsKit::ptree &pt);

	// Encoder/Decoder Classes
	template <class T>
	void encodeVariable(T value);
	template <class S>
    void encodeData(const S structure);
	template <class T>
	void decodeVariable(T &variable);
	template <class S>
	bool decodeData(S &structure);

	void addData(gs_byte *data, Uint32 size);
	bool readDataBlock(gs_byte *data);

    /**
     * @brief save  This function writes all the data from the CPlayGame and CMenu Instances to a file,
     *              closes it and flushes the data block.
     * @return
     */
	bool save();


	bool load();
	bool alreadyExits();
	
	bool busy()
	{ return !m_statefilename.empty(); }

    bool hasQuickloadGame();
	
	std::string getUnnamedSlotName();

    const int getMaxNumofSaveSlots()
    {   return mMaxNumSaves;    }
	
private:

	bool IsOldSGVersion5(const std::string& fname);
	bool IsOldSGVersion4(const std::string& fname);
	int getOldSGVersion(const std::string& fname);

	std::string m_savedir;
    std::string m_statefilename;
    std::string m_stateXMLfilename;
    std::string m_statename;
    int m_Episode;
	int m_Level;
	Uint32 m_offset;	

	std::vector<gs_byte> m_datablock;

    const int mMaxNumSaves = 8;
};


struct SaveGameEvent : public CEvent
{};

struct LoadGameEvent : public CEvent
{};


template <class S>
void CSaveGameController::encodeData(const S structure)
{
    const size_t size = sizeof(S);

    gs_byte sizebuf[sizeof(size_t)];
    gs_byte databuf[sizeof(S)];

    memcpy(sizebuf, &size, sizeof(size_t));
    for( Uint32 i=0 ; i<sizeof(size_t) ; i++ )
    {
        m_datablock.push_back( sizebuf[i] );
    }

    memcpy(databuf, &structure, size);
    for( Uint32 i=0 ; i<size ; i++ )
    {
        m_datablock.push_back( databuf[i] );
    }
}

template <class S>
bool CSaveGameController::decodeData(S &structure)
{
    size_t det_size = 0;
    size_t req_size = sizeof(S);

    const auto datablockSize = m_datablock.size();

    if(m_offset+sizeof(size_t) > datablockSize)
        return false;

    memcpy(&det_size, &m_datablock.at(m_offset), sizeof(size_t));
    m_offset += sizeof(size_t);

    size_t copySize = 0;

    if( det_size > req_size ) // This implementation is used for avoiding possible errors
    {
        if(m_offset+req_size > datablockSize)
        {
            return false;
        }

        copySize = req_size;
    }
    else
    {
        if(m_offset+det_size > datablockSize)
        {
            return false;
        }

        copySize = det_size;
    }

    void *dataPtr = &structure;
    memcpy(dataPtr, &m_datablock.at(m_offset), copySize);

    m_offset += det_size;

    return true;
}


#endif /* CSAVEDGAME_H_ */
