/*
 * CSaveGameControllerCoder.h
 *
 *  Created on: 24.11.2009
 *      Author: gerstrong
 *
 *  Definition of special template functions
 *
 *  They are part of the main header, because otherwise the compiler
 *  would not know how to apply the templates to those special
 *  methods
 */

#ifndef CPlayGameCoder_H_
#define CPlayGameCoder_H_

#include "fileio/TypeDefinitions.h"

// This functions are used for enconding/decoding a variable to the game data format.
// It makes everything platform independent
template <class S>
void CSaveGameController::encodeData(S structure)
{
	size_t size = sizeof(S);
	byte sizebuf[sizeof(size_t)];
	byte databuf[size];
    
	memcpy(sizebuf, &size, sizeof(size_t));
	for( Uint32 i=0 ; i<sizeof(size_t) ; i++ )
		m_datablock.push_back( sizebuf[i] );
    
	memcpy(databuf, &structure, size);
	for( Uint32 i=0 ; i<size ; i++ )
		m_datablock.push_back( databuf[i] );
}

template <class S>
void CSaveGameController::decodeData(S &structure)
{
	size_t det_size = 0;
	size_t req_size = sizeof(S);
    
	memcpy(&det_size, &m_datablock.at(m_offset), sizeof(size_t));
	m_offset += sizeof(size_t);
    
	if( det_size > req_size ) // This implementation is used for avoiding possible errors
		memcpy(&structure, &m_datablock.at(m_offset), req_size);
	else
		memcpy(&structure, &m_datablock.at(m_offset), det_size);
    
	m_offset += det_size;
}

#endif /* CSAVEDGAMECODER_H_ */
