/*
 * CTileLoader.cpp
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#include <cstdlib>
#include <string>

#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include <base/TypeDefinitions.h>

#include "engine/core/CBehaviorEngine.h"
#include "CTileLoader.h"

#include <fileio/KeenFiles.h>

CTileLoader::CTileLoader(int episode, bool demo, int version, unsigned char *data)
{
	m_episode = episode;
	m_demo = demo;
	m_version = version;
	m_data = data;
	m_offset = 0;
	setupOffsetMap();
}

CTileLoader::CTileLoader()
{
    const auto &Exefile = gKeenFiles.exeFile;

	m_episode = Exefile.getEpisode();
	m_demo = Exefile.isDemo();
	m_version = Exefile.getEXEVersion();
	m_data = Exefile.getRawData();
	m_offset = 0;

    if(!Exefile.isPythonScript())
    {
        setupOffsetMap();
    }
}

bool CTileLoader::readOffsetMapfromFile(const std::string &tliFname,
                                        std::vector<byte> &tileData)
{
    std::ifstream tliFile(tliFname.c_str());

    if(!tliFile)
    {
        return false;
    }

    tliFile.seekg(0, tliFile.end);
    const auto fileSize = tliFile.tellg();
    tliFile.seekg(0, tliFile.beg);

    tileData.resize(size_t(fileSize));
    tliFile.read(reinterpret_cast<char*>(tileData.data()), fileSize);

    return true;
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

	// k6demo
	m_offsetMap[8][100] = 0x1f6f2;
}

size_t CTileLoader::getOffset()
{
    return m_offsetMap[m_demo ? 8 : m_episode][m_version];
}

bool CTileLoader::load(const size_t NumUnMaskedTiles,
                       const size_t NumMaskedTiles)
{
	bool success = false;
    bool isTiledataLoaded = false;

    byte *data = nullptr;

    std::vector<byte> tileData;

    if(gKeenFiles.exeFile.isPythonScript())
    {
        const auto &gameDir = gKeenFiles.gameDir;

        const std::string tliFname = "Keen" + itoa(m_episode) + ".tli";
        const auto tliPath = JoinPaths(gameDir, tliFname);
        const auto fullTliPath = GetFullFileName(tliPath);

        if(!readOffsetMapfromFile(fullTliPath, tileData))
        {
            gLogging << "Error reading " << tliPath;
            return false;
        }
        else
        {
            isTiledataLoaded = true;
            data = tileData.data();
        }
    }
    else if(getOffset())
    {
        isTiledataLoaded = true;
        data = m_data + getOffset();
    }

    if(isTiledataLoaded)
	{
		if(m_episode == 1 || m_episode == 2 || m_episode == 3 )
		{
			CTileProperties emptyTileProperties;
			
			for(int i=0 ; i<2 ; i++)
			{
			  std::vector<CTileProperties> &tileProperties = gBehaviorEngine.getTileProperties(i);			
              tileProperties.assign(NumMaskedTiles, emptyTileProperties);
              success = readVorticonTileinfo(data, tileProperties);
			}
		}
		else if(m_episode == 4 || m_episode == 5 || m_episode == 6 || m_episode == 7 )
		{
			CTileProperties TileProperties;
			gBehaviorEngine.getTileProperties(0).assign(NumUnMaskedTiles, TileProperties);
			gBehaviorEngine.getTileProperties(1).assign(NumMaskedTiles, TileProperties);
            success = readGalaxyTileinfo(data, NumUnMaskedTiles, NumMaskedTiles);
		}
	}

	return success;
}

/**
 * \brief This function assings the tileinfo data block previously read to the internal TileProperties
 * 		  structure in CG.
 */
bool CTileLoader::readVorticonTileinfo(byte *data,
                                       std::vector<CTileProperties> &TileProperties)
{
	const size_t NumTiles = TileProperties.size();
	size_t planesize = 2*NumTiles;

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
    // to the galaxy approach. For individual patches it can be adapted
	for(size_t j=0 ; j < NumTiles ;  )
	{
		size_t value = GETWORD( data+(2*j) );
        if(value == 0 || value == 1)
        {
            j++;
            continue;
		}

		// stuff for animated tiles
		for( size_t i=0 ; i<value ; i++ )
		{
            int nxTile;

			if(i == value-1)
            {
                nxTile = -(value-1);
            }
			else
            {
                nxTile = 1;
            }

            TileProperties[j+i].nextTile = nxTile;
            TileProperties[j+i+nxTile].prevTile = j+1;

            TileProperties[j+i].animationTime = 6; // Time that has to pass in game cycles until
												   // animation is performed.
		}

		j += value;
	}

	// This function assigns the correct tiles that have to be changed
	assignChangeTileAttribute(TileProperties);
	return true;
}

bool CTileLoader::readGalaxyTileinfo(byte *data,
                                     const size_t NumUnMaskedTiles,
                                     const size_t NumMaskedTiles)
{
    bool success = true;

    std::vector<CTileProperties> &TileUnmaskedProperties = gBehaviorEngine.getTileProperties(0);

    for(size_t j=0 ; j < NumUnMaskedTiles ; j++)
    {
        TileUnmaskedProperties[j].animationTime = data[j];

        auto nxTile = static_cast<Sint8>(data[NumUnMaskedTiles+j]);
        size_t nxTileIndex = nxTile + j;
        if(nxTileIndex >= NumUnMaskedTiles)
        {
            gLogging.ftextOut("bad nxTileIndex at j=%u nxTile=%d NumUnMaskedTiles=%u",
                              j, nxTile, NumUnMaskedTiles);
            success = false;
            break;
        }

        TileUnmaskedProperties[j].nextTile = nxTile;
        TileUnmaskedProperties[nxTile+j].prevTile   = j;
    }

    std::vector<CTileProperties> &TileMaskedProperties = gBehaviorEngine.getTileProperties(1);
    size_t maskedTilesBase = 2 * NumUnMaskedTiles;
    for(size_t j=0 ; j < NumMaskedTiles ; j++)
    {
        size_t offset = j + maskedTilesBase;
        TileMaskedProperties[j].bup    = data[offset];
        TileMaskedProperties[j].bright = data[offset+NumMaskedTiles];
        TileMaskedProperties[j].bdown  = data[offset+2*NumMaskedTiles];
        TileMaskedProperties[j].bleft  = data[offset+3*NumMaskedTiles];

        auto nxTile = static_cast<Sint8>(data[offset+4*NumMaskedTiles]);
        size_t nxTileIndex = nxTile + j;
        if(nxTileIndex >= NumMaskedTiles)
        {
            gLogging.ftextOut("bad nxTileIndex at j=%u nxTile=%d NumMaskedTiles=%u",
                              j, nxTile, NumUnMaskedTiles);
            success = false;
            break;
        }

        TileMaskedProperties[j].nextTile = nxTile;
        TileMaskedProperties[nxTileIndex].prevTile = j;
        TileMaskedProperties[j].behaviour     = data[offset+5*NumMaskedTiles];
        TileMaskedProperties[j].animationTime = data[offset+6*NumMaskedTiles];
    }

    return success;
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
