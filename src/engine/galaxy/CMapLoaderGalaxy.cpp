/*
 * CMapLoaderGalaxy.cpp
 *
 *  Created on: 29.05.2010
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxy.h"

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
{
	// TODO Auto-generated constructor stub

}

// Gets returns the address of the datablock of the exe file, in where the
byte *CMapLoaderGalaxy::getMapheadPtr()
{
	// Get the offset of the MAPHEAD. It is told that it always begins with $ABCD as word type

	// Seek for that strange number and return the pointer to that datablock
}

bool CMapLoaderGalaxy::loadMap(Uint8 episode, Uint8 level, const std::string& path)
{
	// Get the MAPHEAD Location from within the Exe File
	byte *Maphead = getMapheadPtr();

	// Get location of the level data from MAPHEAD Located in the EXE-File.

	// Then jump to that location and read the level map data
	{
		// Get the Offsets which tell where the compressed planes start at. We have three

		// Get the lengths of those compressed planes

		// Get the dimensions of the level

		// Get internal name of the level (not used by the itself)

		// Get and check the signature

		// Then decompress the level data using rlew and carmack
	}
}

CMapLoaderGalaxy::~CMapLoaderGalaxy()
{
	// TODO Auto-generated destructor stub
}
