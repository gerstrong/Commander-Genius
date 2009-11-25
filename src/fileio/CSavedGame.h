/*
 * CSavedGame.h
 *
 *  Created on: 13.08.2009
 *      Author: gerstrong
 */

#ifndef CSAVEDGAME_H_
#define CSAVEDGAME_H_

#define uchar unsigned char

#include <string>
#include <vector>
#include <SDL.h>
#include <iostream>
#include <fstream>

#include "../CLogFile.h"
#include "../StringUtils.h"

class CSavedGame {
public:
	enum SavedGameCommands{
		NONE, SAVE, LOAD
	};

	// Initialization
	CSavedGame();

	// Setters
	void setEpisode(char Episode);

	// Getters
	std::vector<std::string> getSlotList();
	Uint32 getSlotNumber(const std::string &filename);
	std::string getSlotName(const std::string &filename);
	Uint32 getDataSize(std::ifstream &StateFile);
	void readData(char *buffer, Uint32 size, std::ifstream &StateFile);

	bool prepareSaveGame( int SaveSlot, const std::string &Name);
	bool prepareLoadGame( int SaveSlot);

	// Encoder/Decoder Classes
	template <class T>
	void encodeVariable(T value);
	template <class S>
	void encodeStruct(S structure);
	template <class T>
	void decodeVariable(T &variable);
	template <class S>
	void decodeStruct(S &structure);

	void addData(uchar *data, Uint32 size);
	void readDataBlock(uchar *data);

	bool save();
	bool load();
	
	char getCommand() { return m_Command; }

	// shutdown
	virtual ~CSavedGame();

private:
	std::string m_statefilename;
	std::string m_statename;
	char m_Episode;
	char m_Command;
	Uint32 m_offset;

	std::vector<char> m_datablock;
};

#include "CSavedGameCoder.h"

#endif /* CSAVEDGAME_H_ */
