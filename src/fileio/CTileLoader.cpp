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

CTileLoader::CTileLoader(int episode, int version, unsigned char *data) {
	m_episode = episode;
	m_version = version;
	m_data = data;
	m_offset = 0;
	mp_TileProperty = NULL;
	m_numtiles = 0;
}

bool CTileLoader::setProperOffset()
{
	// Identify the offset
	switch (m_episode)
	{
		case 1:
		{
			m_numtiles = 611;
			switch(m_version)
			{
				case 110: m_offset = 0x131F8; break;
				case 131: m_offset = 0x130F8; break;
				case 134: m_offset = 0x130F8; // This is incorrect! Sure?
						  g_pLogFile->textOut(PURPLE,"If you want to use Episode 1 Version 1.34, assure that is was unpacked before (with unpklite for example).<br>");
						  break;
			}
			break;
		}
		case 2:
		{
			m_numtiles = 689;
			switch(m_version)
			{
				case 100: m_offset = 0x17938; break;
				case 131: m_offset = 0x17828; break;
			}
			break;
		}
		case 3:
		{
			m_numtiles = 715;
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
	int j; // standard counters

	if(!setProperOffset()) return false;

	mp_TileProperty = new stTile[m_numtiles];

	if(!mp_TileProperty)
	{
	     g_pLogFile->textOut(RED,"TileLoader: Memory couldn't be allocated for this version of game!<br>");
	     return false;
	}

	for(j=0 ; j < m_numtiles ; j++)
	{
		mp_TileProperty[j].masktile = 0;
		mp_TileProperty[j].animation = m_data[2*j];
		mp_TileProperty[j].behaviour = m_data[2*(m_numtiles)+2*j];
		mp_TileProperty[j].behaviour += m_data[2*(m_numtiles)+2*j+1] << 8;
		mp_TileProperty[j].bup = m_data[4*(m_numtiles)+2*j];
		mp_TileProperty[j].bup += m_data[4*(m_numtiles)+2*j+1] << 8;
		mp_TileProperty[j].bright = m_data[6*(m_numtiles)+2*j];
		mp_TileProperty[j].bright += m_data[6*(m_numtiles)+2*j+1] << 8;
		mp_TileProperty[j].bdown = m_data[8*(m_numtiles)+2*j];
		mp_TileProperty[j].bdown += m_data[8*(m_numtiles)+2*j+1] << 8;
		mp_TileProperty[j].bleft = m_data[10*(m_numtiles)+2*j];
		mp_TileProperty[j].bleft += m_data[10*(m_numtiles)+2*j+1] << 8;
	}

	int value;
	for( j=0 ; j < m_numtiles ; )
	{
		value = mp_TileProperty[j].animation;

		// stuff for animated tiles
		if(value == 1)
		{
			mp_TileProperty[j++].animOffset = 0;   // starting offset from the base frame
		}
		else if( value == 2 )
		{
			mp_TileProperty[j++].animOffset = 0;   // starting offset from the base frame
			mp_TileProperty[j++].animOffset = 1;   // starting offset from the base frame
		}
		else
		{
			mp_TileProperty[j++].animOffset = 0;   // starting offset from the base frame
			mp_TileProperty[j++].animOffset = 1;   // starting offset from the base frame
			mp_TileProperty[j++].animOffset = 2;   // starting offset from the base frame
			mp_TileProperty[j++].animOffset = 3;   // starting offset from the base frame
		}
	}

	// This function assigns the correct tiles that have to be changed
	assignChangeTileAttribute();

	return true;
}

void CTileLoader::assignChangeTileAttribute()
{
	// This special call is used for workarounds which are wrong in the tiles attributes file of CG.
	// I hope those attributes can be read out of the exe-files in future.
	// Everything to zero in order to avoid bugs in mods
	for(int i=0 ; i<m_numtiles ; i++)
		mp_TileProperty[i].chgtile = 0;

	// At any other case, than the special ones, the tile is always 143 for pickuppable items
	// 17 is tile for an exit. Until row 19, this seems to be valid
	for(int i=0 ; i<m_numtiles ; i++)
		if(canbePickedup(i) || isaDoor(i) )
			mp_TileProperty[i].chgtile = 143;

	switch(m_episode)
	{
		case 1:
		case 2:
		{
			// TODO: Check out how to perform that chgtile algorithm
			for(int i=38*13 ; i<39*13 ; i++) // Workaround for silcar 1. Row 38
				if( canbePickedup(i) )
					mp_TileProperty[i].chgtile = 439;

			for(int i=35*13 ; i<36*13 ; i++) // Workaround for silcar 4. Row 35
				if( canbePickedup(i) )
					mp_TileProperty[i].chgtile = 335;

			for(int i=23*13 ; i<24*13 ; i++) // Workaround in Level 12 of Episode 2, where the tiles are solid after a taken item.
					mp_TileProperty[i].chgtile = 276;   // Row 23

			break;
		}
		case 3:
		{
			// Episode 3 is a special case, because the items are repeated 6 times
			for(int i=0 ; i<=m_numtiles ; i++)
			{
				// Only items!!
				if(canbePickedup(i))
					mp_TileProperty[i].chgtile = (i/13)*13;

				// Only for Doors! Tile is always 182
				if(isaDoor(i))
					mp_TileProperty[i].chgtile = 182;
			}

			break;
		}
	}
}

bool CTileLoader::canbePickedup(int tile)
{
	return ((mp_TileProperty[tile].behaviour >= 6 &&
			mp_TileProperty[tile].behaviour <= 21 &&
			mp_TileProperty[tile].behaviour != 17) ||
			mp_TileProperty[tile].behaviour == 27 ||
			mp_TileProperty[tile].behaviour == 28);
}

bool CTileLoader::isaDoor(int tile)
{
	return (mp_TileProperty[tile].behaviour >= 2 && mp_TileProperty[tile].behaviour <= 5);
}

CTileLoader::~CTileLoader() {

}
