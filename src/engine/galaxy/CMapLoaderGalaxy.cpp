
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
#include "StringUtils.h"
#include "FindFile.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CCarmack.h"
#include "fileio/compression/CRLE.h"
#include "fileio.h"
#include "sdl/CVideoDriver.h"
#include "sdl/music/CMusic.h"
#include "CLogFile.h"
#include "engine/CCamera.h"

/// AI Headers

// General stuff
#include "engine/galaxy/ai/CSpriteItem.h"

#include <fstream>

namespace galaxy
{

CMapLoaderGalaxy::CMapLoaderGalaxy(CExeFile &ExeFile,
		std::vector< SmartPointer<CGalaxySpriteObject> > &ObjectPtr,
		CInventory &Inventory, stCheat &Cheatmode):
m_ExeFile(ExeFile),
m_ObjectPtr(ObjectPtr),
m_Inventory(Inventory),
m_Cheatmode(Cheatmode)
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

bool CMapLoaderGalaxy::gotoSignature(std::ifstream &MapFile)
{
	char c;
	while(!MapFile.eof())
	{
		c = MapFile.get();
		if(c == '!')
		{
			c = MapFile.get();
			if(c == 'I')
			{
				c = MapFile.get();
				if(c == 'D')
				{
					c = MapFile.get();
					if(c == '!')
						return true;
				}
			}
		}
	}
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
    if( decarmacksize == RLE_Plane.size() )
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
            g_pLogFile->textOut( "\nERROR Plane Uncompress RLE Size Failed: Actual "+ itoa(2*Plane.size()) +" bytes Expected " + itoa(derlesize) + " bytes<br>");
        }
    }
    else
    {
    	g_pLogFile->textOut( "\nERROR Plane Uncompress Carmack Size Failed: Actual " + itoa(RLE_Plane.size()) + " bytes Expected " + itoa(decarmacksize) + " bytes<br>");
    }

    MapFile.seekg(initial_pos);
}


bool CMapLoaderGalaxy::loadMap(CMap &Map, Uint8 level)
{
	// Get the MAPHEAD Location from within the Exe File or an external file
	std::vector<char> mapHeadContainer;
	std::string path = m_ExeFile.getDataDirectory();


    Map.gotoPos(0,0);
    Map.setLevel(level);

	// In case no external file was read, let's use data from the embedded data
	byte *Maphead = m_ExeFile.getRawData() + getMapheadOffset();

	// In case there is an external file read it into the container and replace the pointer
	if(gpResource->mapheadFilename != "")
	{
		std::ifstream MapHeadFile;
		if(OpenGameFileR(MapHeadFile, getResourceFilename(gpResource->mapheadFilename,path,true,false), std::ios::binary))
		{
			// get length of file:
			MapHeadFile.seekg (0, std::ios::end);
			unsigned int length = MapHeadFile.tellg();
			MapHeadFile.seekg (0, std::ios::beg);
			mapHeadContainer.resize(length);
			MapHeadFile.read(&mapHeadContainer[0],length*sizeof(char));
			Maphead = reinterpret_cast<byte*>(&mapHeadContainer[0]);
		}
		else
		{
			g_pLogFile->textOut("ERROR The MapHead File was not found. Please check that file or take a look into your patch file");
		}
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
	std::string gamemapfile = gpResource->gamemapsFilename;

	std::ifstream MapFile;
	if(OpenGameFileR(MapFile, getResourceFilename(gamemapfile,path,true,false), std::ios::binary))
	{
		if(level_offset != 0)
		{
			// Then jump to that location and read the level map data
			MapFile.seekg (level_offset, std::ios::beg);

			// Get the level plane header
			if(gotoSignature(MapFile))
			{
				/*
				  Plane Offsets:  Long[3]   Offset within GAMEMAPS to the start of the plane.  The first offset is for the background plane, the
                            second for the foreground plane, and the third for the info plane (see below).
				  Plane Lengths:  Word[3]   Length (in bytes) of the compressed plane data.  The first length is for the background plane, the
                            second for the foreground plane, and the third for the info plane (see below).
				  Width:          Word      Level width (in tiles).
				  Height:         Word      Level height (in tiles).  Together with Width, this can be used to calculate the uncompressed
											size of plane data, by multiplying Width by Height and multiplying the result by sizeof(Word).
				  Name:           Byte[16]  Null-terminated string specifying the name of the level.  This name is used only by TED5, not by Keen.
				  Signature:      Byte[4]   Marks the end of the Level Header.  Always "!ID!".
				 */
				int jumpback = 3*sizeof(longword) + 3*sizeof(word) +
								  2*sizeof(word) + 16*sizeof(byte) + 4*sizeof(byte);

				int headbegin = static_cast<int>(MapFile.tellg()) - jumpback;
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

				for(int c=0 ; c<16 ; c++)
					name[c] = MapFile.get();
				name[16] = '\0';

				// Get and check the signature
				g_pLogFile->textOut("Loading the Level \"" + static_cast<std::string>(name) + "\" (Level No. "+ itoa(level) + ")<br>" );
				Map.setLevelName(name);

				// Then decompress the level data using rlew and carmack
				g_pLogFile->textOut("Decompressing the Map...<br>" );

				// Start with the Background
				Map.createEmptyDataPlane(0, Width, Height);
				Map.createEmptyDataPlane(1, Width, Height);
				Map.createEmptyDataPlane(2, Width, Height);

				unpackPlaneData(MapFile, Map, 0, Plane_Offset[0], Plane_Length[0], magic_word);
				unpackPlaneData(MapFile, Map, 1, Plane_Offset[1], Plane_Length[1], magic_word);
				unpackPlaneData(MapFile, Map, 2, Plane_Offset[2], Plane_Length[2], magic_word);
			}
			MapFile.close();

			// Now that we have all the 3 planes (Background, Foreground, Foes) unpacked...
			// We only will show the first two of them in the screen, because the Foes one
			// is the one which will be used for spawning the foes (Keen, platforms, enemies, etc.)
			spawnFoes(Map);
		}
		else
		{
			MapFile.close();
			g_pLogFile->textOut("This Level doesn't exist in GameMaps");
			return false;
		}
	}
	else
		return false;

	// TODO: Now load the player and enemies

    // Do some post calculations
    // Limit the scroll screens so the blocking (blue in EP1) tiles are3 never seen
    SDL_Rect gamerect = g_pVideoDriver->getGameResolution().SDLRect();
    Map.m_maxscrollx = (Map.m_width<<4) - gamerect.w - 32;
    Map.m_maxscrolly = (Map.m_height<<4) - gamerect.h - 32;

    // Set Scrollbuffer
    g_pVideoDriver->updateScrollBuffer(Map);

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

	// he we go to the adding objects
	data_ptr = start_data;
	for(size_t y=0 ; y<height ; y++)
	{
		for(size_t x=0 ; x<width ; x++)
		{
			CGalaxySpriteObject *pNewfoe = addFoe(Map, *data_ptr++, x<<CSF, y<<CSF);
			if(pNewfoe)
				m_ObjectPtr.push_back(pNewfoe);
		}
	}

	/*std::ofstream File("objlayer.txt");

	// he we go to the adding objects
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
	/*std::ofstream FileTile("tilelayer.txt");

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
	}

	FileTile.close();*/
}

//------------------------------------------------//
// The following part covers the load of the foes
// Some defines are hardcoded so it is easier to
// identify what type of foe/stuff is loaded

/**
 * @brief	Loads a foe given by the coordiantes
 */
CGalaxySpriteObject* CMapLoaderGalaxy::addFoe(CMap &Map, word foe, size_t x, size_t y)
{
	CGalaxySpriteObject *p_newfoe = NULL;

	// Point Item Sprites (Candies, etc...)
	for( Uint32 i=61 ; i<=67 ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 103+2*(i-61);
			p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, m_ObjectPtr, newsprite);
		}
	}

	// Neuronal-stunner
	if( foe == 68 )
	{
		p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, m_ObjectPtr, 127);
	}

	// Gems
	for( Uint32 i=57 ; i<=60 ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 118+2*(i-57);
			p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, m_ObjectPtr, newsprite);
		}
	}


	return p_newfoe;
}

}
