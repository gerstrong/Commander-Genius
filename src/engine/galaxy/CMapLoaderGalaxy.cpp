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
16  2    Length of compressed plane 2 data (ib bytes)
18  2    Width of level (in tiles)
20  2    Height of level (in tiles)
22  16   Internal name for level (used only by editor, not displayed in-game. null-terminated)
38  4    Signature "!ID!"
42  x    Level data, three planes RLEW and Carmack compressed
 *
 * */

CMapLoaderGalaxy::CMapLoaderGalaxy(CExeFile &ExeFile)
{
	// TODO Auto-generated constructor stub

}

bool CMapLoaderGalaxy::loadMap()
{

}

CMapLoaderGalaxy::~CMapLoaderGalaxy()
{
	// TODO Auto-generated destructor stub
}
