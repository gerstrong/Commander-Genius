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

#define SG_HEADERSIZE			7
#define SAVEGAMEVERSION 		'6'
#define OLDSAVEGAMEVERSION5		'5'
#define OLDSAVEGAMEVERSION4		'4'

// Savegame/Loadgame Menu common stuff
const int MENU_WIDTH = 40;
const int TEXT_WIDTH = 32;



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
	bool prepareLoadGame( int SaveSlot);

	// Encoder/Decoder Classes
	template <class T>
	void encodeVariable(T value);
	template <class S>
	void encodeData(S structure);
	template <class T>
	void decodeVariable(T &variable);
	template <class S>
	void decodeData(S &structure);

	void addData(byte *data, Uint32 size);
	void readDataBlock(byte *data);

	bool save();
	bool load();
	bool alreadyExits();
	
	std::string getEmptyString();
	std::string getUnnamedSlotName();

private:

	bool loadSaveGameVersion5(const std::string &fname, OldSaveGameFormatV5& old);
	bool loadSaveGameVersion4(const std::string &fname, OldSaveGameFormatV4& old);

	bool IsOldSGVersion5(const std::string& fname);
	bool IsOldSGVersion4(const std::string& fname);
	int getOldSGVersion(const std::string& fname);

	std::string m_savedir;
	std::string m_statefilename;
	std::string m_statename;
	char m_Episode;
	int m_Level;
	Uint32 m_offset;
	std::string m_emptyString;

	std::vector<byte> m_datablock;
};


struct SaveGameFunctorEvent : public InvokeFunctorEvent
{
	void operator()()
	{
		gpSaveGameController->save();
	}
};

struct LoadGameFunctorEvent : public InvokeFunctorEvent
{
	void operator()()
	{
		gpSaveGameController->load();
	}
};


#include "CSavedGameCoder.h"

#endif /* CSAVEDGAME_H_ */
