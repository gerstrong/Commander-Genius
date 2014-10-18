
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
#include <base/utils/StringUtils.h>
#include <base/utils/FindFile.h>
#include <fileio/ResourceMgmt.h>
#include "fileio/compression/CCarmack.h"
#include "fileio/compression/CRLE.h"
#include "fileio.h"
#include <base/video/CVideoDriver.h>
#include "sdl/audio/music/CMusic.h"
#include <base/GsLogging.h>
#include "engine/core/CCamera.h"
#include "fileio/KeenFiles.h"

/// AI Headers

// General stuff
#include "../common/ai/CSpriteItem.h"

#include <fstream>

namespace galaxy
{



CMapLoaderGalaxy::CMapLoaderGalaxy(CExeFile &ExeFile,
		std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
        std::vector<CInventory> &inventoryVec, stCheat &Cheatmode):
m_ExeFile(ExeFile),
m_ObjectPtr(ObjectPtr),
mInventoryVec(inventoryVec),
m_Cheatmode(Cheatmode),
mNumLoadedPlayers(0)
{}

// Gets returns the address of the datablock of the exe file, in where the
size_t CMapLoaderGalaxy::getMapheadOffset()
{
	size_t offset = 0;

	switch(m_ExeFile.getEpisode())
	{
	case 4:	offset = 0x24830; break;
	case 5:	offset = 0x25990; break;
	case 6:	offset = 0x25080; break;
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
	gLogging.textOut("If you are playing a mod it might okay though. If it's an original game, it is tainted. Continuing...");
	
	return false;	
}

void CMapLoaderGalaxy::unpackPlaneData(std::ifstream &MapFile,
					CMap &Map, size_t PlaneNumber,
					longword offset, longword length,
					word magic_word)
{
	size_t initial_pos = MapFile.tellg();

	std::vector<word> Plane;

	MapFile.seekg(offset);
	std::vector<byte> Carmack_Plane;
	std::vector<byte> RLE_Plane;
	for(size_t i=0 ; i<length ; i++)
		Carmack_Plane.push_back(MapFile.get());

	size_t decarmacksize = (Carmack_Plane.at(1)<<8)+Carmack_Plane.at(0);

	


      
	// Now use the Carmack Decompression
	CCarmack Carmack;
	Carmack.expand(RLE_Plane, Carmack_Plane);
	Carmack_Plane.clear();      
	
      if( decarmacksize > RLE_Plane.size() )
      {
	  gLogging.textOut( "\nWARNING Plane Uncompress Carmack Size differs to the one of the headers: Actual " + itoa(RLE_Plane.size()) + 
			      " bytes Expected " + itoa(decarmacksize) + " bytes. Trying to reconstruct level anyway!<br>");	  
	  
	  while( decarmacksize > RLE_Plane.size() )
	    RLE_Plane.push_back(0);
      }	
		
	
    if( decarmacksize >= RLE_Plane.size() )
    {
                  
    	// Now use the RLE Decompression
    	CRLE RLE;
        size_t derlesize = (RLE_Plane[0]<<8)+RLE_Plane[1];           // Bytes already swapped
    	RLE.expand(Plane, RLE_Plane, magic_word);
    	RLE_Plane.clear();

    	word *ptr = Map.getData(PlaneNumber);
    	for(size_t y=0; y<Map.m_height ; y++)
    	{
    		for(size_t x=0; x<Map.m_width ; x++)
    		{
    			*ptr = Plane.at(y*Map.m_width+x);
    			ptr++;
    		}
    	}

        if( derlesize/2 != Plane.size() )
        {
            gLogging.textOut( "\nERROR Plane Uncompress RLE Size Failed: Actual "+ itoa(2*Plane.size()) +" bytes Expected " + itoa(derlesize) + " bytes<br>");
        }
    }
    else
    {
    	gLogging.textOut( "\nERROR Plane Uncompress Carmack Size Failed: Actual " + itoa(RLE_Plane.size()) + " bytes Expected " + itoa(decarmacksize) + " bytes<br>");
    }

    MapFile.seekg(initial_pos);
}


bool CMapLoaderGalaxy::loadMap(CMap &Map, Uint8 level)
{
  // Get the MAPHEAD Location from within the Exe File or an external file
  std::vector<char> mapHeadContainer;
  const std::string &path = gKeenFiles.gameDir;
  
  // Set Map position and some flags for the freshly loaded level
  Map.gotoPos(0,0);
  Map.setLevel(level);
  Map.isSecret = false;
  Map.mNumFuses = 0;
  
  // In case no external file was read, let's use data from the embedded data
  byte *Maphead = m_ExeFile.getRawData() + getMapheadOffset();
  
  // In case there is an external file read it into the container and replace the pointer
  const std::string mapHeadFilename = gKeenFiles.mapheadFilename;
  std::ifstream MapHeadFile;

  if(OpenGameFileR(MapHeadFile, getResourceFilename(mapHeadFilename,path,true,false), std::ios::binary))
  {
      // get length of file:
      MapHeadFile.seekg (0, std::ios::end);
      unsigned int length = MapHeadFile.tellg();
      MapHeadFile.seekg (0, std::ios::beg);
      mapHeadContainer.resize(length);
      MapHeadFile.read(&mapHeadContainer[0],length*sizeof(char));
      Maphead = reinterpret_cast<byte*>(&mapHeadContainer[0]);
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
  if(OpenGameFileR(MapFile, getResourceFilename(gamemapfile,path,true,false), std::ios::binary))
  {
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
                2*sizeof(word) + 16*sizeof(byte) + 4*sizeof(byte);

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
    Map.setLevelName(name);

    mLevelName = name;

    // Then decompress the level data using rlew and carmack
    gLogging.textOut("Decompressing the Map...<br>" );

    // Start with the Background
    Map.createEmptyDataPlane(0, Width, Height);
    Map.createEmptyDataPlane(1, Width, Height);
    Map.createEmptyDataPlane(2, Width, Height);

    unpackPlaneData(MapFile, Map, 0, Plane_Offset[0], Plane_Length[0], magic_word);
    unpackPlaneData(MapFile, Map, 1, Plane_Offset[1], Plane_Length[1], magic_word);
    unpackPlaneData(MapFile, Map, 2, Plane_Offset[2], Plane_Length[2], magic_word);


    Map.collectBlockersCoordiantes();
    MapFile.close();

    // Now that we have all the 3 planes (Background, Foreground, Foes) unpacked...
    // We only will show the first two of them in the screen, because the Foes one
    // is the one which will be used for spawning the foes (Keen, platforms, enemies, etc.)
    spawnFoes(Map);
  }
  else
  {
      return false;
  }


  // Set Scrollbuffer
  Map.drawAll();
  gVideoDriver.updateScrollBuffer(Map.m_scrollx, Map.m_scrolly);
  
  return true;
}


/**
 * @brief	This will setup the enemies on the map. They are pushed in a objects vector,
 * 			so they can interact all the time
 */
void CMapLoaderGalaxy::spawnFoes(CMap &Map)
{
	word *start_data = Map.getData(2);
	word *data_ptr;
	word width = Map.m_width;
	word height = Map.m_height;

	// If objects are in the buffer clean them up

	if(!m_ObjectPtr.empty())
		m_ObjectPtr.clear();

    const int numPlayers = g_pBehaviorEngine->mPlayers;

	// he we go to the adding objects
	Map.mNumFuses = 0;
	Map.mFuseInLevel = false;		
	data_ptr = start_data;
	for(size_t y=0 ; y<height ; y++)
	{
		for(size_t x=0 ; x<width ; x++)
		{
            const int foeID = *data_ptr;

            // Check if it is the player, because in multiplayer we spawn multiple keens
            if(isKeenPlayer(foeID)) // World Map only
            {
                for(int i = 0 ; i < numPlayers ; i++)
                {
                    std::shared_ptr<CGalaxySpriteObject> pNewfoe( addFoe(Map, foeID, x<<CSF, y<<CSF) );

                    if(pNewfoe)
                        m_ObjectPtr.push_back(pNewfoe);
                }
            }
            else
            {
                std::shared_ptr<CGalaxySpriteObject> pNewfoe( addFoe(Map, foeID, x<<CSF, y<<CSF) );

                if(pNewfoe)
                    m_ObjectPtr.push_back(pNewfoe);
            }

            data_ptr++;
		}
	}

	/*std::ofstream File("objlayer.txt");

	// Note: This is just for tracing stuff from the level maps
	data_ptr = start_data;
	for(size_t y=0 ; y<height ; y++)
	{
		for(size_t x=0 ; x<width ; x++)
		{
			File << *data_ptr++ << " ";
		}
		File << std::endl;
	}

	File.close();*/


	// Print out information about first plane of the map
	/*std::ofstream FileTile("tilelayer1.txt");

	// he we go to the adding objects
	data_ptr = Map.getData(1);
	for(size_t y=0 ; y<height ; y++)
	{
		for(size_t x=0 ; x<width ; x++)
		{
			data_ptr++;
			FileTile << *data_ptr << " ";
		}
		FileTile << std::endl;
	}*/

	/*std::ofstream FileTile0("tilelayer0.txt");

	// he we go to the adding objects
	data_ptr = Map.getData(0);
	for(size_t y=0 ; y<height ; y++)
	{
		for(size_t x=0 ; x<width ; x++)
		{
			data_ptr++;
			FileTile0 << *data_ptr << " ";
		}
		FileTile0 << std::endl;
	}

	FileTile0.close();*/
}

}
