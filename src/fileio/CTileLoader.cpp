/*
 * CTileLoader.cpp
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#include "CTileLoader.h"
#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include <base/TypeDefinitions.h>
#include "engine/core/CBehaviorEngine.h"
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
	
	if(m_offsetMap[m_episode][m_version])
	{
		if(m_episode == 1 || m_episode == 2 || m_episode == 3 )
		{
			CTileProperties emptyTileProperties;
			
			for(int i=0 ; i<2 ; i++)
			{
			  std::vector<CTileProperties> &tileProperties = g_pBehaviorEngine->getTileProperties(i);			
			  tileProperties.assign(NumMaskedTiles, emptyTileProperties);						  
			  readVorticonTileinfo(tileProperties);
			}
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
void CTileLoader::readVorticonTileinfo(std::vector<CTileProperties> &TileProperties)
{
	const size_t NumTiles = TileProperties.size();
	size_t planesize = 2*NumTiles;

	byte *data = m_data + m_offsetMap[m_episode][m_version];

	// Special workaround. I don't know why this happens, but Episode 3 doesn not seems to read
	// the plane size. TODO: Check where that value is hidden in the EXE file.
	if(m_episode == 3) planesize = 2*715;

	for(size_t j=0 ; j < NumTiles ; j++)
	{
		TileProperties[j].behaviour 	= GETWORD( data+planesize+2*j);
		TileProperties[j].bup 			= GETWORD( data+2*(planesize)+2*j );
		TileProperties[j].bright 		= GETWORD( data+3*(planesize)+2*j);
		TileProperties[j].bdown 		= GETWORD( data+4*(planesize)+2*j);
		TileProperties[j].bleft 		= GETWORD( data+5*(planesize)+2*j);

		if( TileProperties[j].bleft && TileProperties[j].bright &&
				TileProperties[j].bup && TileProperties[j].bdown	)
		{ // This should solve some tile bugs in Episode 2
			if(TileProperties[j].behaviour == -2 or  TileProperties[j].behaviour == -1)
				TileProperties[j].behaviour = 0;
		}

		// Now set the ice and slippery tiles in case we have them
		if( TileProperties[j].bup > 1)
		{
			TileProperties[j].slippery = TileProperties[j].bup;
			TileProperties[j].bup = 1;
		}
	}

	// This is a special case, because vorticon engine handles animation different
	// to our new structure. For individual patches it can be adapted
	for(size_t j=0 ; j < NumTiles ;  )
	{
		size_t value = GETWORD( data+(2*j) );
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
	assignChangeTileAttribute(TileProperties);
}

void CTileLoader::readGalaxyTileinfo(size_t NumUnMaskedTiles, size_t NumMaskedTiles)
{
	byte *data = m_data + m_offsetMap[m_episode][m_version];
	std::vector<CTileProperties> &TileUnmaskedProperties = g_pBehaviorEngine->getTileProperties(0);
	for(size_t j=0 ; j < NumUnMaskedTiles ; j++)
	{
		TileUnmaskedProperties[j].animationtime = data[j];
		TileUnmaskedProperties[j].nextTile = static_cast<Sint8>(data[NumUnMaskedTiles+j]);
	}

	std::vector<CTileProperties> &TileMaskedProperties = g_pBehaviorEngine->getTileProperties(1);
	for(size_t j=0 ; j < NumMaskedTiles ; j++)
	{
		TileMaskedProperties[j].bup 			= data[j+2*NumUnMaskedTiles];
		TileMaskedProperties[j].bright 			= data[j+2*NumUnMaskedTiles+NumMaskedTiles];
		TileMaskedProperties[j].bdown 			= data[j+2*NumUnMaskedTiles+2*NumMaskedTiles];
		TileMaskedProperties[j].bleft 			= data[j+2*NumUnMaskedTiles+3*NumMaskedTiles];

		TileMaskedProperties[j].nextTile 		= static_cast<Sint8>(data[j+2*NumUnMaskedTiles+4*NumMaskedTiles]);
		TileMaskedProperties[j].behaviour 		= data[j+2*NumUnMaskedTiles+5*NumMaskedTiles];
		TileMaskedProperties[j].animationtime 	= data[j+2*NumUnMaskedTiles+6*NumMaskedTiles];		
	}
}

void CTileLoader::assignChangeTileAttribute(std::vector<CTileProperties> &TilePropertiesVec)
{
	// smart tile change is the change style that uses Keen 3. This is vorticon only stuff!
	bool smart_tilechanger = false;

	if(m_episode == 3 || smartTileChangerEnabled())
		smart_tilechanger = true;
	
	// At any other case, except some special ones.
	// This is the case
	size_t i = 0;
	for( auto &TileProperties : TilePropertiesVec )
	{
		if( canbePickedup(TileProperties.behaviour) )
		{
			if(smart_tilechanger)
				TileProperties.chgtile = (i/13)*13;
			else
				TileProperties.chgtile = (i>=0x131) ? 0x114 : 0x8F;
		}
		if( isaDoor(TileProperties.behaviour) )
		{
			if(m_episode == 3)
				TileProperties.chgtile = 0xB6;
			else if(smart_tilechanger)
				TileProperties.chgtile = (i/13)*13;
			else
				TileProperties.chgtile = 0x8F;
		}
		
		i++;
	}
}

bool CTileLoader::canbePickedup(const signed char behaviour)
{
	return ((behaviour >= 6 && 
		 behaviour <= 21 &&
		 behaviour != 17) ||
		 behaviour == 27 ||
		 behaviour == 28);
}

bool CTileLoader::isaDoor(const signed char behaviour)
{
	return (behaviour >= 2 && behaviour <= 5);
}

/**
 * \brief This will read the code for changed tiles in the exe files. Some
 * mods use that in Episode 1 for the smart tile change. Also see
 * assignChangeTileAttribute()
 * %patch $4409      $26 $8B $07 $B6 $0D $F6 $FE
 * %patch $4410  $F6 $EE $26 $89 $07 $E9 $60 $01
 */
bool CTileLoader::smartTileChangerEnabled()
{
	byte *pointertocode = m_data+0x4409;

	byte comparecode[] =
	{
			0x26, 0x8B, 0x07, 0xB6, 0x0D, 0xF6, 0xFE,
			0xF6, 0xEE, 0x26, 0x89, 0x07, 0xE9, 0x60, 0x01
	};

	return (memcmp(comparecode, pointertocode, sizeof(comparecode))==0);
}
