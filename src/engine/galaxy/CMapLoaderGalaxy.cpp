/*
 * CMapLoaderGalaxy.cpp
 *
 *  Created on: 29.05.2010
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxy.h"
#include "../../StringUtils.h"
#include "../../FindFile.h"
#include "../../fileio/ResourceMgmt.h"
#include <fstream>

/*
 *
0   4    Offset in GAMEMAPS to beginning of compressed plane 0 data
4   4    Offset in GAMEMAPS to beginning of compressed plane 1 data
8   4    Offset in GAMEMAPS to beginning of compressed plane 2 data
12  2    Length of compressed plane 0 data (in bytes)
14  2    Length of compressed plane 1 data (in bytes)
16  2    Length of compressed plane 2 data (in bytes)
18  2    Width of level (in tiles)
20  2    Height of level (in tiles)
22  16   Internal name for level (used only by editor, not displayed in-game. null-terminated)
38  4    Signature "!ID!"
42  x    Level data, three planes RLEW and Carmack compressed
 *
 * */

CMapLoaderGalaxy::CMapLoaderGalaxy(CExeFile &ExeFile):
m_ExeFile(ExeFile)
{}

/* \brief	This function searches for a word type value in a given buffer
 * \param	data	pointer to the data buffer
 * \param	ident	The value that has to be searched for
 * \param	repeat	an optional parameter which identicates how many times
 * 					the iden has to repeat until we get the right offset
 * \return 	the position of the offset or zero if nothing was found. The offset must
 * 			always be different than zero!!
*/
size_t findWordPos(byte* data, word ident, size_t buffer_size, size_t repeat)
{
	size_t offset = 0;
	byte* startdata = data;

	byte low_byte = ident | 0x00FF;
	byte high_byte = ident >> 8;

	while( static_cast<size_t>(data - startdata) < buffer_size )
	{
		if(*data == low_byte)
		{
			if(*(data+1) == high_byte)
			{
				offset = data - startdata;
				repeat--;
				if(repeat == 0)
					break;
			}
		}
		data++;
	}

	return offset;
}

// Gets returns the address of the datablock of the exe file, in where the
byte *CMapLoaderGalaxy::getMapheadPtr()
{
	// Get the offset of the MAPHEAD. It is told that it always begins with $ABCD as word type
	byte* data = m_ExeFile.getRawData();
	return (data + findWordPos(data, 0xABCD, m_ExeFile.getExeDataSize(), 2));
}

bool CMapLoaderGalaxy::loadMap(CMap &Map, Uint8 level)
{
	// Get the MAPHEAD Location from within the Exe File
	byte *Maphead = getMapheadPtr();
	word magic_word;
	longword level_offset;
	std::string path;

	// Get the magic number of the level data from MAPHEAD Located in the EXE-File.
	// This is used for the decompression.
	magic_word = READWORD(Maphead);

	// Get location of the level data from MAPHEAD Located in the EXE-File.
	Maphead += (level-1)*sizeof(longword);
	level_offset = READLONGWORD(Maphead);

	// Open the Gamemaps file
	path = m_ExeFile.getDataDirectory();
	std::string gamemapfile = "gamemaps.ck"+itoa(m_ExeFile.getEpisode());

	std::ifstream MapFile;
	if(OpenGameFileR(MapFile, getResourceFilename("gamemaps.ck",path,true,false), std::ios::binary))
	{
		// Then jump to that location and read the level map data

		// Get the Offsets which tell where the compressed planes start at. We have three

		// Get the lengths of those compressed planes

		// Get the dimensions of the level

		// Get internal name of the level (not used by the itself)

		// Get and check the signature

		// Then decompress the level data using rlew and carmack
		return true;
	}
	else
		return false;
}

CMapLoaderGalaxy::~CMapLoaderGalaxy()
{
	// TODO Auto-generated destructor stub
}
