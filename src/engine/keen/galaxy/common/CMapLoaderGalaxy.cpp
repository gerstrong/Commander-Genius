
/*
 * CVorticonMapLoaderGalaxy.cpp
 *
 *  Created on: 29.05.2010
 *      Author: gerstrong
 *
 *	This class is responsible for loading the Sprites and Tiles of a memory into the memory.
 *	TODO: This must be split up for Episodes 4, 5, 6 and Dreams.
 */

#include "CMapLoaderGalaxy.h"
#include "engine/core/CCamera.h"
#include "fileio/KeenFiles.h"
#include <engine/core/CBehaviorEngine.h>
#include <fileio/ResourceMgmt.h>
#include <fileio/fileio.h>
#include <fileio/compression/CCarmack.h>
#include <fileio/compression/CRLE.h>
#include <base/video/CVideoDriver.h>
#include <base/GsLogging.h>
#include <base/interface/StringUtils.h>
#include <base/interface/FindFile.h>
#include <base/audio/music/CMusic.h>

#include <fstream>

namespace galaxy
{



CMapLoaderGalaxy::CMapLoaderGalaxy(std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
        std::vector<CInventory> &inventoryVec):
m_ObjectPtr(ObjectPtr),
mInventoryVec(inventoryVec),
mNumLoadedPlayers(0)
{}

// Gets returns the address of the datablock of the exe file, in where the
size_t CMapLoaderGalaxy::getMapheadOffset()
{
	size_t offset = 0;

    const auto ep = gKeenFiles.exeFile.getEpisode();
    const auto version = gKeenFiles.exeFile.getEXEVersion();

    switch(ep)
	{
	case 4:	offset = 0x24830; break;
	case 5:	offset = 0x25990; break;
    case 6:
    {
        if(version == 150)
            offset = 0x29AE0;
        else
            offset = gKeenFiles.exeFile.isDemo() ? 0x1F560 : 0x25080;
        break;
    }
	default:	break;
	}

	// Get the offset of the MAPHEAD. It is told that it always begins with $ABCD as word type
	return offset;
}


size_t findInStream(std::ifstream &stream, const std::string &sig)
{
  const size_t start = stream.tellg();
    
  while(!stream.eof())
  {	
	auto it = sig.begin();
	for( size_t s=0 ; it != sig.end() ; it++, s++ )
	{
	  char c = stream.get();
	  
	  if( sig[s] != c )
	    break;
	}
	
	if( it == sig.end() )
	  break;
  }
  
  size_t pos;
  
  if(stream.eof())
    pos = std::string::npos;
  else
    pos = stream.tellg();
  
  stream.seekg(start);
  
  return pos;
}


bool CMapLoaderGalaxy::gotoNextSignature(std::ifstream &MapFile)
{
  // try the original "!ID!" Sig...
	size_t pos = findInStream(MapFile, "!ID!");
	MapFile.seekg( pos, std::ios::beg );
	
	if(pos != std::string::npos)
	  return true;

	gLogging.textOut("Warning! Your are opening a map which is not correctly signed. Some Mods, using different Editors, have that issue!!");
    gLogging.textOut("If you are playing a mod it might okay though. If it's an original game, it is tainted and you should get a better copy. Continuing...");
	
	return false;	
}

// never allow more than 100 MB of uncompressed data. Anything larger is assumed too large
const size_t fileSizeLimit = 100 * 1024 * 1024;

bool CMapLoaderGalaxy::unpackPlaneData( std::ifstream &mapFile,
                                        std::shared_ptr<CMap> &map,
                                        const size_t planeNumber,
                                        longword offset,
                                        longword length,
                                        word magic_word)
{
    size_t initial_pos = mapFile.tellg();

    std::vector<word> plane;

    mapFile.seekg(offset);

	std::vector<gs_byte> Carmack_Plane;	

	for(size_t i=0 ; i<length ; i++)
    {
        const gs_byte value = mapFile.get();
        Carmack_Plane.push_back( value );
    }

	size_t decarmacksize = (Carmack_Plane.at(1)<<8)+Carmack_Plane.at(0);

	
    if(decarmacksize > fileSizeLimit)
    {
        gLogging.textOut( "\nERROR: Plane is too large at " + itoa(decarmacksize) + ".<br>");
        return false;
    }

      
	// Now use the Carmack Decompression
	CCarmack Carmack;
    std::vector<gs_byte> RLE_Plane;

    Carmack.expand(RLE_Plane, Carmack_Plane, decarmacksize);
	
    if( decarmacksize >= RLE_Plane.size() )
    {                  
    	// Now use the RLE Decompression
    	CRLE RLE;
        const size_t derlesize = (RLE_Plane[0]<<8) + RLE_Plane[1];

        if(derlesize >= 0xffff)
        {
            gLogging.textOut( "\nERROR: Map is too tall.");
            return false;
        }


        RLE.expand(plane, RLE_Plane, magic_word);

        word *mapPtr = (planeNumber == 2) ?
                          map->getInfoData() : map->getData(planeNumber);


        for(size_t y=0; y<map->m_height ; ++y)
    	{
            const int stride = y*map->m_width;

            for(size_t x=0; x<map->m_width ; ++x)
    		{
                const int offset = stride+x;
                const word tile = plane.at(offset);

                *mapPtr = tile;
                mapPtr++;
    		}
    	}

        if( derlesize/2 != plane.size() )
        {
            gLogging.textOut( "\nERROR Plane Uncompress RLE Size Failed: Actual "+ itoa(2*plane.size()) +" bytes Expected " + itoa(derlesize) + " bytes<br>");
            return false;
        }
    }
    else
    {
    	gLogging.textOut( "\nERROR Plane Uncompress Carmack Size Failed: Actual " + itoa(RLE_Plane.size()) + " bytes Expected " + itoa(decarmacksize) + " bytes<br>");
        return false;
    }

    mapFile.seekg(initial_pos);

    return true;
}


bool CMapLoaderGalaxy::loadMap(std::shared_ptr<CMap> &map, Uint8 level)
{
    bool ok = true;

    // Get the MAPHEAD Location from within the Exe File or an external file
    std::vector<char> mapHeadContainer;
    const std::string &path = gKeenFiles.gameDir;

    map.reset(new CMap());

    // Set Map position and some flags for the freshly loaded level    
    map->setLevel(level);
    map->isSecret = false;
    map->mNumFuses = 0;

    // In case no external file was read, let's use data from the embedded data
    gs_byte *Maphead = gKeenFiles.exeFile.getRawData() + getMapheadOffset();

    // In case there is an external file read it into the container and replace the pointer
    const std::string mapHeadFilename = gKeenFiles.mapheadFilename;
    std::ifstream MapHeadFile;

    if( OpenGameFileR(MapHeadFile, getResourceFilename(mapHeadFilename,path,false,false), std::ios::binary) )
    {
        // get length of file:
        MapHeadFile.seekg (0, std::ios::end);
        unsigned int length = MapHeadFile.tellg();
        MapHeadFile.seekg (0, std::ios::beg);
        mapHeadContainer.resize(length);
        MapHeadFile.read(&mapHeadContainer[0],length*sizeof(char));
        Maphead = reinterpret_cast<gs_byte*>(&mapHeadContainer[0]);
    }

    word magic_word;
    longword level_offset;

    // Get the magic number of the level data from MAPHEAD Located in the EXE-File.
    // This is used for the decompression.
    magic_word = READWORD(Maphead);

    // Get location of the level data from MAPHEAD Located in the EXE-File.
    Maphead += level*sizeof(longword);
    level_offset = READLONGWORD(Maphead);

    // Open the Gamemaps file
    std::string gamemapfile = gKeenFiles.gamemapsFilename;

    std::ifstream MapFile;
    if(!OpenGameFileR(MapFile,
                      getResourceFilename(gamemapfile,path,true,false), std::ios::binary))
    {
        gLogging.ftextOut("Error while trying to open the \"%s\" file!", gamemapfile.c_str() );
        return false;
    }

    if(level_offset == 0 && mapHeadContainer.empty())
    {
        MapFile.close();
        gLogging.textOut("This Level doesn't exist in GameMaps");
        return false;
    }

    // Then jump to that location and read the level map data
    MapFile.seekg (level_offset, std::ios::beg);

    int headbegin;

    // Get the level plane header
    if(gotoNextSignature(MapFile))
    {
        /*
     *			  Plane Offsets:  Long[3]   Offset within GAMEMAPS to the start of the plane.  The first offset is for the background plane, the
     *                           second for the foreground plane, and the third for the info plane (see below).
     *			  Plane Lengths:  Word[3]   Length (in bytes) of the compressed plane data.  The first length is for the background plane, the
     *                           second for the foreground plane, and the third for the info plane (see below).
     *			  Width:          Word      Level width (in tiles).
     *			  Height:         Word      Level height (in tiles).  Together with Width, this can be used to calculate the uncompressed
     *										size of plane data, by multiplying Width by Height and multiplying the result by sizeof(Word).
     *			  Name:           Byte[16]  Null-terminated string specifying the name of the level.  This name is used only by TED5, not by Keen.
     *			  Signature:      Byte[4]   Marks the end of the Level Header.  Always "!ID!".
     */
        int jumpback = 3*sizeof(longword) + 3*sizeof(word) +
                       2*sizeof(word) + 16*sizeof(gs_byte) + 4*sizeof(gs_byte);

        headbegin = static_cast<int>(MapFile.tellg()) - jumpback;
    }
    else
    {
        MapFile.clear();
        headbegin =  level_offset;
    }

    MapFile.seekg( headbegin, std::ios_base::beg);

    // Get the header of level data
    longword Plane_Offset[3];
    longword Plane_Length[3];
    word Width, Height;
    char name[17];

    // Get the plane offsets
    Plane_Offset[0] = fgetl(MapFile);
    Plane_Offset[1] = fgetl(MapFile);
    Plane_Offset[2] = fgetl(MapFile);

    // Get the dimensions of the level
    Plane_Length[0] = fgetw(MapFile);
    Plane_Length[1] = fgetw(MapFile);
    Plane_Length[2] = fgetw(MapFile);

    Width = fgetw(MapFile);
    Height = fgetw(MapFile);


    if(Width>1024 || Height>1024)
    {
        gLogging.textOut("Sorry, but I cannot uncompress this map and must give up."
                         "Please report this to the developers and send that version to them in order to fix it.<br>" );
        return false;
    }


    for(int c=0 ; c<16 ; c++)
    {
        name[c] = MapFile.get();
    }
    name[16] = '\0';

    // Get and check the signature
    gLogging.textOut("Loading the Level \"" + static_cast<std::string>(name) + "\" (Level No. "+ itoa(level) + ")<br>" );
    map->setLevelName(name);

    mLevelName = name;

    // Then decompress the level data using rlew and carmack decompression
    gLogging.textOut("Allocating memory for the level planes ...<br>" );

    // Start with the Background
    map->setupEmptyDataPlanes(2, 16, Width, Height);

    // Start with the Background
    gLogging.textOut("Decompressing the Map... plane 0 (Background)<br>" );
    ok &= unpackPlaneData(MapFile, map, 0, Plane_Offset[0], Plane_Length[0], magic_word);

    // Continue with Foreground
    gLogging.textOut("Decompressing the Map... plane 1 (Foreground)<br>" );
    ok &= unpackPlaneData(MapFile, map, 1, Plane_Offset[1], Plane_Length[1], magic_word);

    // And finish up with the info layer
    gLogging.textOut("Decompressing the Map... plane 2 (Infolayer)<br>" );
    ok &= unpackPlaneData(MapFile, map, 2, Plane_Offset[2], Plane_Length[2], magic_word);

    map->gotoPos(0,0);

    map->resetScrollBlocker();
    map->collectBlockersCoordiantes();
    map->setupAnimationTimer();

    // Now that we have all the 3 planes (Background, Foreground, Foes) unpacked...
    // We only will show the first two of them in the screen, because the Foes one
    // is the one which will be used for spawning the foes (Keen, platforms, enemies, etc.)
    gLogging.textOut("Loading the foes ...<br>" );
    spawnFoes(map);

    if(!ok)
    {
        gLogging.textOut("Something went wrong while loading the map!" );
        return false;
    }


    // Set Scrollbuffer
    map->drawAll();
    gLogging << "Map got loaded successfully!" << CLogFile::endl;

    return true;
}


/**
 * @brief	This will setup the enemies on the map. They are pushed in a objects vector,
 * 			so they can interact all the time
 */
void CMapLoaderGalaxy::spawnFoes(std::shared_ptr<CMap> &map)
{
    word *start_data = map->getInfoData();
	word *data_ptr;
    word width = map->m_width;
    word height = map->m_height;

	// If objects are in the buffer clean them up

	if(!m_ObjectPtr.empty())
		m_ObjectPtr.clear();

    const int numPlayers = gBehaviorEngine.numPlayers();

    // Time to add the foe objects
    map->mNumFuses = 0;
    map->mFuseInLevel = false;
	data_ptr = start_data;

	for(size_t y=0 ; y<height ; y++)
	{
		for(size_t x=0 ; x<width ; x++)
		{
            const int foeID = *data_ptr++;

            // Skip unused 0 foe id, as an optimisation.
            if(foeID == 0)
                continue;

            if(x==39 && y==21)
            {
                printf("Test");
            }

            // Check if it is the player, because in multiplayer we spawn multiple keens
            if(isKeenPlayer(foeID)) // World Map only
            {
                for(int i = 0 ; i < numPlayers ; i++)
                {
                    std::shared_ptr<CGalaxySpriteObject> pNewfoe( addFoe(map, foeID, x<<CSF, y<<CSF) );

                    if(pNewfoe)
                        m_ObjectPtr.push_back(pNewfoe);
                }
            }
            else
            {
                std::shared_ptr<CGalaxySpriteObject> pNewfoe( addFoe(map, foeID, x<<CSF, y<<CSF) );

                if(pNewfoe)
                {
                    m_ObjectPtr.push_back(pNewfoe);
                }
            }
		}
	}

    // Extra Lua spawns
    GsLua lua;
    auto ok = lua.loadFile(
                        JoinPaths(gKeenFiles.gameDir, "extraSpawn.lua"));

    if(ok)
    {
        int forLevel = -1;

        ok = lua.runFunctionRetOneInt("spawnForLevel", forLevel);
        if(ok && forLevel == map->getLevel())
        {
            int numFoes = 0;

            ok = lua.runFunctionRetOneInt("howMany", numFoes);


            for(int i=1 ; i<=numFoes ; i++)
            {
                int foeIdx = -1;
                std::array<int,2> coordArray;

                lua.runFunctionRetOneInt("who", i, foeIdx);
                lua.runFunctionRetOneInt("where_x", i, coordArray[0]);
                lua.runFunctionRetOneInt("where_y", i, coordArray[1]);

                std::shared_ptr<CGalaxySpriteObject> pNewfoe(
                            addFoe(map, foeIdx,
                                   coordArray[0]<<CSF, coordArray[1]<<CSF) );

                if(pNewfoe)
                {
                    m_ObjectPtr.push_back(pNewfoe);
                }
            }
        }
    }

}

}
