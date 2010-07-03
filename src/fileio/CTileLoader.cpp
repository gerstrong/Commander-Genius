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
#include "TypeDefinitions.h"
#include <cstdlib>
#include <string>

CTileLoader::CTileLoader(int episode, int version, unsigned char *data)
{
	m_episode = episode;
	m_version = version;
	m_data = data;
	m_offset = 0;
	setupOffsetMap();
}

CTileLoader::CTileLoader(CExeFile &Exefile)
{
	m_episode = Exefile.getEpisode();
	m_version = Exefile.getEXEVersion();
	m_data = Exefile.getRawData();
	m_offset = 0;
	setupOffsetMap();
}

void CTileLoader::setupOffsetMap()
{
	m_offsetMap[1][110] = 0x131F8;
	m_offsetMap[1][131] = 0x130F8;
	m_offsetMap[1][134] = 0x130F8;

	m_offsetMap[2][100] = 0x17938;
	m_offsetMap[2][131] = 0x17828;

	m_offsetMap[3][100] = 0x199F8;
	m_offsetMap[3][131] = 0x198C8;

	m_offsetMap[4][140] = 0x249C2;

	m_offsetMap[5][140] = 0x25B22;

	m_offsetMap[6][140] = 0x25212;

	m_offsetMap[7][100] = 0x11322;
}

bool CTileLoader::load(size_t NumUnMaskedTiles, size_t NumMaskedTiles)
{
	std::string fname;
	
	long offset = m_offsetMap[m_episode][m_version];

	m_data += offset;
	
	if(m_offsetMap[m_episode][m_version])
	{
		if(m_episode == 1 || m_episode == 2 || m_episode == 3 )
		{
			CTileProperties TileProperties;
			g_pBehaviorEngine->getTileProperties(1).assign(NumMaskedTiles, TileProperties);
			readVorticonTileinfo(NumMaskedTiles);
		}
		if(m_episode == 4 || m_episode == 5 || m_episode == 6 || m_episode == 7 )
		{
			CTileProperties TileProperties;
			g_pBehaviorEngine->getTileProperties(0).assign(NumUnMaskedTiles, TileProperties);
			g_pBehaviorEngine->getTileProperties(1).assign(NumMaskedTiles, TileProperties);
			readGalaxyTileinfo(NumUnMaskedTiles, NumMaskedTiles);
		}
	}

	return true;
}

/**
 * \brief This function assings the tileinfo data block previously read to the internal TileProperties
 * 		  structure in CG.
 */
void CTileLoader::readVorticonTileinfo(size_t NumTiles)
{
	size_t planesize = 2*NumTiles;

	// Special workaround. I don't know why this happens, but Episode 3 doesn not seems to read
	// the plane size. TODO: Check where that value is hidden in the EXE file.
	if(m_episode == 3) planesize = 2*715;

	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties(1);
	for(size_t j=0 ; j < NumTiles ; j++)
	{
		TileProperties[j].behaviour 	= GETWORD( m_data+planesize+2*j);
		TileProperties[j].bup 			= GETWORD( m_data+2*(planesize)+2*j );
		TileProperties[j].bright 		= GETWORD( m_data+3*(planesize)+2*j);
		TileProperties[j].bdown 		= GETWORD( m_data+4*(planesize)+2*j);
		TileProperties[j].bleft 		= GETWORD( m_data+5*(planesize)+2*j);

		if( TileProperties[j].bleft && TileProperties[j].bright &&
				TileProperties[j].bup && TileProperties[j].bdown	)
		{ // This should solve some tile bugs in Episode 2
			if(TileProperties[j].behaviour == -2 or  TileProperties[j].behaviour == -1)
				TileProperties[j].behaviour = 0;
		}
	}

	// This is a special case, because vorticon engine handles animation different
	// to our new structure. For individual patches it can be adapted
	for(size_t j=0 ; j < NumTiles ;  )
	{
		size_t value = GETWORD( m_data+(2*j) );
		if(value == 0 || value == 1) {
			j++; continue;
		}

		// stuff for animated tiles
		for( size_t i=0 ; i<value ; i++ )
		{
			if(i == value-1)
				TileProperties[j+i].nextTile = -(value-1);
			else
				TileProperties[j+i].nextTile = 1;
			TileProperties[j+i].animationtime = 6; // Time that has to pass in game cycles until
												   // animation is performed.
		}
		j += value;
	}

	// This function assigns the correct tiles that have to be changed
	assignChangeTileAttribute(NumTiles);
}

void CTileLoader::readGalaxyTileinfo(size_t NumUnMaskedTiles, size_t NumMaskedTiles)
{
	std::vector<CTileProperties> &TileUnmaskedProperties = g_pBehaviorEngine->getTileProperties(0);
	std::vector<CTileProperties> &TileMaskedProperties = g_pBehaviorEngine->getTileProperties(1);
	for(size_t j=0 ; j < NumUnMaskedTiles ; j++)
	{
		TileUnmaskedProperties[j].animationtime = m_data[j];
		TileUnmaskedProperties[j].nextTile = static_cast<char>(m_data[NumUnMaskedTiles+j]);

		/*TileProperties[j].bup 	= GETWORD( m_data+2*(NumTiles)+j );
		TileProperties[j].bright 	= GETWORD( m_data+3*(NumTiles)+j);
		TileProperties[j].bdown 	= GETWORD( m_data+4*(NumTiles)+j);
		TileProperties[j].bleft 	= GETWORD( m_data+5*(NumTiles)+j);

		if( TileProperties[j].bleft && TileProperties[j].bright &&
				TileProperties[j].bup && TileProperties[j].bdown	)
		{ // This should solve some tile bugs in Episode 2
			if(TileProperties[j].behaviour == -2 or  TileProperties[j].behaviour == -1)
				TileProperties[j].behaviour = 0;
		}*/
	}

	/*for(size_t j=0 ; j < NumTiles ;  )
	{
		size_t value = TileProperties[j].animation;
		if(value == 0) {
			j++; continue;
		}

		// stuff for animated tiles
		for( size_t i=0 ; i<value ; i++ )
			TileProperties[j+i].animOffset = i;
		j += value;
	}

	// This function assigns the correct tiles that have to be changed
	assignChangeTileAttribute();*/
}

void CTileLoader::assignChangeTileAttribute(size_t NumTiles)
{
	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties(1);
	// This special call is used for workarounds which are wrong in the tiles attributes file of CG.
	// I hope those attributes can be read out of the exe-files in future.
	// Everything to zero in order to avoid bugs in mods
	// It also assigns the special background tile which is sometimes used in the game for changes
	// to real backgrounds
	for(size_t i=0 ; i<NumTiles ; i++)
		TileProperties[i].chgtile = 0;
	
	// At any other case, except some special ones, the tile is always 143 for pickuppable items
	// 17 is tile for an exit. Until row 19, this seems to be valid
	for(size_t i=0 ; i<NumTiles ; i++)
		if(canbePickedup(i) || isaDoor(i) )
			TileProperties[i].chgtile = 143;
	
	switch(m_episode)
	{
		case 1:
		case 2:
		{
			// TODO: Check out how to perform that chgtile algorithm
			for(int i=38*13 ; i<39*13 ; i++) // Workaround for silcar 1. Row 38
				if( canbePickedup(i) )
					TileProperties[i].chgtile = 439;
			
			for(int i=35*13 ; i<36*13 ; i++) // Workaround for silcar 4. Row 35
				if( canbePickedup(i) )
					TileProperties[i].chgtile = 335;
			
			for(int i=23*13 ; i<24*13 ; i++) // Workaround in Level 12 of Episode 2, where the tiles are solid after a taken item.
				TileProperties[i].chgtile = 276;   // Row 23
			
			break;
		}
		case 3:
		{
			// Episode 3 is a special case, because the items are repeated 6 times
			for(size_t i=0 ; i<NumTiles ; i++)
			{
				// Only items!!
				if(canbePickedup(i))
					TileProperties[i].chgtile = (i/13)*13;
				
				// Only for Doors! Tile is always 182
				if(isaDoor(i))
					TileProperties[i].chgtile = 182;
			}
			
			break;
		}
	}
}

bool CTileLoader::canbePickedup(int tile)
{
	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties(1);
	return ((TileProperties[tile].behaviour >= 6 &&
			 TileProperties[tile].behaviour <= 21 &&
			 TileProperties[tile].behaviour != 17) ||
			TileProperties[tile].behaviour == 27 ||
			TileProperties[tile].behaviour == 28);
}

bool CTileLoader::isaDoor(int tile)
{
	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties(1);
	return (TileProperties[tile].behaviour >= 2 && TileProperties[tile].behaviour <= 5);
}

CTileLoader::~CTileLoader() {
	
}
