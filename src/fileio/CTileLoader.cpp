/*
 * CTileLoader.cpp
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#include "CTileLoader.h"
#include "../CLogFile.h"
#include "../StringUtils.h"
#include "../FindFile.h"
#include <cstdlib>
#include <string>

extern stTile tiles[MAX_TILES+1];

CTileLoader::CTileLoader(int episode, int version, unsigned char *data) {
	TileProperty = NULL;
	m_episode = episode;
	m_version = version;
	m_data = data;
	m_offset = 0;
}

CTileLoader::~CTileLoader() {
	// Here we really start reading the tiles
	if(TileProperty != NULL)
	{
		for(int i = 0 ; i < MAX_TILES ; i++)
		{
			if(TileProperty[i] != NULL)
			{
				delete [] TileProperty[i];
				TileProperty[i] = NULL;
			}
		}
		delete [] TileProperty;
		TileProperty = NULL;
	}
}

bool CTileLoader::setProperOffset()
{
	// Identify the offset
	switch (m_episode)
	{
		case 1:
		{
			numtiles = 611;
			switch(m_version)
			{
				case 110: m_offset = 0x131F8; break;
				case 131: m_offset = 0x130F8; break;
				case 134: m_offset = 0x130F8; // This is incorrect!
						  g_pLogFile->textOut(PURPLE,"If you want to use Episode 1 Version 1.34, assure that is was unpacked before (with unpklite for example).<br>");
						  break;
			}
			break;
		}
		case 2:
		{
			numtiles = 689;
			switch(m_version)
			{
				case 100: m_offset = 0x17938; break;
				case 131: m_offset = 0x17828; break;
			}
			break;
		}
		case 3:
		{
			numtiles = 715;
			switch(m_version)
			{
				case 100: m_offset = 0x199F8; break;
				case 131: m_offset = 0x198C8; break;
			}
			break;
		}
		default:
		{
			g_pLogFile->textOut(PURPLE,"CAUTION: The version was not detected correctly. The game muy be unplayable!<br>");
			return false;
		}
	}
	m_data += m_offset;

	return true;
}

bool CTileLoader::load()
{
	std::string fname;
	int i,j; // standard counters

	if(!setProperOffset()) return false;

	TileProperty = new int*[MAX_TILES];

	for(i = 0 ; i < MAX_TILES ; i++)
	{
		TileProperty[i] = NULL;
		TileProperty[i] = new int[6];
	}

	for(j=0 ; j < MAX_TILES ; j++ )
	{
		for(i=0; i < 6 ; i++)
			TileProperty[j][i]=0;
	}

	if(TileProperty == NULL)
	{
	     g_pLogFile->textOut(RED,"TileLoader: The memory couldn't be allocated for this version of game!<br>");
	     return false;
	}

	for(i=0 ; i < 6 ; i++)
	{
		for(j=0 ; j < numtiles ; j++)
		{
			TileProperty[j][i] = m_data[i*2*(numtiles)+2*j];
			TileProperty[j][i] += m_data[i*2*(numtiles)+2*j+1]<<8;
		}
	}

	int value;
	for( j=0 ; j < numtiles ;)
	{
		value = TileProperty[j][0];

		// stuff for animated tiles
		if(value == 1)
		{
		    tiles[j++].animOffset = 0;   // starting offset from the base frame
		}
		else if( value == 2 )
		{
			 tiles[j++].animOffset = 0;   // starting offset from the base frame
			 tiles[j++].animOffset = 1;   // starting offset from the base frame
		}
		else
		{
			 tiles[j++].animOffset = 0;   // starting offset from the base frame
			 tiles[j++].animOffset = 1;   // starting offset from the base frame
			 tiles[j++].animOffset = 2;   // starting offset from the base frame
			 tiles[j++].animOffset = 3;   // starting offset from the base frame
		}
	}

	// This function assigns the correct tiles that have to be changed
    assignChangeTileAttribute(tiles);

	return true;
}

void CTileLoader::assignChangeTileAttribute(stTile *tile)
{
	// This special call is used for workarounds which are wrong in the tiles attributes file of CG.
	// I hope those attributes can be read out of the exe-files in future.
	// Everything to zero in order to avoid bugs in mods
	for(int i=0 ; i<numtiles ; i++)
		tile[i].chgtile = 0;

	// At any other case, than the special ones, the tile is always 143 for pickuppable items
	// 17 is tile for an exit. Until row 19, this seems to be valid
	for(int i=0 ; i<numtiles ; i++)
		if(canbePickedup(i) || isaDoor(i) )
			tile[i].chgtile = 143;

	switch(m_episode)
	{
		case 1:
		case 2:
		{
			for(int i=38*13 ; i<39*13 ; i++) // Workaround for silcar 1. Row 38
				if( canbePickedup(i) )
					tile[i].chgtile = 439;

			for(int i=35*13 ; i<36*13 ; i++) // Workaround for silcar 4. Row 35
				if( canbePickedup(i) )
					tile[i].chgtile = 335;

			for(int i=23*13 ; i<24*13 ; i++) // Workaround in Level 12 of Episode 2, where the tiles are solid after a taken item.
					tile[i].chgtile = 276;   // Row 23

			break;
		}
		case 3:
		{
			// Episode 3 is a special case, because the items are repeated 6 times
			for(int i=0 ; i<=numtiles ; i++)
			{
				// Only items!!
				if(canbePickedup(i))
					tile[i].chgtile = (i/13)*13;

				// Only for Doors! Tile is always 182
				if(isaDoor(i))
					tile[i].chgtile = 182;
			}

			break;
		}
	}
}

bool CTileLoader::canbePickedup(int tile)
{
	return ((TileProperty[tile][BEHAVIOR] >= 6 &&
			TileProperty[tile][BEHAVIOR] <= 21 &&
			TileProperty[tile][BEHAVIOR] != 17) ||
			TileProperty[tile][BEHAVIOR] == 27 ||
			TileProperty[tile][BEHAVIOR] == 28);
}

bool CTileLoader::isaDoor(int tile)
{
	return (TileProperty[tile][BEHAVIOR] >= 2 && TileProperty[tile][BEHAVIOR] <= 5);
}
