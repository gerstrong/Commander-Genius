/*
 * CSavedGameCoder.h
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

// This function is used for enconding a variable to the game data format.
// It makes everything platform independent
// Special case, because the usage must be known for the compiler in order to get it build correctly!
template <class T>
void CSavedGame::encodeVariable(T value){
	uchar size = sizeof(T);
	Uint32 data;
	m_datablock.push_back(size);
	for( Uint32 i=size-1 ; i>=0 ; i-- )
	{
		data = value&(0xFF<<(i*8));
		data >>= (i*8);
		m_datablock.push_back( static_cast<uchar>(data) );
	}
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

template <class T>
void CSavedGame::decodeVariable(T &variable)
{
	variable = 0;
	uchar datasize=0;
	uchar reqsize = sizeof(T);

	datasize = m_datablock[m_offset++];
	for( Uint32 i=datasize ; i>=0 ; i-- )
		variable += m_datablock[m_offset++] <<(i*8);

	if(datasize != reqsize) // Check if we get the size we request
	{
		std::string errortext = "Warning decoding the data of the file!\n";
		errortext += "Data size is incorrect! Still trying to get the right data!";
		errortext += " req: " + itoa(reqsize);
		errortext += " ; obt: " + itoa(datasize);
		g_pLogFile->textOut(errortext);
	}
}

template <class S>
void CSavedGame::decodeStruct(S &structure)
{

}

#endif /* CSAVEDGAMECODER_H_ */
