
/*
 * CMapLoaderGalaxy.cpp
 *
 *  Created on: 29.05.2010
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxy.h"
#include "StringUtils.h"
#include "FindFile.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CCarmack.h"
#include "fileio/compression/CRLE.h"
#include "fileio.h"
#include "sdl/CVideoDriver.h"
#include "CLogFile.h"
#include "engine/CCamera.h"

// AI Headers
#include "engine/galaxy/ai/CMiragia.h"
#include "engine/galaxy/ai/CPlayerWM.h"
#include "engine/galaxy/ai/CPlayerLevel.h"

#include <fstream>

CMapLoaderGalaxy::CMapLoaderGalaxy(CExeFile &ExeFile, std::vector<CObject*>& ObjectPtr):
m_ExeFile(ExeFile),
m_ObjectPtr(ObjectPtr)
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
	// Get the MAPHEAD Location from within the Exe File

    Map.gotoPos(0,0);
    Map.setLevel(level);

	size_t offset = getMapheadOffset();
	byte *Maphead = m_ExeFile.getRawData() + offset;
	word magic_word;
	longword level_offset;
	std::string path;

	// Get the magic number of the level data from MAPHEAD Located in the EXE-File.
	// This is used for the decompression.
	magic_word = READWORD(Maphead);

	// Get location of the level data from MAPHEAD Located in the EXE-File.
	Maphead += level*sizeof(longword);
	level_offset = READLONGWORD(Maphead);

	// Open the Gamemaps file
	path = m_ExeFile.getDataDirectory();
	std::string gamemapfile = "gamemaps.ck"+itoa(m_ExeFile.getEpisode());

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
				g_pLogFile->textOut("Loading the Level \"" + static_cast<std::string>(name) + "\"<br>" );

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
    SDL_Rect gamerect = g_pVideoDriver->getGameResolution();
    Map.m_maxscrollx = (Map.m_width<<4) - gamerect.w - 32;
    Map.m_maxscrolly = (Map.m_height<<4) - gamerect.h - 32;

    // Set Scrollbuffer
    g_pVideoDriver->setScrollBuffer(&Map.m_scrollx_buf, &Map.m_scrolly_buf);

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

	/*std::ofstream file("foe.txt");
	data_ptr = start_data;
	for(size_t y=0 ; y<height ; y++)
	{
		for(size_t x=0 ; x<width ; x++)
		{
			file << (*data_ptr);
			file << " ";
			data_ptr++;
		}
		file << std::endl;
	}
	file.close();*/

	// If objects are in the buffer clean them up
	while(!m_ObjectPtr.empty())
	{
		delete m_ObjectPtr.back();
		m_ObjectPtr.pop_back();
	}

	// he we go to the adding objects
	data_ptr = start_data;
	for(size_t y=0 ; y<height ; y++)
	{
		for(size_t x=0 ; x<width ; x++)
		{
			addFoe(Map, *data_ptr, x, y);
			data_ptr++;
		}
	}
}

/**
 * @brief	Loads a foe given by the coordiantes
 */
void CMapLoaderGalaxy::addFoe(CMap &Map, word foe, size_t x, size_t y)
{
	x <<= CSF;
	y <<= CSF;

	CObject *p_newfoe = NULL;
	CCamera *camera = NULL;
	VectorD2<Uint32> Location(x,y);

	switch(foe)
	{
	case 1:
	case 2:
		// This is the player on map
		p_newfoe = new galaxy::CPlayerLevel(&Map, x, y, m_ObjectPtr, (foe==1) ? RIGHT : LEFT );

		// Add the Camera into the game scene and attach it to this player
		camera = new CCamera(&Map,x,y);
		camera->attachToObject(*p_newfoe);
		m_ObjectPtr.push_back(camera);
		break;

	case 3:
		// This is the player on map
		p_newfoe = new galaxy::CPlayerWM(&Map, x, y, m_ObjectPtr);

		// Add the Camera into the game scene and attach it to this player
		camera = new CCamera(&Map,x,y);
		camera->attachToObject(*p_newfoe);
		m_ObjectPtr.push_back(camera);
		break;

	case 33:

		p_newfoe = new galaxy::CMiragia(&Map, Location);
		break;

	default:
		break;
	}

	if(p_newfoe)
		m_ObjectPtr.push_back(p_newfoe);
}

CMapLoaderGalaxy::~CMapLoaderGalaxy()
{
	// TODO Auto-generated destructor stub
}
