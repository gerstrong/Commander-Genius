/*
 * CEGAGraphics.cpp
 *
 *  Created on: 27.12.2009
 *      Author: gerstrong
 *
 *  Decompression and decoding routine for
 *  Commander Keen Galaxy Series.
 *  EGAHEAD and EGADICT are normally files that have been embedded
 *  into the executable.
 *  EGAGRAPH is a seperate file which contains the graphics itself
 *  All thre files are needed to extract the graphics
 */

#include "CEGAGraphicsGalaxy.h"
#include "../../graphics/CGfxEngine.h"
#include "../../FindFile.h"
#include "../../fileio/CHuffman.h"
#include "../../CLogFile.h"
#include "../../StringUtils.h"
#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../engine/CPlanes.h"
#include <fstream>
#include <cstring>
#include <SDL.h>

namespace galaxy
{

// The ExeImageSize fields are values from the real start of exe image,
// i.e. KEEN4E.EXE + 0x2E00 to EndOfFile=0x3D740
// It seems that the standard shareware KDREAMS.EXE starts at 0x1C00

static EpisodeInfoStruct EpisodeInfo[] = {
	{	/* Episode 4 */
		0x3D740,	/* ExeImageSize */
		0x21080,	/* OffEgaHead */
		0x358F6,	/* OffEgaDict */
		4751,		/* NumChunks */
		3, 3,		/* NumFonts, IndexFonts */
		115, 6,		/* NumBitmaps, IndexBitmaps */
		3, 121,		/* NumMaskedBitmaps, IndexMasked Bitmaps */
		397, 124,	/* NumSprites, IndexSprites */
		104, 521,	/* Num8Tiles, Index8Tiles */
		20, 522,	/* Num8MaskedTiles, Index8MaskedTiles */
		1296, 523,	/* Num16Tiles, Index16Tiles */
		2916, 1819,	/* Num16MaskedTiles, Index16MaskedTiles */
		4, 4735,	/* NumMisc, IndexMisc */
		7, 4739,	/* NumTexts, IndexTexts */
		5, 4746		/* NumDemos, IndexDemos */
	},
	{	/* Episode 5 */
		0x3E370,	/* ExeImageSize */
		0x21FC0,	/* OffEgaHead */
		0x362C4,	/* OffEgaDict */
		4931,		/* NumChunks */
		3, 3,		/* NumFonts, IndexFonts */
		93, 6,		/* NumBitmaps, IndexBitmaps */
		3, 99,		/* NumMaskedBitmaps, IndexMasked Bitmaps */
		346, 102,	/* NumSprites, IndexSprites */
		104, 448,	/* Num8Tiles, Index8Tiles */
		20, 449,	/* Num8MaskedTiles, Index8MaskedTiles */
		1512, 450,	/* Num16Tiles, Index16Tiles */
		2952, 1962,	/* Num16MaskedTiles, Index16MaskedTiles */
		5, 4921,	/* NumMisc, IndexMisc */
		7, 4914,	/* NumTexts, IndexTexts */
		5, 4926		/* NumDemos, IndexDemos */
	},
	{	/* Episode 6 */
		0x3F950,	/* ExeImageSize */
		0x20F50,	/* OffEgaHead */
		0x372EE,	/* OffEgaDict */
		5560,		/* NumChunks */
		3, 3,		/* NumFonts, IndexFonts */
		37, 6,		/* NumBitmaps, IndexBitmaps */
		3, 43,		/* NumMaskedBitmaps, IndexMasked Bitmaps */
		390, 46,	/* NumSprites, IndexSprites */
		104, 436,	/* Num8Tiles, Index8Tiles */
		12, 437,	/* Num8MaskedTiles, Index8MaskedTiles */
		2376, 438,	/* Num16Tiles, Index16Tiles */
		2736, 2814,	/* Num16MaskedTiles, Index16MaskedTiles */
		4, 5551,	/* NumMisc, IndexMisc */
		1, 5550,	/* NumTexts, IndexTexts */
		5, 5555		/* NumDemos, IndexDemos */
	},
	{	/* Episode 7 (Dreams) */
		// The following ExeImageSize = KDREAMS.EXE (213536) - 0x1C00:
		206368,		/* ExeImageSize */
		0x1CB20,	/* OffEgaHead */
		0x29F12,	/* OffEgaDict */
		3019,		/* NumChunks */
		0, 0,		/* NumFonts, IndexFonts */
		0, 0,		/* NumBitmaps, IndexBitmaps */
		0, 0,		/* NumMaskedBitmaps, IndexMasked Bitmaps */
		0, 0,		/* NumSprites, IndexSprites */
		1, 368,		/* Num8Tiles, Index8Tiles */
		1, 369,		/* Num8MaskedTiles, Index8MaskedTiles */
		1440, 370,	/* Num16Tiles, Index16Tiles */
		1224,1810 ,	/* Num16MaskedTiles, Index16MaskedTiles */
		0, 0,		/* NumMisc, IndexMisc */
		0, 0,		/* NumTexts, IndexTexts */
		0, 0		/* NumDemos, IndexDemos */
	}
};

/////
// Class members start here!
/////
CEGAGraphicsGalaxy::CEGAGraphicsGalaxy(short episode, const std::string& path, CExeFile &ExeFile) :
CEGAGraphics(episode, path),
m_Exefile(ExeFile)
{}

bool CEGAGraphicsGalaxy::loadData()
{
	if(!begin()) return false;
	if(!readBitmaps()) return false;

	return true;
}

bool CEGAGraphicsGalaxy::begin()
{
	// The stuff is Huffman compressed. Use an instance for that
	CHuffman Huffman;
	unsigned long exeheaderlen = 0;
	unsigned long exeimglen = 0;
	int ep = m_episode - 4;

	unsigned char *p_data = m_Exefile.getHeaderData();

	//if(m_episode == 7) exeheaderlen = HEADERLEN_KDREAMS;
	if(!m_Exefile.readExeImageSize( p_data, &exeimglen, &exeheaderlen))
		return false;

	// We need the EGADICT. Read it to our structure of Huffman, he needs it!
	Huffman.readDictionary(p_data, exeheaderlen + EpisodeInfo[ep].OffEgaDict);

	// Now we go to EGAHEAD
	// TODO: Implement a structure which will look for the files first and take them if possible

	// Read the EGAHEAD
	unsigned char *p_head = p_data + exeheaderlen + EpisodeInfo[ep].OffEgaHead;
	unsigned long offset = 0;
	unsigned long offset_limit;

	// For some reason, MultiMania's KDR support uses a slightly different limit
	// in offset ops. We're not in DOS, so we don't have to worry about
	// memory here >:P
	if (ep < 3) offset_limit = 0x00FFFFFF;
	else offset_limit = 0xFFFFFFFF;

	for(size_t i = 0 ; i < EpisodeInfo[ep].NumChunks ; i++)
	{
		if (ep != 3) {
			memcpy(&offset, p_head,3); // Keen 4-6
			p_head += 3;
			offset &= offset_limit;
		}
		else {
			memcpy(&offset, p_head,4); // KeenDreams
			p_head += 4;
		}
		m_egahead.push_back(offset);
	}

	// Now read the EGAGRAPH
	std::string filename;
	if (ep < 3) filename =  m_path + "EGAGRAPH.CK" + itoa(m_episode);
	else filename =  m_path + "KDREAMS.EGA";

	std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);

	if(!File)
	{
		g_pLogFile->textOut(RED,"Error the file \"" + filename + "\" is missing or can't be read!");
		return false;
	}

	int egagraphlen;
	File.seekg(1,std::ios::end);
	egagraphlen = File.tellg();
	egagraphlen--;
	File.seekg(0,std::ios::beg);

	unsigned char *CompEgaGraphData = new unsigned char[egagraphlen];

	File.read((char*)CompEgaGraphData, egagraphlen);

	// Make a clean memory pattern
	ChunkStruct ChunkTemplate;
	ChunkTemplate.len=0;
	m_egagraph.assign(EpisodeInfo[ep].NumChunks, ChunkTemplate);

	unsigned long inlen, outlen;
	inlen = outlen = 0;

	// Now lets decompress the graphics
	for(size_t i = 0; i < EpisodeInfo[ep].NumChunks; i++)
	{
		// Show that something is happening
		offset = m_egahead[i];

		// Make sure the chunk is valid
		if(offset != offset_limit)
		{
			// Get the expanded length of the chunk
			if(i >= EpisodeInfo[ep].Index8Tiles && i < EpisodeInfo[ep].Index16MaskedTiles + EpisodeInfo[ep].Num16MaskedTiles)
			{
				// Expanded sizes of 8, 16,and 32 tiles are implicit
				if(i >= EpisodeInfo[ep].Index16MaskedTiles) // 16x16 tiles are one/chunk
					outlen = 2 * 16 * 5;
				else if(i >= EpisodeInfo[ep].Index16Tiles)
					outlen = 2 * 16 * 4;
				else if(i >= EpisodeInfo[ep].Index8MaskedTiles)	// 8x8 tiles are all in one chunk!
					outlen = EpisodeInfo[ep].Num8MaskedTiles * 8 * 5;
				else if(i >= EpisodeInfo[ep].Index8Tiles)
					outlen = EpisodeInfo[ep].Num8Tiles * 8 * 4;
			}
			else
			{
				memcpy(&outlen, CompEgaGraphData + offset, 4);
				offset += 4;
			}

			// Allocate memory and decompress the chunk
			m_egagraph[i].len = outlen;
			m_egagraph[i].data.assign(outlen, 0);

			inlen = 0;
			// Find out the input length
			size_t j;
			for(j = i + 1; j < EpisodeInfo[ep].NumChunks; j++)
			{
				if(m_egahead[j] != offset_limit)
				{
					inlen = m_egahead[j] - offset;
					break;
				}
			}
			if(j == EpisodeInfo[ep].NumChunks)
				inlen = egagraphlen - offset;
			Huffman.expand(CompEgaGraphData + offset, &m_egagraph[i].data[0], inlen, outlen);
		}
		else
		{
			m_egagraph[i].len = 0;
		}
	}

	// Set up pointers to bitmap and sprite tables if not doing KDR operations
	//if (ep < 4) {
		//BmpHead = (BitmapHeadStruct *)m_egagraph[0].data;
		//BmpMaskedHead = (BitmapHeadStruct *)m_EgaGraph[1].data;
		//SprHead = (SpriteHeadStruct *)m_EgaGraph[2].data;
	//}

	delete [] CompEgaGraphData;

	File.close();
	return true;
}

// This function gets the bit of an unsigned char variable
Uint8 CEGAGraphicsGalaxy::getBit(unsigned char data, Uint8 leftshift)
{
	Uint8 value;

	value = data & (1<<leftshift);
	value >>= leftshift;

	return value;
}

// The colours are not correct when you select the engine first time. After playing a Keen game they are okay...

// This one extracts the bitmaps used in Keen 4-6 (Maybe Dreams in future)
bool CEGAGraphicsGalaxy::readBitmaps()
{
	int ep = m_episode - 4;
	BitmapHeadStruct *BmpHead = (BitmapHeadStruct *) &(m_egagraph.at(0).data.at(0));
	SDL_Color *Palette = g_pGfxEngine->Palette.m_Palette;

	g_pGfxEngine->createEmptyBitmaps(EpisodeInfo[ep].NumBitmaps);

	for(size_t i = 0; i < EpisodeInfo[ep].NumBitmaps; i++)
	{
		CBitmap &Bitmap = g_pGfxEngine->getBitmap(i);
		Bitmap.setDimensions(BmpHead[i].Width*8, BmpHead[i].Height);
		Bitmap.createSurface(g_pVideoDriver->getScrollSurface()->flags, Palette);

		SDL_Surface* sfc = Bitmap.getSDLSurface();

		if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
		SDL_FillRect(sfc, NULL, 0x00);

		if(!m_egagraph.at(EpisodeInfo[ep].IndexBitmaps + i).data.empty())
		{
			// Decode the bitmap data
			for(int p = 0; p < 4; p++)
			{
				unsigned char *pointer;
				Uint8* pixel = (Uint8*) sfc->pixels;

				// get location of plane p
				pointer = &(m_egagraph[EpisodeInfo[ep].IndexBitmaps + i].data[0]) + p * BmpHead[i].Width * BmpHead[i].Height;

				// now try to extract the bits and pass it to the SDL-Surface
				for(size_t y = 0; y < BmpHead[i].Height; y++)
				{
					for(size_t x = 0; x < BmpHead[i].Width; x++)
					{
						Uint8 bit,b;
						for(b=0 ; b<8 ; b++)
						{
							bit = getBit(*pointer, 7-b);
							*pixel |= (bit<<p);
							pixel++;
						}
						pointer++;
					}
				}
			}
		}
	}
	return true;
}

CEGAGraphicsGalaxy::~CEGAGraphicsGalaxy()
{
}

}
