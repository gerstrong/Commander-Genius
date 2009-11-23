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

	void addData(uchar *data, Uint32 size);

	bool save();
	bool load() { return false; }
	
	char getCommand() { return m_Command; }

	// shutdown
	virtual ~CSavedGame();

private:
	std::string m_statefilename;
	std::string m_statename;
	char m_Episode;
	char m_Command;

	std::vector<char> m_datablock;
};

////
// Definition of special template functions
////

// This function is used for enconding a variable to the game data format.
// It makes everything platform independent
// Special case, because the usage must be known for the compiler in order to get it build correctly!
template <class T>
void CSavedGame::encodeVariable(T value){
	Uint32 size = sizeof(T);
	for( Uint32 i=0 ; i<sizeof(Uint32) ; i++ )
		m_datablock.push_back(size>>(i*8));
	for( Uint32 i=0 ; i<size ; i++ )
		m_datablock.push_back( static_cast<uchar>(value>>(i*8)) );
}

// Same case as above but for structures
template <class S>
void CSavedGame::encodeStruct(S structure){
	size_t size = sizeof(S);
	uchar buf[size];

	for( Uint32 i=0 ; i<sizeof(Uint32) ; i++ )
		m_datablock.push_back(size>>(i*8));

	memcpy(buf, &structure, size);

	for( Uint32 i=0 ; i<size ; i++ )
		m_datablock.push_back( buf[i] );
}


#endif /* CSAVEDGAME_H_ */
