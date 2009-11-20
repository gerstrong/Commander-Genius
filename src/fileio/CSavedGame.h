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

class CSavedGame {
public:
	CSavedGame(const std::string &SaveDirectory);

	// Encoder/Decoder Classes
	template <class T>
	void encodeVariable(T value);
	template <class S>
	void encodeStruct(S structure);

	void addData(uchar *data, Uint32 size);

	char save(int slot, char name);
	char IsValidSaveGame(std::string fname);
	bool load(int slot);
	
	virtual ~CSavedGame();

private:
	void readHeader(FILE *fp, uchar *episode, uchar *level, uchar *num_players, uchar *primary_player, uchar *name);
	std::string	m_DataDirectory;
	std::vector<uchar> m_datablock;
};

////
// Definition of special template classes
////

// This function is used for enconding a variable to the game data format.
// It makes everything platform independent
// Special case, because the usage must be known for the compiler in order to get it build correctly!
template <class T>
void CSavedGame::encodeVariable(T value){
	for( Uint32 i=0 ; i<sizeof(T) ; i++ )
		m_datablock.push_back( static_cast<uchar>(value>>(i*8)) );
}

// Same case as above but for structures
template <class S>
void CSavedGame::encodeStruct(S structure){
	size_t size = sizeof(S);
	uchar buf[size];

	memcpy(buf, &structure, size);

	for( Uint32 i=0 ; i<size ; i++ )
		m_datablock.push_back( buf[i] );
}


#endif /* CSAVEDGAME_H_ */
