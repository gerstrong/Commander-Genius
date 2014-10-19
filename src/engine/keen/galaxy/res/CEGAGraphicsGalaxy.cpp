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
 *  EGAGRAPH is a separate file which contains the graphics itself
 *  All three files are needed to extract the graphics
 */

#include "CEGAGraphicsGalaxy.h"
#include "graphics/GsGraphics.h"
#include "fileio/ResourceMgmt.h"
#include "engine/core/CBehaviorEngine.h"
#include "fileio/compression/CHuffman.h"
#include "fileio/KeenFiles.h"
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include <base/utils/StringUtils.h>
#include <base/video/CVideoDriver.h>
#include "fileio/CTileLoader.h"
#include "engine/core/CSpriteObject.h"
#include "engine/core/CPlanes.h"
#include <fstream>
#include <cstring>
#include <SDL.h>


namespace galaxy
{

// The ExeImageSize fields are values from the real start of exe image,
// i.e. KEEN4E.EXE + 0x2E00 to EndOfFile=0x3D740
// It seems that the standard shareware KDREAMS.EXE starts at 0x1C00

static EpisodeInfoStruct EpisodeInfo[] =
{
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
/**
 * \brief	This creates the class for reading the graphical
 * \param	episode		Episode of the chosen game
 * \param	path		Path to where the game is found on the media
 * \param	ExeFile		Object to ExeFile in which the
 */
CEGAGraphicsGalaxy::CEGAGraphicsGalaxy(CExeFile &ExeFile) :
CEGAGraphics(ExeFile.getEpisode(), gKeenFiles.gameDir),
m_Exefile(ExeFile)
{
	createBitmapsIDs();
	g_pBehaviorEngine->setEpisodeInfoStructPtr(EpisodeInfo);
}


/**
 * \brief	load the data into the structure
 * \return 	returns true, if loading was successful
 */
bool CEGAGraphicsGalaxy::loadData()
{
	// Set the palette, so the proper colours are loaded
	gGraphics.Palette.setupColorPalettes(m_Exefile.getRawData(), m_episode);

	if(!begin()) return false;

	// First, retrieve the Tile properties so the tilemap gets properly formatted
	// Important especially for masks, and later in the game for the behaviours
	// of those objects
	CTileLoader TileLoader( m_Exefile );
	if(!TileLoader.load(EpisodeInfo[m_episode-4].Num16Tiles,
						EpisodeInfo[m_episode-4].Num16MaskedTiles))
		return false;

    if(!readfonts()) return false;
    if(!readBitmaps()) return false;
    if(!readMaskedBitmaps()) return false;

	gGraphics.createEmptyTilemaps(4);

    if(!readTilemaps(EpisodeInfo[m_episode-4].Num16Tiles, 4, 18,
			EpisodeInfo[m_episode-4].Index16Tiles,
			gGraphics.getTileMap(0), false)) return false;
	if(!readMaskedTilemaps(EpisodeInfo[m_episode-4].Num16MaskedTiles, 4, 18,
			EpisodeInfo[m_episode-4].Index16MaskedTiles,
			gGraphics.getTileMap(1), false)) return false;
    if(!readTilemaps(EpisodeInfo[m_episode-4].Num8Tiles, 3, 1,
			EpisodeInfo[m_episode-4].Index8Tiles,
			gGraphics.getTileMap(2), true)) return false;
	if(!readMaskedTilemaps(EpisodeInfo[m_episode-4].Num8MaskedTiles, 3, 1,
			EpisodeInfo[m_episode-4].Index8MaskedTiles,
			gGraphics.getTileMap(3), true)) return false;

	if(!readSprites( EpisodeInfo[m_episode-4].NumSprites,
			EpisodeInfo[m_episode-4].IndexSprites )) return false;

    if(!readTexts())
        return false;


    //k456_export_texts();

    if( !readMiscStuff() )
        return false;

	//k456_export_demos();
	//k456_export_end();

    // Now try to store a preview if possible
    // Create an intro in case it does not exist yet
    const std::string  path = gKeenFiles.gameDir;
    std::string fullpath = getResourceFilename("preview.bmp", path, false);
    if( fullpath == "" )
    {   // Not found create it
        fullpath = JoinPaths(path, "preview.bmp");
        fullpath = GetWriteFullFileName(fullpath, true);
        GsBitmap *pBitmap = gGraphics.getBitmapFromStr("TITLE");
        SDL_SaveBMP( pBitmap->getSDLSurface(), fullpath.c_str());
    }

	return true;
}

/**
 * \brief 	This function extracts a picture from the galaxy graphics map, and converts it properly to a
 * 			SDL Surface
 */
void CEGAGraphicsGalaxy::extractPicture(SDL_Surface *sfc,
		std::vector<unsigned char> &data, size_t Width, size_t Height,
		bool masked)
{
	if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
	SDL_FillRect(sfc, NULL, 0x0);

	if(!data.empty())
	{
		// Decode the bitmap data
		for(size_t p = 0; p < 4; p++)
		{
			Uint8* pixel = (Uint8*) sfc->pixels;

			// get location of plane p
			if(masked)
			{
                Uint8* pointer = &data[0] + (p+1) * Width * Height;

				// now try to extract the bits and pass it to the SDL-Surface
				for(size_t y = 0; y < Height; y++)
				{
					for(size_t x = 0; x < Width; x++)
					{
						for(Uint8 b=0 ; b<8 ; b++)
						{
                            Uint8 bit = getBit(*pointer, 7-b);
                            *pixel |= (bit<<p);
							pixel++;
						}
						pointer++;
					}
				}
			}
			else
			{
                Uint8* pointer = &data[0] + p * Width * Height;

				// now try to extract the bits and pass it to the SDL-Surface
				for(size_t y = 0; y < Height; y++)
				{
					for(size_t x = 0; x < Width; x++)
					{
						for(Uint8 b=0 ; b<8 ; b++)
						{
                            Uint8 bit = getBit(*pointer, 7-b);
                            *pixel |= (bit<<p);
							pixel++;
						}
						pointer++;
					}
				}
			}
		}

		if(masked)
		{
			// This stuff is for reading the mask and apply it to the pixel map
			Uint8 *pointer = &data[0];
			Uint8* pixel = (Uint8*) sfc->pixels;

			// now try to extract the bits and pass it to the SDL-Surface
			for(size_t y = 0; y < Height; y++)
			{
				for(size_t x = 0; x < Width; x++)
				{
					for(Uint8 b=0 ; b<8 ; b++)
					{
						Uint8 bit = getBit(*pointer, 7-b);
						if(bit == 1)
							*pixel = 16;

						pixel++;
					}
					pointer++;
				}
			}
		}
	}

	if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
}

/**
 * \brief 	This function extracts a tile from the galaxy graphics map, and converts it properly to a
 * 			SDL Surface
 */
void CEGAGraphicsGalaxy::extractTile(SDL_Surface *sfc, std::vector<unsigned char> &data,
		Uint16 size, Uint16 columns, size_t tile, bool usetileoffset)
{
	if(!data.empty())
	{
		// Decode the image data
		for(size_t p = 0; p < 4; p++)
		{
			// Decode the lines of the bitmap data
			size_t tileoff = usetileoffset ? (tile*4*columns*(size/8)*size) : 0;
			Uint8 *pointer = &(data[0]) + tileoff + p * (size/8) * size;
			for(size_t y = 0; y < size; y++)
			{
				Uint8 *pixel = (Uint8*)sfc->pixels +
						size*(tile%columns) +
						size*size*columns*(tile/columns) +
						(size*columns*y);
				for(size_t x = 0; x < (size/8); x++)
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

/**
 * \brief 	This function extracts a masked tile from the galaxy graphics map, and converts it properly to a
 * 			SDL Surface
 */
void CEGAGraphicsGalaxy::extractMaskedTile(SDL_Surface *sfc, std::vector<unsigned char> &data,
		Uint16 size, Uint16 columns, size_t tile, bool usetileoffset)
{
	if(!data.empty())
	{
		// Decode the image data
		size_t tileoff = usetileoffset ? (tile*5*columns*(size/8)*size) : 0;
		for(size_t p = 0; p < 4; p++)
		{
			// Decode the lines of the bitmap data
			Uint8 *pointer = &(data[0]) + tileoff + (p+1) * (size/8) * size;
			for(size_t y = 0; y < size; y++)
			{
				Uint8 *pixel = (Uint8*)sfc->pixels +
						size*(tile%columns) +
						size*size*columns*(tile/columns) +
						(size*columns*y);
				for(size_t x = 0; x < (size/8); x++)
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

		// now apply the mask!
		Uint8 *pointer = &(data[0]) + tileoff;
		for(size_t y = 0; y < size; y++)
		{
			Uint8 *pixel = (Uint8*)sfc->pixels +
					size*(tile%columns) +
					size*size*columns*(tile/columns) +
					(size*columns*y);
			for(size_t x = 0; x < (size/8); x++)
			{
				Uint8 bit,b;
				for(b=0 ; b<8 ; b++)
				{
					bit = getBit(*pointer, 7-b);
					if(bit == 1)
						*pixel = 16;
					pixel++;
				}
				pointer++;
			}
		}
	}
}




bool CEGAGraphicsGalaxy::readEGAHead()
{
	// The file can be embedded in an exe file or separate on disk. Look for the disk one first!
	std::string filename;
	if (m_episode <= 6) filename =  m_path + "EGAHEAD.CK" + itoa(m_episode);
	else filename =  m_path + "KDREAMSHEAD.EGA"; // Not sure about that one
	const int ep = m_episode - 4; // index for EpisodeInfo; 0 - keen4, 1 - keen5, etc.

	std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);
	byte *p_head = nullptr;

    std::vector<char> egaHeadData;

	size_t numChunks = EpisodeInfo[ep].NumChunks;

    if(File) // File exists!
	{
        size_t egaheadlen = 0;
		File.seekg(1,std::ios::end);
        egaheadlen = File.tellg();

        // TODO: Keen 6 vs Mirror mod. Mirror mod shows as count one chunk more, please check!
        numChunks = egaheadlen/3; // 24-bit chunks
        if(egaheadlen != 0) // File not empty!
		{
            egaheadlen--;
			File.seekg(0,std::ios::beg);

			char b;
			while(!File.eof())
			{
				File.get(b);
                egaHeadData.push_back(b);
			}

            p_head = reinterpret_cast<byte*>(&egaHeadData.front());
		}
	} // no external file. Read it from the exe then
    else
	{
		byte *p_data = reinterpret_cast<byte*>(m_Exefile.getHeaderData());

		// The stuff is Huffman compressed. Use an instance for that
		unsigned long exeheaderlen = 0;
		unsigned long exeimglen = 0;

		//if(m_episode == 7) exeheaderlen = HEADERLEN_KDREAMS;
		if(!m_Exefile.readExeImageSize( p_data, &exeimglen, &exeheaderlen))
			return false;

		// Read the EGAHEAD
		p_head = p_data + exeheaderlen + EpisodeInfo[ep].OffEgaHead;
	}

	unsigned long offset = 0;
	unsigned long offset_limit;

	// For some reason, MultiMania's KDR support uses a slightly different limit
	// in offset ops. We're not in DOS, so we don't have to worry about
	// memory here >:P
	if (ep < 3) offset_limit = 0x00FFFFFF;
	else offset_limit = 0xFFFFFFFF;

    // TODO: The 4-byte offset should go outside the loop... somehow...
    for(size_t i = 0 ; i < numChunks ; i++)
	{
		if (ep != 3)
		{
            memcpy(&offset, p_head, 3); // Keen 4-6
			p_head += 3;
			offset &= offset_limit;
		}
		else
		{
            memcpy(&offset, p_head, 4); // KeenDreams
			p_head += 4;
		}
		m_egahead.push_back(offset);
	}

	return true;
}




/*void dumpData(const std::string &dumpfile, byte *in, const uint inlen)
{
    std::ofstream ofile( dumpfile.c_str() );
    ofile.write( reinterpret_cast<char*>(in), inlen );
}*/


std::vector<unsigned long> CEGAGraphicsGalaxy::readOutLenVec(const int ep,
                                                             const std::vector<unsigned char> &compEgaGraphData)
{
    unsigned long offset = 0;
    unsigned long offset_limit;

    // For some reason, MultiMania's KDR support uses a slightly different limit
    // in offset ops. We're not in DOS, so we don't have to worry about
    // memory here >:P
    if (ep < 3) offset_limit = 0x00FFFFFF;
    else offset_limit = 0xFFFFFFFF;


    std::vector<unsigned long> outLenVec;

    std::vector<unsigned long>::iterator offPtr = m_egahead.begin();

    for(size_t i = 0 ; offPtr != m_egahead.end() ; offPtr++, i++)
    {
        // Show that something is happening
        offset = *offPtr;

        unsigned long outlen = 0;

        // Make sure the chunk is valid
        if(offset < offset_limit && offset + 4 <= compEgaGraphData.size())
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
                memcpy(&outlen, &compEgaGraphData[offset], 4);
                offset += 4;
            }

        }

        outLenVec.push_back(outlen);
    }

    return outLenVec;
}


/**
 * \brief	prepares to load the data. Does a bit of extraction
 * \return 	true, if loading was successful, otherwise false
 */
bool CEGAGraphicsGalaxy::begin()
{
	// The stuff is Huffman compressed. Use an instance for that
	CHuffman Huffman;
	unsigned long exeheaderlen = 0;
	unsigned long exeimglen = 0;
	assert(m_episode >= 4);
	int ep = m_episode - 4; // index for EpisodeInfo; 0 - keen4, 1 - keen5, etc

	byte *p_data = reinterpret_cast<byte*>(m_Exefile.getHeaderData());

	//if(m_episode == 7) exeheaderlen = HEADERLEN_KDREAMS;
	if(!m_Exefile.readExeImageSize( p_data, &exeimglen, &exeheaderlen))
		return false;

	std::string filename;

	// We need the EGADICT. Read it to our structure of Huffman, he needs it!
	// Try to read it either from a file

    if(!gKeenFiles.egadictFilename.empty())
        filename =  m_path + gKeenFiles.egadictFilename;

	if( Huffman.readDictionaryFromFile(filename) )
	{
		gLogging.textOut("EGADICT was read from external file");
	}
	else
	{
		Huffman.readDictionaryNumberfromEnd( m_Exefile ); // or from the embedded Exe file
	}

	// Now we go for EGAHEAD
	if(!readEGAHead())
	{
		gLogging.textOut("Error! Couldn't read EGAHEAD from this game!");
		return false;
	}

	// Now read the EGAGRAPH
	if (m_episode <= 6) filename =  m_path + "EGAGRAPH.CK" + itoa(m_episode);
	else filename =  m_path + "KDREAMS.EGA";

	std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);

	if(!File)
	{
		gLogging.textOut(RED,"Error the file \"" + filename + "\" is missing or can't be read!");
		return false;
	}

	size_t egagraphlen = 0;
	File.seekg(1,std::ios::end);
	egagraphlen = File.tellg();
	if(egagraphlen == 0)
	{
		gLogging.textOut(RED,"Error the file \"" + filename + "\" is empty!");
		return false;
	}
	egagraphlen--;
	File.seekg(0,std::ios::beg);

	std::vector<unsigned char> CompEgaGraphData(egagraphlen);
	File.read((char*)&CompEgaGraphData[0], egagraphlen);

	// Make a clean memory pattern
	ChunkStruct ChunkTemplate;
	ChunkTemplate.len=0;
	m_egagraph.assign(m_egahead.size(), ChunkTemplate);

	unsigned long inlen = 0, outlen = 0;

	unsigned long offset = 0;
	unsigned long offset_limit;

	// For some reason, MultiMania's KDR support uses a slightly different limit
	// in offset ops. We're not in DOS, so we don't have to worry about
	// memory here >:P
	if (ep < 3) offset_limit = 0x00FFFFFF;
	else offset_limit = 0xFFFFFFFF;


    std::vector<unsigned long> outLenVec = readOutLenVec(ep, CompEgaGraphData);


	// Now lets decompress the graphics
	auto offPtr = m_egahead.begin();
    for(size_t i = 0 ; offPtr != m_egahead.end() ; offPtr++, i++)
	{
		// Show that something is happening
		offset = *offPtr;

        outlen = outLenVec[i];

        /*if(outlen == 0)
            continue;*/

		// Make sure the chunk is valid
		if(offset < offset_limit && offset + 4 <= CompEgaGraphData.size())
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
                memcpy(&outlen, &CompEgaGraphData[offset], 4);
                offset += 4;
            }


			// Allocate memory and decompress the chunk
			m_egagraph[i].len = outlen;
			m_egagraph[i].data.assign(outlen, 0);

			inlen = 0;
			// Find out the input length
			size_t j;

			auto secondOffPtr = offPtr;
			secondOffPtr++;
			for( j = i + 1; secondOffPtr != m_egahead.end() ; secondOffPtr++, j++ )
			{
			    const unsigned long second = *secondOffPtr;
			    if(second != offset_limit)
			    {
                    inlen = second - offset;
                    break;
			    }
			}

			if( secondOffPtr == m_egahead.end() )
				inlen = egagraphlen - offset;

			byte *in = &CompEgaGraphData[offset];
			byte *out = &m_egagraph[i].data[0];

			Huffman.expand(in, out, inlen, outlen);

            //dumpData("indump.dat", in, inlen);
            //dumpData("outdump.dat", out, outlen);

			//printf("%d %d\n", *out, *in);

			//m_egagraph[i].len = inlen;
			//m_egagraph[i].data.assign(inlen, 0);
			//memcpy(&m_egagraph[i].data[0], &CompEgaGraphData[offset], inlen);
		}
		else
		{
			m_egagraph[i].len = 0;
		}
	}

	File.close();
	return true;
}

/**
 * \brief	This function gets the bit of an unsigned char variable at certain position
 * \param	data		variable where the bit is to be sent.
 * \param	leftshift	sets the position of the bit the function has to retrieve
 * \return 	returns 1, if the bit at position is one, else 0
 */
Uint8 CEGAGraphicsGalaxy::getBit(unsigned char data, Uint8 leftshift)
{
	Uint8 value;

	value = data & (1<<leftshift);
	value >>= leftshift;

	return value;
}

/**
 * \brief	Read the fonts to the Gfx-Engine
 * \return 	returns true, if the fonts were read successfully, else false
 */
bool CEGAGraphicsGalaxy::readfonts()
{
	int bw, y, x;

	int ep = m_episode - 4;
	SDL_Color *Palette = gGraphics.Palette.m_Palette;

	gGraphics.createEmptyFontmaps(EpisodeInfo[ep].NumFonts+1);

	for(Uint16 i = 0; i < EpisodeInfo[ep].NumFonts; i++)
	{
		GsFont &Font = gGraphics.getFont(i);
		//Font.setMonochrome(true);

		if(m_egagraph.at(EpisodeInfo[ep].IndexFonts + i).data.at(0))
		{
			// ARM processor requires all ints and structs to be 4-byte aligned, so we're just using memcpy()
			FontHeadStruct FontHeadData, *FontHead = &FontHeadData;
			memcpy( FontHead, &(m_egagraph.at(EpisodeInfo[ep].IndexFonts + i).data.at(0)), sizeof(FontHeadStruct) );

			// Find out the maximum character width
			int maxwidth=0;
			for(Uint16 j = 0; j < 256; j++)
			{
				Font.setWidthToCharacter(FontHead->Width[j],j);
				if(FontHead->Width[j] > maxwidth)
					maxwidth = FontHead->Width[j];
			}

			Font.CreateSurface(Palette, gVideoDriver.getScrollSurface()->flags, maxwidth*16, FontHead->Height * 16);

			SDL_Surface* sfc = Font.getSDLSurface();

			SDL_FillRect(sfc, NULL, 0x8);

			if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
			Uint8* pixel = (Uint8*) sfc->pixels;

			unsigned char *pointer = &(m_egagraph[EpisodeInfo[ep].IndexFonts + i].data.at(0));

			if(!m_egagraph.at(EpisodeInfo[ep].IndexFonts + i).data.empty())
			{
				// Decode the font data
				for(int j = 0; j < 256; j++)
				{
					// Get the width of the character in bytes
					bw = (FontHead->Width[j] + 7) / 8;

					Uint8 *pixelpos;

					if(FontHead->Width[j] > 0)
					{
						SDL_Rect rect;

						rect.x = (j%16)*maxwidth;
						rect.y = (j/16)*FontHead->Height;
						rect.w = FontHead->Width[j];
						rect.h = FontHead->Height;

						for( y = 0 ; y < rect.h ; y++ )
						{
							pixelpos = pixel + (rect.y+y)*sfc->pitch+rect.x;
							for( x = 0 ; x < rect.w ; x++ )
							{
								Uint8 color = getBit(*(pointer + FontHead->Offset[j] + (y*bw) + x/8 ), 7-(x%8) )*0xF;
								if(color == 0x0) // Put a mask on black colors in font always
									color = COLORKEY;
								pixelpos[x] = color;
							}
						}
					}
				}
			}
			SDL_UnlockSurface(sfc);
		}
	}

	return true;
}

/**
 * \brief	This one extracts the bitmaps used in Keen 4-6 (Maybe Dreams in future)
 * \return 	returns true, if the fonts were read successfully, else false
 */
bool CEGAGraphicsGalaxy::readBitmaps()
{
	int ep = m_episode - 4;
	// ARM processor requires all ints and structs to be 4-byte aligned, so we're just using memcpy()
	BitmapHeadStruct BmpHead[EpisodeInfo[ep].NumBitmaps];
	memcpy( BmpHead, &(m_egagraph.at(0).data.at(0)), EpisodeInfo[ep].NumBitmaps*sizeof(BitmapHeadStruct));
	SDL_Color *Palette = gGraphics.Palette.m_Palette;

	gGraphics.createEmptyBitmaps(EpisodeInfo[ep].NumBitmaps);

	SDL_Rect bmpRect;
	bmpRect.x = bmpRect.y = 0;

	for(size_t i = 0; i < EpisodeInfo[ep].NumBitmaps; i++)
	{
        GsBitmap &Bitmap = gGraphics.getBitmapFromId(i);
		bmpRect.w = BmpHead[i].Width*8;
		bmpRect.h = BmpHead[i].Height;
		Bitmap.createSurface(gVideoDriver.getScrollSurface()->flags, bmpRect, Palette);

		extractPicture(Bitmap.getSDLSurface(),
				m_egagraph.at(EpisodeInfo[ep].IndexBitmaps + i).data,
				BmpHead[i].Width, BmpHead[i].Height);


		Bitmap.setName(m_BitmapNameMap[ep][i]);
	}

	return true;
}

bool CEGAGraphicsGalaxy::readMaskedBitmaps()
{
	int ep = m_episode - 4;
	// ARM processor requires all ints and structs to be 4-byte aligned, so we're just using memcpy()
	BitmapHeadStruct BmpMaskedHead[EpisodeInfo[ep].NumMaskedBitmaps];
	memcpy( BmpMaskedHead, &(m_egagraph.at(1).data.at(0)), EpisodeInfo[ep].NumMaskedBitmaps*sizeof(BitmapHeadStruct) );
	SDL_Color *Palette = gGraphics.Palette.m_Palette;

	gGraphics.createEmptyMaskedBitmaps(EpisodeInfo[ep].NumMaskedBitmaps);
    gGraphics.createEmptyMisGsBitmaps(2);

	SDL_Rect bmpRect;
	bmpRect.x = bmpRect.y = 0;

	for(size_t i = 0; i < EpisodeInfo[ep].NumMaskedBitmaps; i++)
	{
		GsBitmap &Bitmap = gGraphics.getMaskedBitmap(i);
		bmpRect.w = BmpMaskedHead[i].Width*8;
		bmpRect.h = BmpMaskedHead[i].Height;

		Bitmap.createSurface(gVideoDriver.getScrollSurface()->flags, bmpRect, Palette);

		extractPicture(Bitmap.getSDLSurface(),
				m_egagraph.at(EpisodeInfo[ep].IndexMaskedBitmaps + i).data,
				BmpMaskedHead[i].Width, BmpMaskedHead[i].Height, true);
	}
	return true;
}

bool CEGAGraphicsGalaxy::readTilemaps( const size_t NumTiles, size_t pbasetilesize,
										size_t rowlength, size_t IndexOfTiles,
										GsTilemap &Tilemap, bool tileoff)
{
	Tilemap.CreateSurface( gGraphics.Palette.m_Palette, SDL_SWSURFACE,
							NumTiles, pbasetilesize, rowlength );
	SDL_Surface *sfc = Tilemap.getSDLSurface();
	SDL_FillRect(sfc,NULL, 0);
	if(SDL_MUSTLOCK(sfc))	SDL_LockSurface(sfc);

	for(size_t i = 0; i < NumTiles; i++)
	{
		extractTile(sfc, m_egagraph.at(IndexOfTiles + (tileoff ? 0 : i)).data, (1<<pbasetilesize), rowlength, i, tileoff);
	}

    SDL_UnlockSurface(sfc);

    /// Let's see if there is a high colour tilemap we can load instead
    if(pbasetilesize == 4) // Only valid for the 16x16 tiles tilemap!
    {
        Tilemap.loadHiresTile("gfx/4TIL0000", m_path);
    }

	return true;
}

bool CEGAGraphicsGalaxy::readMaskedTilemaps( size_t NumTiles, size_t pbasetilesize,
											size_t rowlength, size_t IndexOfTiles,
											GsTilemap &Tilemap, bool tileoff)
{
	Tilemap.CreateSurface( gGraphics.Palette.m_Palette, SDL_SWSURFACE,
							NumTiles, pbasetilesize, rowlength );
	SDL_Surface *sfc = Tilemap.getSDLSurface();
	SDL_FillRect(sfc,NULL, 0);
	if(SDL_MUSTLOCK(sfc))	SDL_LockSurface(sfc);

	for(size_t i = 0; i < NumTiles; i++)
	{
		extractMaskedTile(sfc, m_egagraph.at(IndexOfTiles + (tileoff ? 0 : i)).data, (1<<pbasetilesize), rowlength, i, tileoff);
	}

	SDL_UnlockSurface(sfc);

    /// Let's see if there is a high colour tilemap we can load instead
    if(pbasetilesize == 4) // Only valid for the 16x16 tiles tilemap!
    {
        // Looking for high color pictures
        if( Tilemap.loadHiresTile("gfx/4TIL0001", m_path) )
        {
            Tilemap.applyGalaxyMask();
        }
    }


	return true;
}






bool CEGAGraphicsGalaxy::readSprites( size_t NumSprites, size_t IndexSprite )
{
	// Create all the sprites
    gGraphics.createEmptySprites(4, NumSprites);

	int ep = m_episode - 4;

	// ARM processor requires all ints and structs to be 4-byte aligned, so we're just using memcpy()
    std::vector<SpriteHeadStruct> sprHead(NumSprites, SpriteHeadStruct());
    memcpy( sprHead.data(), &(m_egagraph.at(2).data.at(0)), NumSprites*sizeof(SpriteHeadStruct) );

	for(size_t i = 0; i < NumSprites; i++)
	{
        SpriteHeadStruct Head = sprHead[i];
		std::vector<unsigned char> &data = m_egagraph.at(IndexSprite + i).data;

        GsSprite &Sprite = gGraphics.getSprite(0,i);
		Sprite.setSize( Head.Width*8, Head.Height );

        Sprite.setOffset( Head.OrgX>>(TILE_S), Head.OrgY>>(TILE_S) );

		// Setup the collision information
		int boxX1 = ((Head.Rx1) << (STC-TILE_S));
		int boxY1 = ((Head.Ry1) << (STC-TILE_S));
		int boxX2 = ((Head.Rx2) << (STC-TILE_S));
		int boxY2 = ((Head.Ry2) << (STC-TILE_S));

		if(boxX2-boxX1 >= 1<<STC)
		{
		   boxX2 --;
		   boxX1 += (1<<STC);
		}

		if(boxY2-boxY1 >= 1<<STC)
		{
		   boxY2 --;
		   boxY1 += (1<<STC);
		}

		Sprite.setBoundingBoxCoordinates( boxX1, boxY1, boxX2, boxY2 );

		Sprite.createSurface( gVideoDriver.mpVideoEngine->getBlitSurface()->flags,
				gGraphics.Palette.m_Palette );

		SDL_Surface *sfc = Sprite.getSDLSurface();
		SDL_FillRect(sfc,NULL, 0);
		if(SDL_MUSTLOCK(sfc))	SDL_LockSurface(sfc);

		if(!data.empty())
		{
			// Decode the image data
			for(size_t p = 0; p < 4; p++)
			{
				// Decode the lines of the bitmap data
				Uint8 *pointer = &(data[0]) + (p+1) * Head.Width * Head.Height;
				for(size_t y = 0; y < Head.Height; y++)
				{
					Uint8 *pixel = (Uint8*)sfc->pixels +
							(Head.Width * 8 *y);
					for(size_t x = 0; x < Head.Width; x++)
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

			// now apply the mask!
			Uint8 *pointer = &(data[0]);
			for(size_t y = 0; y < Head.Height; y++)
			{
				Uint8 *pixel = (Uint8*)sfc->pixels +
						(Head.Width * 8*y);
				for(size_t x = 0; x < Head.Width; x++)
				{
					Uint8 bit,b;
					for(b=0 ; b<8 ; b++)
					{
						bit = getBit(*pointer, 7-b);
						if(bit == 1)
							*pixel = 16;
						pixel++;
					}
					pointer++;
				}
			}
		}
		SDL_UnlockSurface(sfc);

		Sprite.setName(m_SpriteNameMap[ep][i]);
	}

    // Now let's copy all the sprites ant tint to the proper colors

    auto &SpriteOrigVec = gGraphics.getSpriteVec(0);

    for( unsigned int i=1 ; i<4 ; i++ )
    {
        gGraphics.getSpriteVec(i) = SpriteOrigVec;
    }

    // For the other variant let's exchange some colors
    auto &SpriteVecPlayer2 = gGraphics.getSpriteVec(1);
    for( auto &sprite : SpriteVecPlayer2)
    {
        // Red against Purple
        sprite.exchangeSpriteColor( 5, 4, 0 );
        sprite.exchangeSpriteColor( 13, 12, 0 );

        // Yellow against Green
        sprite.exchangeSpriteColor( 2, 6, 0 );
        sprite.exchangeSpriteColor( 10, 14, 0 );
        sprite.optimizeSurface();
    }

    auto &SpriteVecPlayer3 = gGraphics.getSpriteVec(2);
    for( auto &sprite : SpriteVecPlayer3)
    {
        // Red against Green
        sprite.exchangeSpriteColor( 2, 4, 0 );
        sprite.exchangeSpriteColor( 10, 12, 0 );

        // Yellow against Purple
        sprite.exchangeSpriteColor( 5, 6, 0 );
        sprite.exchangeSpriteColor( 13, 14, 0 );
        sprite.optimizeSurface();
    }


    auto &SpriteVecPlayer4 = gGraphics.getSpriteVec(3);
    for( auto &sprite : SpriteVecPlayer4)
    {
        // Red against Yellow
        sprite.exchangeSpriteColor( 6, 4, 0 );
        sprite.exchangeSpriteColor( 14, 12, 0 );

        // Green against Purple
        sprite.exchangeSpriteColor( 2, 5, 0 );
        sprite.exchangeSpriteColor( 10, 13, 0 );
        sprite.optimizeSurface();
    }

    for(auto &sprite : gGraphics.getSpriteVec(0))
    {
        sprite.optimizeSurface();
    }

	return true;
}

/**
 * This reads the Texts used in Computer Wrist and Ending as Beginning Pages. In-Level texts are not handled here
 */
bool CEGAGraphicsGalaxy::readTexts()
{
	// g_pBehaviorEngine->addMessage(name, message);
	 /*char filename[PATH_MAX];
	FILE *f;
	int i;
	int ep = Switches->Episode - 4;

	if(!ExportInitialised)
		quit("Trying to export texts before initialisation!");

	// Export all the texts
	printf("Exporting texts: ");

	for(i = 0; i < EpisodeInfo[ep].NumTexts; i++)
	{
		// Show that something is happening
		showprogress((i * 100) / EpisodeInfo[ep].NumTexts);

		if(EgaGraph[EpisodeInfo[ep].IndexTexts + i].data)
		{
			// Create the text file
			sprintf(filename, "%s/%cTXT%04d.txt", Switches->OutputPath, '0' + Switches->Episode, i);
			f = openfile(filename, "wb", Switches->Backup);
			if(!f)
				quit("Can't open text file %s!", filename);
			fwrite(EgaGraph[EpisodeInfo[ep].IndexTexts + i].data, EgaGraph[EpisodeInfo[ep].IndexTexts + i].len, 1, f);
			fclose(f);
		}
		//printf("\x8\x8\x8\x8");
	}
	completemsg();*/
	 return true;
}




bool CEGAGraphicsGalaxy::readMiscStuff()
{
    int width = 0; int height = 0;
    SDL_Color *Palette = gGraphics.Palette.m_Palette;

    // Only position 1 and 2 are read. This will the terminator text.
    // Those are monochrom...

    for(int misc = 1 ; misc<3 ; misc++)
    {
        const int index = EpisodeInfo[m_episode-4].IndexMisc + misc;
        Uint16 *dataPtr;

        memcpy( &dataPtr, &(m_egagraph.at(index).data), sizeof(Uint16 *) );

        memcpy(&height, dataPtr, sizeof(Uint16) );
        dataPtr++;
        memcpy(&width, dataPtr, sizeof(Uint16) );
        dataPtr++;

        SDL_Rect bmpRect;

        GsBitmap &Bitmap = gGraphics.getMisGsBitmap(misc-1);
        bmpRect.w = width;
        bmpRect.h = height;

        Uint16 *rlepointer = dataPtr;
        rlepointer += height;

        Bitmap.createSurface(0, bmpRect, Palette);

        SDL_Surface *bmp = Bitmap.getSDLSurface();

        SDL_LockSurface(bmp);

        Uint8 *sfcPtr =  static_cast<Uint8*>(bmp->pixels);

        const int bytePerPixel = bmp->format->BytesPerPixel;

        Uint32 textColor;

        switch(m_episode) // The color of the terminator depends on the episode.
        {
        case 6:  textColor = SDL_MapRGB(bmp->format, 0xff,0x55,0xff); break;
        case 5:  textColor = SDL_MapRGB(bmp->format, 0xff,0x55,0x55); break;
        default: textColor = SDL_MapRGB(bmp->format, 0xaa,0xaa,0xaa); break;
        }


        const Uint32 blackColor = SDL_MapRGB(bmp->format, 0,0,0);

        Uint32 currentColor = blackColor;

        int amountOfPixels = 0;

        for(int line=0 ; line < height ; line++)
        {
            Uint16 pixelCount = *rlepointer;

            while( pixelCount != 0xFFFF ) // End-Flag
            {
                for(int i=0 ; i<pixelCount ; i++)
                {
                    *sfcPtr = currentColor;
                    sfcPtr += bytePerPixel;
                    amountOfPixels++;
                }

                currentColor =
                        (currentColor == blackColor) ?
                            textColor : blackColor;

                rlepointer++;
                pixelCount = *rlepointer;
            }

            currentColor =
                    (currentColor == blackColor) ?
                        textColor : blackColor;

            rlepointer++;
        }

        if(amountOfPixels != height*width)
        {
            gLogging.ftextOut("Warning! Someting is wrong with the amount of read pixels in MisGsBitmap %d.\n", misc);
        }

        SDL_UnlockSurface(bmp);
    }


    return true;
}


}
