/*
 * CMapLoaderGalaxy.cpp
 *
 *  Created on: 29.05.2010
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxy.h"
#include "../../StringUtils.h"
#include "../../FindFile.h"
#include "../../fileio/ResourceMgmt.h"
#include "../../fileio/compression/CCarmack.h"
#include "../../fileio/compression/CRLE.h"
#include "../../fileio.h"
#include "../../CLogFile.h"
#include "../../CLogFile.h"
#include <fstream>

CMapLoaderGalaxy::CMapLoaderGalaxy(CExeFile &ExeFile):
m_ExeFile(ExeFile)
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

void CMapLoaderGalaxy::unpackPlaneData(std::ifstream &MapFile, CMap &Map, size_t PlaneNumber,
										longword offset, longword length, word magic_word)
{
	MapFile.seekg(offset);
	std::vector<byte> Carmack_Plane;
	std::vector<byte> RLE_Plane;
	std::vector<word> Plane;
	for(size_t i=0 ; i<length ; i++)
		Carmack_Plane.push_back(MapFile.get());

	// Now use the Carmack Decompression
	CCarmack Carmack;
	Carmack.expand(RLE_Plane, Carmack_Plane);

	// Now use the RLE Decompression
	CRLE RLE;
	RLE.expand(Plane, RLE_Plane, magic_word);

	// TODO: Now read the raw data off the Plane vector...
}


bool CMapLoaderGalaxy::loadMap(CMap &Map, Uint8 level)
{
	// Get the MAPHEAD Location from within the Exe File
	size_t offset = getMapheadOffset();
	byte *Maphead = m_ExeFile.getRawData() + offset;
	word magic_word;
	longword level_offset;
	std::string path;

	// Get the magic number of the level data from MAPHEAD Located in the EXE-File.
	// This is used for the decompression.
	magic_word = READWORD(Maphead);

	// Get location of the level data from MAPHEAD Located in the EXE-File.
	Maphead += (level-1)*sizeof(longword);
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
				size_t jumpback = 3*sizeof(longword) + 3*sizeof(word) +
								  2*sizeof(word) + 16*sizeof(byte) + 4*sizeof(byte);
				MapFile.seekg( -jumpback, std::ios::cur);

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
				Map.createEmptyBackground(Width*Height);
				unpackPlaneData(MapFile, Map, 1, Plane_Offset[0], Plane_Length[0], magic_word);
			}
			MapFile.close();
			return true;
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


}

CMapLoaderGalaxy::~CMapLoaderGalaxy()
{
	// TODO Auto-generated destructor stub
}
