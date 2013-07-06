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
#include <SDL.h>
#include <iostream>
#include <fstream>

#include "CLogFile.h"
#include "StringUtils.h"
#include "fileio/TypeDefinitions.h"
#include "CSingleton.h"
#include "engine/CEvent.h"
#include "Oldsavegamestructs.h"

#include <boost/property_tree/ptree.hpp>


#define SG_HEADERSIZE			7
#define SAVEGAMEVERSION 		'6'
#define OLDSAVEGAMEVERSION5		'5'
#define OLDSAVEGAMEVERSION4		'4'


#define gpSaveGameController CSaveGameController::Get()


class CSaveGameController : public CSingleton<CSaveGameController>
{
public:

	// Initialization
	CSaveGameController();

	// Setters
	void setGameDirectory(const std::string& game_directory);
	void setEpisode(char Episode);
	void setLevel(int Level);

	// Getters
	std::vector<std::string> getSlotList();

	bool convertOldFormat(size_t slot);
	void convertAllOldFormats();
	void readOldHeader(FILE *fp, byte *episode, byte *level, byte *lives, byte *num_players);
	Uint32 getSlotNumber(const std::string &filename);
	std::string getSlotName(const std::string &filename);
	Uint32 getDataSize(std::ifstream &StateFile);
	void readData(char *buffer, Uint32 size, std::ifstream &StateFile);

	bool Fileexists( int SaveSlot );
	bool prepareSaveGame( int SaveSlot, const std::string &Name);
	bool prepareLoadGame( int SaveSlot );

    bool saveXMLNode(boost::property_tree::ptree &pt);

	// Encoder/Decoder Classes
	template <class T>
	void encodeVariable(T value);
	template <class S>
	void encodeData(S structure);
	template <class T>
	void decodeVariable(T &variable);
	template <class S>
	bool decodeData(S &structure);

	void addData(byte *data, Uint32 size);
	bool readDataBlock(byte *data);

	bool save();
	bool load();
	bool alreadyExits();
	
	bool busy()
	{ return !m_statefilename.empty(); }
	
	std::string getUnnamedSlotName();
	
private:

	bool loadSaveGameVersion5(const std::string &fname, OldSaveGameFormatV5& old);
	bool loadSaveGameVersion4(const std::string &fname, OldSaveGameFormatV4& old);

	bool IsOldSGVersion5(const std::string& fname);
	bool IsOldSGVersion4(const std::string& fname);
	int getOldSGVersion(const std::string& fname);

	std::string m_savedir;
    std::string m_statefilename;
    std::string m_stateXMLfilename;
    std::string m_statename;
	char m_Episode;
	int m_Level;
	Uint32 m_offset;	

	std::vector<byte> m_datablock;
};


struct SaveGameEvent : public CEvent
{};

struct LoadGameEvent : public CEvent
{};


#include "CSavedGameCoder.h"

#endif /* CSAVEDGAME_H_ */
