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
#include "graphics/GsGametext.h"
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
#include <string>
#include <SDL.h>


namespace galaxy
{

// The ExeImageSize fields are values from the real start of exe image,
// i.e. KEEN4E.EXE + 0x2E00 to EndOfFile=0x3D740
// It seems that the standard shareware KDREAMS.EXE starts at 0x1C00

static EpisodeInfoStruct EpisodeInfo[] =
{
    {   /* Episode 4 */
        0x3D740,    /* ExeImageSize */
        0x21080,    /* OffEgaHead */
        0x358F6,    /* OffEgaDict */
        4751,       /* NumChunks */
        3, 3,       /* NumFonts, IndexFonts */
        115, 6,     /* NumBitmaps, IndexBitmaps */
        3, 121,     /* NumMaskedBitmaps, IndexMasked Bitmaps */
        397, 124,   /* NumSprites, IndexSprites */
        104, 521,   /* Num8Tiles, Index8Tiles */
        20, 522,    /* Num8MaskedTiles, Index8MaskedTiles */
        1296, 523,  /* Num16Tiles, Index16Tiles */
        2916, 1819, /* Num16MaskedTiles, Index16MaskedTiles */
        4, 4735,    /* NumMisc, IndexMisc */
        7, 4739,    /* NumTexts, IndexTexts */
        5, 4746     /* NumDemos, IndexDemos */
    },
    {   /* Episode 5 */
        0x3E370,    /* ExeImageSize */
        0x21FC0,    /* OffEgaHead */
        0x362C4,    /* OffEgaDict */
        4931,       /* NumChunks */
        3, 3,       /* NumFonts, IndexFonts */
        93, 6,      /* NumBitmaps, IndexBitmaps */
        3, 99,      /* NumMaskedBitmaps, IndexMasked Bitmaps */
        346, 102,   /* NumSprites, IndexSprites */
        104, 448,   /* Num8Tiles, Index8Tiles */
        20, 449,    /* Num8MaskedTiles, Index8MaskedTiles */
        1512, 450,  /* Num16Tiles, Index16Tiles */
        2952, 1962, /* Num16MaskedTiles, Index16MaskedTiles */
        5, 4921,    /* NumMisc, IndexMisc */
        7, 4914,    /* NumTexts, IndexTexts */
        5, 4926     /* NumDemos, IndexDemos */
    },
    {   /* Episode 6 */
        0x3F950,    /* ExeImageSize */
        0x20F50,    /* OffEgaHead */
        0x372EE,    /* OffEgaDict */
        5560,       /* NumChunks */
        3, 3,       /* NumFonts, IndexFonts */
        37, 6,      /* NumBitmaps, IndexBitmaps */
        3, 43,      /* NumMaskedBitmaps, IndexMasked Bitmaps */
        390, 46,    /* NumSprites, IndexSprites */
        104, 436,   /* Num8Tiles, Index8Tiles */
        12, 437,    /* Num8MaskedTiles, Index8MaskedTiles */
        2376, 438,  /* Num16Tiles, Index16Tiles */
        2736, 2814, /* Num16MaskedTiles, Index16MaskedTiles */
        4, 5551,    /* NumMisc, IndexMisc */
        1, 5550,    /* NumTexts, IndexTexts */
        5, 5555     /* NumDemos, IndexDemos */
    },
    {   /* Episode 7 (Dreams) */
        // The following ExeImageSize = KDREAMS.EXE (213536) - 0x1C00:
        206368,     /* ExeImageSize */
        0x1CB20,    /* OffEgaHead */
        0x29F12,    /* OffEgaDict */
        3019,       /* NumChunks */
        0, 0,       /* NumFonts, IndexFonts */
        0, 0,       /* NumBitmaps, IndexBitmaps */
        0, 0,       /* NumMaskedBitmaps, IndexMasked Bitmaps */
        0, 0,       /* NumSprites, IndexSprites */
        1, 368,     /* Num8Tiles, Index8Tiles */
        1, 369,     /* Num8MaskedTiles, Index8MaskedTiles */
        1440, 370,  /* Num16Tiles, Index16Tiles */
        1224,1810 , /* Num16MaskedTiles, Index16MaskedTiles */
        0, 0,       /* NumMisc, IndexMisc */
        0, 0,       /* NumTexts, IndexTexts */
        0, 0        /* NumDemos, IndexDemos */
    },
    {   /* Episode 6 (demo) */
        // Size of datafile.dump.k6demo (exefile raw data)
        0x37650,    /* ExeImageSize */
        // Offset in exefile raw data of 000000 chunk before contiguous
        // run of chunk offsets. (Which should be preceded by two other
        // 000000 "chunks".)
        0x1c230,    /* OffEgaHead k6demo */
        // Look for fd 01 00 00 00 00, + 6 - 1024 to get start of egadict.
        0x2fa40,    /* OffEgaDict */
        // Inspect egahead. This excludes last chunk which is offset to end of data file.
        4365,       /* NumChunks */
        // Fonts has the same sizes as k4, so looks good.
         3, 3,       /* NumFonts, IndexFonts */
        // Sum of NumBitmaps and IndexBitmaps should give IndexMasked Bitmaps.
        // NumBitmaps should match outlen for i=0 (chunk 0).
        32, 6,      /* NumBitmaps, IndexBitmaps */
        // Masked bitmaps starts with size=10920 (jump) like k4,
        // so needs to have index of 38.
        // The sizes match k4, so NumMaskedBitmaps looks good as is at 3.
        // NumMaskedBitmaps should match outlen for i=1 (chunk 1).
        3, 38,      /* NumMaskedBitmaps, IndexMasked Bitmaps */
        // The index is the sum of the previous num and index.
        281, 41,    /* NumSprites, IndexSprites */
        // Index8Tiles is the sum of NumSprites and IndexSprites.
        104, 322,   /* Num8Tiles, Index8Tiles */
        12, 323,    /* Num8MaskedTiles, Index8MaskedTiles */
        // 16Tiles have size of 128 or 0.
        1296, 324,  /* Num16Tiles, Index16Tiles */
        // last 16 tile seems to be at 1616
        // 16MaskedTiles have size of 160 or 0.
        2736, 1620, /* Num16MaskedTiles, Index16MaskedTiles */
        // Misc have size of ~4008-15044
        // The index is the sum of NumTexts and IndexTexts.
        4, 4357,    /* NumMisc, IndexMisc */
        // Texts have size of ~313-6884
        1, 4356,    /* NumTexts, IndexTexts */
        // Demos have size ~66-194 and small first two words.
        // NumChunks - 4 gives IndexDemos.
        4, 4361     /* NumDemos, IndexDemos */
    }
};



CEGAGraphicsGalaxy::CEGAGraphicsGalaxy()
{
    createBitmapsIDs();
    gBehaviorEngine.setEpisodeInfoStructPtr(EpisodeInfo);
}

// Get the index for EpisodeInfo.
// 0 - keen4
// 1 - keen5
// 2 - keen6
// 3 - keen dreams
// 4 - keen6 demo
size_t CEGAGraphicsGalaxy::getEpisodeInfoIndex()
{
  const int  episode = gKeenFiles.exeFile.getEpisode();
  const bool isDemo    = gKeenFiles.exeFile.isDemo();
  
  if (episode == 6 && isDemo)
    {
      return 4;
    }
  
  return size_t(episode - 4);
}

bool CEGAGraphicsGalaxy::loadData()
{  
    // Set the palette, so the proper colours are loaded
    const int  episode = gKeenFiles.exeFile.getEpisode();
  
    gGraphics.Palette.setupColorPalettes(nullptr, episode);

    if(!begin())
    {
        return false;
    }

    auto &curEpInfo = EpisodeInfo[getEpisodeInfoIndex()];

    // First, retrieve the Tile properties so the tilemap gets properly formatted
    // Important especially for masks, and later in the game for the behaviours
    // of those objects
    CTileLoader TileLoader;
    if(!TileLoader.load(curEpInfo.Num16Tiles,
                        curEpInfo.Num16MaskedTiles))
    {
        return false;
    }


    if(!readTables())
    {
        return false;
    }

    if(!readfonts())
    {
        return false;
    }

    if(!readBitmaps())
    {
        return false;
    }

    if(!readMaskedBitmaps())
    {
        return false;
    }

    gGraphics.createEmptyTilemaps(4);

    if(!readTilemaps(curEpInfo.Num16Tiles, 4, 18,
                     curEpInfo.Index16Tiles,
                     gGraphics.getTileMap(0), false))
    {
        return false;
    }


    if(!readMaskedTilemaps(curEpInfo.Num16MaskedTiles, 4, 18,
                           curEpInfo.Index16MaskedTiles,
                           gGraphics.getTileMap(1), false))
    {
        return false;
    }


    if(!readTilemaps(curEpInfo.Num8Tiles, 3, 1,
                     curEpInfo.Index8Tiles,
                     gGraphics.getTileMap(2), true))
    {
        return false;
    }


    if(!readMaskedTilemaps(curEpInfo.Num8MaskedTiles, 3, 1,
                           curEpInfo.Index8MaskedTiles,
                           gGraphics.getTileMap(3), true))
    {
        return false;
    }


    if(!readSprites( curEpInfo.NumSprites,
                     curEpInfo.IndexSprites ))
    {
        return false;
    }


    if(!readTexts())
    {
        return false;
    }


    //k456_export_texts();

    if( !readMiscStuff() )
    {
        return false;
    }

    //k456_export_demos();
    //k456_export_end();

    // Now try to store a preview if possible
    // Create an intro in case it does not exist yet
    const std::string  &path = gKeenFiles.gameDir;
    std::string fullpath = getResourceFilename("preview.bmp", path, false);
    if( fullpath == "" )
    {   // Not found create it
        fullpath = JoinPaths(path, "preview.bmp");
        fullpath = GetWriteFullFileName(fullpath, true);
        GsBitmap *pBitmap = gGraphics.getBitmapFromStr(0, "TITLE");
        SDL_SaveBMP( pBitmap->getSDLSurface(), fullpath.c_str());
    }

    return true;
}


void CEGAGraphicsGalaxy::
extractPicture(SDL_Surface *sfc,
               std::vector<unsigned char> &data, size_t Width, size_t Height,
               bool masked)
{
    if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
    SDL_FillRect(sfc, nullptr, 0x0);

    if(!data.empty())
    {
        // Decode the bitmap data
        for(size_t p = 0; p < 4; p++)
        {
            auto pixel = reinterpret_cast<Uint8*>(sfc->pixels);

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
 * \brief   This function extracts a tile from the galaxy graphics map, and converts it properly to a
 *          SDL Surface
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
 * \brief   This function extracts a masked tile from the galaxy graphics map, and converts it properly to a
 *          SDL Surface
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
    
    const auto &exefile = gKeenFiles.exeFile;
    const int  episode  = exefile.getEpisode();        
    const std::string &gamedir = gKeenFiles.gameDir;
    
    if (episode <= 6) filename = JoinPaths(gamedir, "EGAHEAD.CK" + to_string(episode));
    else filename =  JoinPaths(gamedir, "KDREAMSHEAD.EGA"); // Not sure about that one
    const int ep = getEpisodeInfoIndex();

    std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);
    byte *headPtr = nullptr;

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

            headPtr = reinterpret_cast<byte*>(&egaHeadData.front());
        }
    } // no external file. Read it from the exe then
    else
    {
        byte *p_data = reinterpret_cast<byte*>(exefile.getHeaderData());

        // The stuff is Huffman compressed. Use an instance for that
        unsigned long exeheaderlen = 0;
        unsigned long exeimglen = 0;

        //if(m_episode == 7) exeheaderlen = HEADERLEN_KDREAMS;
        if(!exefile.readExeImageSize( p_data, &exeimglen, &exeheaderlen))
            return false;

        // Read the EGAHEAD
        headPtr = p_data + exeheaderlen + EpisodeInfo[ep].OffEgaHead;
    }

    unsigned long offset = 0;

    // For some reason, MultiMania's KDR support uses a slightly different limit
    // in offset ops. We're not in DOS, so we don't have to worry about
    // memory here >:P
    bool dreams = (ep == 3);
    unsigned long offset_limit = dreams ? 0xFFFFFFFF : 0x00FFFFFF;

    size_t chunkSize = dreams ? 4 : 3;
    for(size_t i = 0 ; i < numChunks ; i++)
    {
        memcpy(&offset, headPtr, chunkSize);
        headPtr += chunkSize;
        offset &= offset_limit;
        m_egahead.push_back(offset);
    }

    return true;
}



/*void dumpData(const std::string &dumpfile, byte *in, const unsigned int inlen)
{
    std::ofstream ofile( dumpfile.c_str() );
    ofile.write( reinterpret_cast<char*>(in), inlen );
}*/



std::vector<unsigned long> CEGAGraphicsGalaxy::readOutLenVec(const int ep,
                                                             const std::vector<unsigned char> &compEgaGraphData)
{
    unsigned long offset = 0;

    // For some reason, MultiMania's KDR support uses a slightly different limit
    // in offset ops. We're not in DOS, so we don't have to worry about
    // memory here >:P
    bool dreams = (ep == 3);
    unsigned long offset_limit = dreams ? 0xFFFFFFFF : 0x00FFFFFF;

    std::vector<unsigned long> outLenVec;

    std::vector<unsigned long>::iterator offPtr = m_egahead.begin();
    
    auto &info = EpisodeInfo[ep];

    for(size_t i = 0 ; offPtr != m_egahead.end() ; offPtr++, i++)
    {
        // Show that something is happening
        offset = *offPtr;

        unsigned long outlen = 0;

        // Make sure the chunk is valid
        if(offset < offset_limit && offset + 4 <= compEgaGraphData.size())
        {
            // Get the expanded length of the chunk
            if(i >= info.Index8Tiles && i < info.Index16MaskedTiles + info.Num16MaskedTiles)
            {
                // Expanded sizes of 8, 16,and 32 tiles are implicit
                if(i >= info.Index16MaskedTiles) // 16x16 tiles are one/chunk
                    outlen = 2 * 16 * 5;
                else if(i >= info.Index16Tiles)
                    outlen = 2 * 16 * 4;
                else if(i >= info.Index8MaskedTiles) // 8x8 tiles are all in one chunk!
                    outlen = info.Num8MaskedTiles * 8 * 5;
                else if(i >= info.Index8Tiles)
                    outlen = info.Num8Tiles * 8 * 4;
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
 * \brief   prepares to load the data. Does a bit of extraction
 * \return  true, if loading was successful, otherwise false
 */
bool CEGAGraphicsGalaxy::begin()
{  
    const auto &exefile = gKeenFiles.exeFile;
    const int  episode  = exefile.getEpisode();          
  
    // The stuff is Huffman compressed. Use an instance for that
    CHuffman Huffman;
    unsigned long exeheaderlen = 0;
    unsigned long exeimglen = 0;
    assert(episode >= 4);
    auto epIdx = int(getEpisodeInfoIndex());
    
    const std::string &gamedir = gKeenFiles.gameDir;
    

    byte *dataPtr = reinterpret_cast<byte*>(exefile.getHeaderData());

    if( !exefile.isPythonScript() )
    {
        //if(m_episode == 7) exeheaderlen = HEADERLEN_KDREAMS;
        if(!exefile.readExeImageSize( dataPtr, &exeimglen, &exeheaderlen))
            return false;
    }


    std::string filename;

    // We need the EGADICT. Read it to our structure of Huffman, he needs it!
    // Try to read it either from a file

    if(!gKeenFiles.egadictFilename.empty())
    {
        filename =  JoinPaths(gamedir, gKeenFiles.egadictFilename);
    }


    if( IsFileAvailable(filename) )
    {
        if( !Huffman.readDictionaryFromFile(filename) )
        {
            gLogging << "Fatal error reading EGADICT file at path: \""
                     << filename << "\".";
            return false;
        }
        else
        {
            gLogging << "EGADICT was read at path: \"" << filename << "\".";
        }
    }
    else
    {
        if(exefile.isPythonScript())
        {
            gLogging << "You need to provide a dictionary file if you "
                        "are using a python executable";
            return false;
        }
        else
        {

            if(!Huffman.readDictionaryNumberfromEnd( exefile ))
            {
                return  false;
            }
        }
    }


    // Now we go for EGAHEAD
    if(!readEGAHead())
    {
        gLogging.textOut("Error! Couldn't read EGAHEAD from this game!");
        return false;
    }

    // Now read the EGAGRAPH
    if (episode <= 6) filename = JoinPaths(gamedir, "EGAGRAPH.CK" + to_string(episode));
    else filename = JoinPaths(gamedir, "KDREAMS.EGA");

    std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);

    if(!File)
    {
        gLogging.textOut(FONTCOLORS::RED, "Error the file \"" + filename + "\" is missing or can't be read!");
        return false;
    }

    size_t egagraphlen = 0;
    File.seekg(1,std::ios::end);
    egagraphlen = File.tellg();
    if(egagraphlen == 0)
    {
        gLogging.textOut(FONTCOLORS::RED, "Error the file \"" + filename + "\" is empty!");
        return false;
    }
    egagraphlen--;
    File.seekg(0,std::ios::beg);

    std::vector<unsigned char> CompEgaGraphData(egagraphlen);
    File.read((char*)&CompEgaGraphData[0], egagraphlen);

    // Make a clean memory pattern
    ChunkStruct ChunkTemplate;
    m_egagraph.assign(m_egahead.size()+1, ChunkTemplate);

    unsigned long inlen = 0, outlen = 0;

    unsigned long offset = 0;

    // For some reason, MultiMania's KDR support uses a slightly different limit
    // in offset ops. We're not in DOS, so we don't have to worry about
    // memory here >:P
    bool dreams = (epIdx == 3);
    unsigned long offset_limit = dreams ? 0xFFFFFFFF : 0x00FFFFFF;

    std::vector<unsigned long> outLenVec = readOutLenVec(epIdx, CompEgaGraphData);

    auto dataSize = CompEgaGraphData.size();
    size_t numBadChunks = 0;
    
    auto &info = EpisodeInfo[epIdx];

    // Now lets decompress the graphics
    auto offPtr = m_egahead.begin();
    for(size_t i = 0 ; offPtr != m_egahead.end() ; offPtr++, i++)
    {
        // Show that something is happening
        offset = *offPtr;

        outlen = outLenVec[i];

        // Make sure the chunk is valid
        if(offset < offset_limit && offset + 4 <= dataSize)
        {

            // Get the expanded length of the chunk
            if(i >= info.Index8Tiles && 
	       i < info.Index16MaskedTiles + info.Num16MaskedTiles)
            {
                // Expanded sizes of 8, 16,and 32 tiles are implicit
                if(i >= info.Index16MaskedTiles) // 16x16 tiles are one/chunk
                    outlen = 2 * 16 * 5;
                else if(i >= info.Index16Tiles)
                    outlen = 2 * 16 * 4;
                else if(i >= info.Index8MaskedTiles) // 8x8 tiles are all in one chunk!
                    outlen = info.Num8MaskedTiles * 8 * 5;
                else if(i >= info.Index8Tiles)
                    outlen = info.Num8Tiles * 8 * 4;
            }
            else
            {
                memcpy(&outlen, &CompEgaGraphData[offset], 4);
                offset += 4;
            }

            // Set an arbitrary, but large, limit on outlen, to avoid blowouts
            // on bad input data.
            if(outlen > 100000)
            {
                gLogging.ftextOut("outlen too big at i=%u offset=%x outlen=%u",
                                  i, offset, outlen);
                return false;
            }

            inlen = 0;
            // Find out the input length
            size_t j;

            auto secondOffPtr = offPtr;
            secondOffPtr++;
            for( j = i + 1; secondOffPtr != m_egahead.end() ; secondOffPtr++, j++ )
            {
                const unsigned long second = *secondOffPtr;
                if(second < offset_limit)
                {
                    // Check that the second offset is valid
                    if(second > dataSize)
                    {
                        gLogging.textOut(FONTCOLORS::RED,"Error! The file \"" + filename + "\" contains a second offset that is too large!");
                    }
                    else if(second < offset)
                    {
                        gLogging.textOut(FONTCOLORS::RED,"Error! The file \"" + filename + "\" contains a second offset that is less than the first offset!");
                    }
                    else
                    {
                        inlen = second - offset;
                    }
                    break;
                }
            }

            if( secondOffPtr == m_egahead.end() )
            {
                inlen = egagraphlen - offset;
            }
            else if(inlen == 0) {
                m_egagraph[i].len = 0;
                gLogging.ftextOut("Giving up due to bad chunk at offset=%x", offset);
                ++numBadChunks;
                break;
            }

            // Allocate memory and decompress the chunk
            m_egagraph[i].len = outlen;
            m_egagraph[i].data.assign(outlen, 0);

            byte *in = &CompEgaGraphData[offset];
            byte *out = &m_egagraph[i].data[0];

            Huffman.expand(in, out, inlen, outlen);
        }
        else
        {
            m_egagraph[i].len = 0;
            if (offset != offset_limit)
            {
                gLogging.ftextOut("Skipping chunk with bad offset=%x\n", offset);
                ++numBadChunks;
            }
        }
    }

    gLogging << "Found a total of " << numBadChunks << " bad offsets\n.";

    File.close();
    return true;
}

/**
 * \brief   This function gets the bit of an unsigned char variable at certain position
 * \param   data        variable where the bit is to be sent.
 * \param   leftshift   sets the position of the bit the function has to retrieve
 * \return  returns 1, if the bit at position is one, else 0
 */
Uint8 CEGAGraphicsGalaxy::getBit(unsigned char data, Uint8 leftshift)
{
    Uint8 value;

    value = data & (1<<leftshift);
    value >>= leftshift;

    return value;
}

/**
 * \brief   Read the tables for pictures (bitmaps), masked pictures (bitmaps), and sprites.
 * \return  returns true, if the tables were read successfully, else false
 */
bool CEGAGraphicsGalaxy::readTables()
{
    const auto epIdx = int(getEpisodeInfoIndex());
    const std::vector<unsigned char> &bitmapTable = m_egagraph.at(0).data;
    
    auto &info = EpisodeInfo[epIdx];

    if(bitmapTable.size() != (info.NumBitmaps * 4))
    {
        gLogging.ftextOut("bad bitmap table size=%u vs NumBitmaps=%u (x4)",
                          bitmapTable.size(), info.NumBitmaps);
    }

    const std::vector<unsigned char> &maskedBitmapTable = m_egagraph.at(1).data;

    if(maskedBitmapTable.size() != (info.NumMaskedBitmaps * 4))
    {
        gLogging.ftextOut("bad masked bitmap table size=%u vs NumBitmaps=%u (x4)",
                          maskedBitmapTable.size(), info.NumMaskedBitmaps);
    }

    const std::vector<unsigned char> &spriteTable = m_egagraph.at(2).data;

    if(spriteTable.size() != (info.NumSprites * 18))
    {
        gLogging.ftextOut("bad sprite table size=%u vs NumSprites=%u (x18)",
                          spriteTable.size(), info.NumSprites);
    }

    // gLogging.ftextOut("sprite table size=%u vs NumSprites=%u",
    //                   spriteTable.size(), EpisodeInfo[ep].NumSprites);

    // for(size_t i = 0; i < spriteTable.size(); ++i)
    // {
    //     gLogging.ftextOut("sprite table i=%u entry=%x", i, spriteTable[i]);
    // }

    return true;
}


/**
 * \brief   Read the fonts to the Gfx-Engine
 * \return  returns true, if the fonts were read successfully, else false
 */
bool CEGAGraphicsGalaxy::readfonts()
{
    int bw, y, x;

    const int ep = getEpisodeInfoIndex();
    SDL_Color *Palette = gGraphics.Palette.m_Palette;

    gGraphics.createEmptyFontmaps(EpisodeInfo[ep].NumFonts+1);

    for(Uint16 i = 0; i < EpisodeInfo[ep].NumFonts; i++)
    {
        GsFont &font = gGraphics.getFont(i);

        const std::vector<unsigned char> &fontData = m_egagraph.at(EpisodeInfo[ep].IndexFonts + i).data;

        if(fontData.at(0))
        {
            if(fontData.size() < sizeof(FontHeadStruct))
            {
                gLogging.ftextOut("bad font head source data size i=%u size=%u", i, fontData.size());
                return false;
            }

            // ARM processor requires all ints and structs to be 4-byte aligned, so we're just using memcpy()
            FontHeadStruct FontHeadData, *FontHead = &FontHeadData;
            memcpy( FontHead, &(fontData.at(0)), sizeof(FontHeadStruct) );

            if(FontHead->Height == 0)
            {
                gLogging.ftextOut("bad font head height i=%u Height=%u", i, FontHead->Height);
                return false;
            }

            // Find out the maximum character width
            int maxwidth=0;
            for(Uint16 j = 0; j < 256; j++)
            {
                if(FontHead->Width[j] > maxwidth)
                    maxwidth = FontHead->Width[j];
            }

            font.CreateSurface(Palette, gVideoDriver.getScrollSurface()->flags, maxwidth*16, FontHead->Height * 16);

            auto sfc = font.SDLSurfacePtr();

            SDL_FillRect(sfc, NULL, 0x8);

            if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
            Uint8* pixel = (Uint8*) sfc->pixels;

            const std::vector<unsigned char> &data = m_egagraph[EpisodeInfo[ep].IndexFonts + i].data;
            const unsigned char * const pointer = &(data.at(0));
            const unsigned char * const pointerEnd = pointer + data.size();

            auto createfontMap = [&](const int from, const int numChars, const int startOff)
            {
                if(!data.empty())
                {
                    // Decode the font data
                    for(int j = from; j < from+numChars; j++)
                    {
                        font.setWidthToCharacter(FontHead->Width[j], j+startOff);

                        // Get the width of the character in bytes
                        bw = (FontHead->Width[j] + 7) / 8;

                        Uint8 *pixelpos;

                        if(FontHead->Width[j] > 0)
                        {
                            SDL_Rect rect;

                            rect.w = FontHead->Width[j];
                            rect.h = FontHead->Height;

                            // Check that data size is enough for given offset, etc.
                            if(FontHead->Offset[j] + (((rect.h - 1) * bw) + ((rect.w - 1) / 8)) > (pointerEnd - pointer))
                            {
                                gLogging.ftextOut("bad font head width or height i=%u j=%d w=%d h=%d", i, j, rect.w, rect.h);
                                return false;
                            }

                            const unsigned char * const offsetPointer = pointer + FontHead->Offset[j];

                            rect.x = ((j+startOff)%16)*maxwidth;
                            rect.y = ((j+startOff)/16)*FontHead->Height;

                            for( y = 0 ; y < rect.h ; y++ )
                            {
                                pixelpos = pixel + (rect.y+y)*sfc->pitch+rect.x;
                                for( x = 0 ; x < rect.w ; x++ )
                                {
                                    Uint8 color = getBit(*(offsetPointer + (y*bw) + x/8 ), 7-(x%8) )*0xF;
                                    if(color == 0x0) // Put a mask on black colors in font always
                                        color = COLORKEY;
                                    pixelpos[x] = color;
                                }
                            }
                        }
                    }
                }
                return true;
            };

            // The first two fonts have an ordered which fits quite well to the ascii tables.
            // The third one is much different so we just patch it differently
            bool success;
            if(i < 2)
            {
                success = createfontMap(0, 256, '\0');
            }
            else
            {
                success =
                    // Capital letters // 'A' == 65 in ASCII
                    createfontMap(32, 26, 33) &&

                    // Lower case letters
                    createfontMap(58, 26, 39) &&

                    // Special characters
                    createfontMap(84, 1, 46-84) &&  // '.' == 46 in ASCII 84 in Plane 2
                    createfontMap(85, 1, 44-85) &&  // ',' == 44 in ASCII
                    createfontMap(86, 1, 45-86) &&  // '-' == 45 in ASCII
                    createfontMap(87, 1, 34-87) &&  // '"' == 34 in ASCII
                    createfontMap(88, 1, 32-88) &&  // ' ' == 32 in ASCII
                    createfontMap(89, 1, 33-89) &&  // '!' == 33 in ASCII
                    createfontMap(90, 1, 39-90);  // ''' == 39 in ASCII
            }

            SDL_UnlockSurface(sfc);

            if(!success)
            {
                return false;
            }
        }

        font.deriveHighResSurfaces();
    }

    return true;
}


void filterList(std::set<std::string> &mySet,
                const std::string nameFilter)
{
    for(auto it=mySet.begin() ; it!=mySet.end();)
    {
        if(it->find(nameFilter) == it->npos)
            it = mySet.erase(it);
        else
            it++;
    }
}

/**
 * \brief   This one extracts the bitmaps used in Keen 4-6 (Maybe Dreams in future)
 * \return  returns true, if the fonts were read successfully, else false
 */
bool CEGAGraphicsGalaxy::readBitmaps()
{
    const auto ep = getEpisodeInfoIndex();

    const EpisodeInfoStruct &epInfo = EpisodeInfo[ep];
    
    const auto &exefile = gKeenFiles.exeFile;
    

    // ARM processor requires all ints and structs to be 4-byte aligned, so we're just using memcpy()
    BitmapHeadStruct BmpHead[epInfo.NumBitmaps];
    memcpy( BmpHead, &(m_egagraph.at(0).data.at(0)), epInfo.NumBitmaps*sizeof(BitmapHeadStruct));
    SDL_Color *Palette = gGraphics.Palette.m_Palette;

    gGraphics.createEmptyBitmaps(1, epInfo.NumBitmaps);

    SDL_Rect bmpRect;
    bmpRect.x = bmpRect.y = 0;


    size_t bitmapNameOffset = ep;

    // Special case for k6demo
    if( ep == 4 && exefile.isDemo())
    {
        bitmapNameOffset = 3;
    }

    auto &bitmapNamesThisEp = m_BitmapNameMap[bitmapNameOffset];

    for(unsigned int i = 0; i < epInfo.NumBitmaps; i++)
    {
        // Use upper limit to protect against overflow.
        if(BmpHead[i].Width < 1 || BmpHead[i].Width > 100)
        {
            gLogging.ftextOut("bad bitmap i=%u BmpHead[i].Width=%u", i, BmpHead[i].Width);
            return false;
        }
        if(BmpHead[i].Height < 1 || BmpHead[i].Height > 1000)
        {
            gLogging.ftextOut("bad bitmap i=%u BmpHead[i].Height=%u", i, BmpHead[i].Height);
            return false;
        }

        // Check that data size is consistent with width and height.
        std::vector<unsigned char> &data = m_egagraph.at(epInfo.IndexBitmaps + i).data;
        if(!data.empty() && BmpHead[i].Width * BmpHead[i].Height * 4u != data.size())
        {
            gLogging.ftextOut("bad bitmap i=%u Width=%u Height=%u size=%u", i, BmpHead[i].Width, BmpHead[i].Height, data.size());
            return false;
        }

        GsBitmap &bitmap = gGraphics.getBitmapFromId(0, i);
        bmpRect.w = BmpHead[i].Width*8;
        bmpRect.h = BmpHead[i].Height;
        bitmap.createSurface(gVideoDriver.getScrollSurface()->flags,
                             bmpRect, Palette);

        extractPicture(bitmap.getSDLSurface(),
                data,
                BmpHead[i].Width, BmpHead[i].Height);

        auto &bmpName = bitmapNamesThisEp[i];

        gGraphics.setBitmapNameForIdx(bmpName, i);        
    }


    // For extra graphics loaded externally

    // Looks for bitmap files lying around
    // If there are some load them as well
    const std::string gfxDir = "gfx";
    const std::string playerDir = "player";
    const std::string playersPathList = JoinPaths(gKeenFiles.gameDir,
                                                 gfxDir,
                                                 playerDir);

    std::set<std::string> playersList;
    FileListAdder fileListAdder;
    GetFileList(playersList, fileListAdder,
                playersPathList, false, FM_DIR);

    const auto BitmapOrigVec = gGraphics.bmpVecVec()[0];

    // If there are more players than preallocated, enlarge the rooster
    if(gGraphics.bmpVecVec().size() < playersList.size())
    {
        auto curNumBmpVecs = gGraphics.bmpVecVec().size();

        gLogging << "More players to load. Enlarging bitmap buffer...";

        // Copy the bitmap to the enlarged version
        for( auto i=curNumBmpVecs ; i< playersList.size() ; i++ )
        {
            gGraphics.appendBitmapVec(BitmapOrigVec);
        }
    }



    // For a list of players try to load different portraits
    size_t numbitmapVar = 0;
    for(const auto &player : playersList)
    {
        auto &bmpVecPlayer = gGraphics.bmpVecVec()[numbitmapVar];

        const std::string curPlayerPath = JoinPaths(playersPathList, player);

        std::set<std::string> bmpList;
        FileListAdder bmpfilesAdder;
        GetFileList(bmpList, bmpfilesAdder,
                    curPlayerPath, false, FM_REG);

        filterList(bmpList, "_pic_");

        for(const auto &bmpFile : bmpList)
        {
            const auto numStr = bmpFile.substr(bmpFile.length()-8,
                                               bmpFile.length()-4);

            int idx = atoi(numStr.c_str());

            if(idx >= int(bmpVecPlayer.size()))
            {
                gLogging << "Warning: Index " << idx << " out of reach.";
                continue;
            }

            const auto bmpFilePath = JoinPaths(curPlayerPath,
                                                  bmpFile);

            auto &bmp = bmpVecPlayer[size_t(idx)];

            if( !bmp.loadHQBitmap(bmpFilePath) )
            {
                gLogging << "Warning: " << bmpFile << " could not be loaded.";
            }
        }

        numbitmapVar++;
    }




    return true;
}

bool CEGAGraphicsGalaxy::readMaskedBitmaps()
{
    const int ep = getEpisodeInfoIndex();

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
        // Use upper limit to protect against overflow.
        if(BmpMaskedHead[i].Width < 1 || BmpMaskedHead[i].Width > 100)
        {
            gLogging.ftextOut("bad masked bitmap i=%u Width=%u", i, BmpMaskedHead[i].Width);
            return false;
        }
        if(BmpMaskedHead[i].Height < 1 || BmpMaskedHead[i].Height > 1000)
        {
            gLogging.ftextOut("bad masked bitmap i=%u Height=%u", i, BmpMaskedHead[i].Height);
            return false;
        }

        // Check that data size is consistent with width and height.
        std::vector<unsigned char> &data = m_egagraph.at(EpisodeInfo[ep].IndexMaskedBitmaps + i).data;
        if(!data.empty() && BmpMaskedHead[i].Width * BmpMaskedHead[i].Height * 5u != data.size())
        {
            gLogging.ftextOut("bad masked bitmap i=%u Width=%u Height=%u size=%u", i, BmpMaskedHead[i].Width, BmpMaskedHead[i].Height, data.size());
            return false;
        }

        GsBitmap &Bitmap = gGraphics.getMaskedBitmap(i);
        bmpRect.w = BmpMaskedHead[i].Width*8;
        bmpRect.h = BmpMaskedHead[i].Height;

        Bitmap.createSurface(gVideoDriver.getScrollSurface()->flags, bmpRect, Palette);

        extractPicture(Bitmap.getSDLSurface(),
                data,
                BmpMaskedHead[i].Width, BmpMaskedHead[i].Height, true);
    }
    return true;
}

bool CEGAGraphicsGalaxy::readTilemaps( const size_t NumTiles, size_t pbasetilesize,
                                        size_t rowlength, size_t IndexOfTiles,
                                        GsTilemap &Tilemap, bool tileoff)
{
    const std::string  &gamedir = gKeenFiles.gameDir;
    
    Tilemap.CreateSurface( gGraphics.Palette.m_Palette, SDL_SWSURFACE,
                            NumTiles, pbasetilesize, rowlength );
    SDL_Surface *sfc = Tilemap.getSDLSurface();
    SDL_FillRect(sfc, nullptr, 0);
    if(SDL_MUSTLOCK(sfc))   SDL_LockSurface(sfc);

    std::vector<unsigned char> &data = m_egagraph.at(IndexOfTiles).data;

    const Uint16 size = (1 << pbasetilesize);

    const size_t tileSize = 4 * (size / 8) * size;

    if(tileoff)
    {
        size_t expectedSize = NumTiles * rowlength * tileSize;
        if(!data.empty() && data.size() != expectedSize)
        {
            gLogging.ftextOut("bad tile offset data expected size=%u data size=%u", expectedSize, data.size());
        }
    }

    for(size_t i = 0; i < NumTiles; i++)
    {
        // Check that data size is consistent with pbasetilesize and rowlength.
        if(!tileoff)
        {
            data = m_egagraph.at(IndexOfTiles + i).data;
            if(!data.empty() && data.size() != tileSize)
            {
                gLogging.ftextOut("bad tile i=%u expected size=%u data size=%u", i, tileSize, data.size());
                return false;
            }
        }

        extractTile(sfc, data, size, rowlength, i, tileoff);
    }

    // std::string filename = std::string("/tmp/read_tilemaps_") + std::to_string(NumTiles) + std::string("_") + std::to_string(IndexOfTiles) + std::string(".bmp");
    // SDL_SaveBMP(sfc, filename.c_str());

    SDL_UnlockSurface(sfc);

    /// Let's see if there is a high colour tilemap we can load instead
    if(pbasetilesize == 4) // Only valid for the 16x16 tiles tilemap!
    {
        Tilemap.loadHiresTile("gfx/4TIL0000", gamedir);
    }

    // Optimize surfaces for the screen
    Tilemap.optimizeSurface();

    return true;
}

bool CEGAGraphicsGalaxy::readMaskedTilemaps( size_t NumTiles, size_t pbasetilesize,
                                            size_t rowlength, size_t IndexOfTiles,
                                            GsTilemap &Tilemap, bool tileoff)
{
    const std::string  &gamedir = gKeenFiles.gameDir;  
  
    Tilemap.CreateSurface( gGraphics.Palette.m_Palette, SDL_SWSURFACE,
                            NumTiles, pbasetilesize, rowlength );
    SDL_Surface *sfc = Tilemap.getSDLSurface();
    SDL_FillRect(sfc,NULL, 0);
    if(SDL_MUSTLOCK(sfc))   SDL_LockSurface(sfc);

    std::vector<unsigned char> &data = m_egagraph.at(IndexOfTiles).data;

    const Uint16 size = (1 << pbasetilesize);

    const size_t tileSize = 5 * (size / 8) * size;

    if(tileoff)
    {
        const size_t expectedSize = NumTiles * rowlength * tileSize;
        if(!data.empty() && data.size() != expectedSize)
        {
            gLogging.ftextOut("bad masked tile offset data expected size=%u data size=%u", expectedSize, data.size());
        }
    }

    for(size_t i = 0; i < NumTiles; i++)
    {
        // Check that data size is consistent with pbasetilesize and rowlength.
        if(!tileoff)
        {
            data = m_egagraph.at(IndexOfTiles + i).data;
            if(!data.empty() && data.size() != tileSize)
            {
                gLogging.ftextOut("bad masked tile i=%u expected size=%u data size=%u", i, tileSize, data.size());
                return false;
            }
        }

        extractMaskedTile(sfc, data, size, rowlength, i, tileoff);
    }

    // std::string filename = std::string("/tmp/read_masked_tilemaps_") + std::to_string(NumTiles) + std::string("_") + std::to_string(IndexOfTiles) + std::string(".bmp");
    // SDL_SaveBMP(sfc, filename.c_str());

    SDL_UnlockSurface(sfc);

    /// Let's see if there is a high colour tilemap we can load instead
    if(pbasetilesize == 4) // Only valid for the 16x16 tiles tilemap!
    {
        // Looking for high color pictures
        if( Tilemap.loadHiresTile("gfx/4TIL0001", gamedir) )
        {
            Tilemap.applyGalaxyHiColourMask();
        }
    }

    return true;
}



bool CEGAGraphicsGalaxy::readSprites( const size_t numSprites,
                                      const size_t indexSprite )
{
    // Create all the sprites
    gGraphics.createEmptySprites(4, numSprites);

    const auto ep = static_cast<int>(getEpisodeInfoIndex());

    // Check that source head data size is appropriate.
    const std::vector<unsigned char> &headData = m_egagraph.at(2).data;
    if(headData.size() != numSprites * sizeof(SpriteHeadStruct))
    {
        gLogging << "bad sprite head data size=" <<  int(headData.size()) << ".";
        return false;
    }

    // ARM processor requires all ints and structs to be 4-byte aligned, so we're just using memcpy()
    std::vector<SpriteHeadStruct> sprHeads(numSprites, SpriteHeadStruct());
    memcpy( sprHeads.data(), &(headData.at(0)), numSprites*sizeof(SpriteHeadStruct) );

    for(int i = 0; i < int(numSprites); i++)
    {
        const SpriteHeadStruct curSprHead = sprHeads[size_t(i)];

        auto &data = m_egagraph.at(indexSprite + size_t(i)).data;

        // Check that data size is consistent with Head.Width and Head.Height.
        // Width and Height are unsigned short, so there's no overflow risk.
        if(!data.empty() && data.size() != (curSprHead.Width * curSprHead.Height * 5u))
        {
            gLogging << "bad sprite data at " << i
                     << " Size=" << curSprHead.Width << "x" << curSprHead.Height
                     << " datasize=" << int(data.size()) << "\n";
            return false;
        }

        GsSprite &sprite = gGraphics.getSprite(0, i);
        sprite.setSize( Uint8(curSprHead.Width*8), Uint8(curSprHead.Height) );

        sprite.setOffset( curSprHead.OrgX>>(TILE_S), curSprHead.OrgY>>(TILE_S) );

        // Setup the collision information
        int boxX1 = ((curSprHead.Rx1) << (STC-TILE_S));
        int boxY1 = ((curSprHead.Ry1) << (STC-TILE_S));
        int boxX2 = ((curSprHead.Rx2) << (STC-TILE_S));
        int boxY2 = ((curSprHead.Ry2) << (STC-TILE_S));

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

        sprite.setBoundingBoxCoordinates( boxX1, boxY1, boxX2, boxY2 );

        sprite.createSurface( gVideoDriver.mpVideoEngine->getBlitSurface()->flags,
                              gGraphics.Palette.m_Palette );

        auto &gsSfc = sprite.Surface();
        gsSfc.fillRGB(0,0,0);

        SDL_Surface *sfc = gsSfc.getSDLSurface();

        if(SDL_MUSTLOCK(sfc))   SDL_LockSurface(sfc);

        if(!data.empty())
        {
            // Decode the image data
            for(size_t p = 0; p < 4; p++)
            {
                // Decode the lines of the bitmap data
                Uint8 *pointer = &(data[0]) + (p+1) * curSprHead.Width * curSprHead.Height;
                for(size_t y = 0; y < curSprHead.Height; y++)
                {
                    Uint8 *pixel = (Uint8*)sfc->pixels +
                            (curSprHead.Width * 8 *y);
                    for(size_t x = 0; x < curSprHead.Width; x++)
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
            for(size_t y = 0; y < curSprHead.Height; y++)
            {
                Uint8 *pixel = (Uint8*)sfc->pixels +
                        (curSprHead.Width * 8*y);
                for(size_t x = 0; x < curSprHead.Width; x++)
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

        // Special case for k6demo
        size_t spriteNameOffset = (ep == 4 ? 3 : ep);
        sprite.setName(m_SpriteNameMap[spriteNameOffset][i]);

        // std::string filename = std::string("/tmp/read_sprites_") + std::to_string(i) + std::string(".bmp");
        // SDL_SaveBMP(sfc, filename.c_str());
    }

    // Now let's copy all the sprites. After that some of them are tinted to the proper colors

    const auto SpriteOrigVec = gGraphics.getSpriteVec(0);

    // Copy the sprites
    for( unsigned int i=1 ; i<4 ; i++ )
    {
        gGraphics.getSpriteVec(i) = SpriteOrigVec;
    }

    // For the other variant let's exchange some colors
    // Second Player, could be kylie's own bitmaps
    {
        auto &SpriteVecPlayer2 = gGraphics.getSpriteVec(1);
        int ctr = 0;

        {
            for( GsSprite &sprite : SpriteVecPlayer2)
            {
                // Red against Purple
                sprite.exchangeSpriteColor( 5, 4, 0 );
                sprite.exchangeSpriteColor( 13, 12, 0 );

                // Yellow against Green
                sprite.exchangeSpriteColor( 2, 6, 0 );
                sprite.exchangeSpriteColor( 10, 14, 0 );
                ctr++;
            }

        }
    }

    // Third Player
    {
        auto &SpriteVecPlayer3 = gGraphics.getSpriteVec(2);
        for( auto &sprite : SpriteVecPlayer3)
        {
            // Red against Green
            sprite.exchangeSpriteColor( 2, 4, 0 );
            sprite.exchangeSpriteColor( 10, 12, 0 );

            // Yellow against Purple
            sprite.exchangeSpriteColor( 5, 6, 0 );
            sprite.exchangeSpriteColor( 13, 14, 0 );
        }
    }

    // Fourth Player
    {
        auto &SpriteVecPlayer4 = gGraphics.getSpriteVec(3);
        for( auto &sprite : SpriteVecPlayer4)
        {
            // Red against Yellow
            sprite.exchangeSpriteColor( 6, 4, 0 );
            sprite.exchangeSpriteColor( 14, 12, 0 );

            // Green against Purple
            sprite.exchangeSpriteColor( 2, 5, 0 );
            sprite.exchangeSpriteColor( 10, 13, 0 );
        }
    }

    const std::string gfxDir = "gfx";
    const std::string playerDir = "player";
    const std::string playersPathList = JoinPaths(gKeenFiles.gameDir,
                                                 gfxDir,
                                                 playerDir);

    std::set<std::string> playersList;
    FileListAdder fileListAdder;
    GetFileList(playersList, fileListAdder,
                playersPathList, false, FM_DIR);

    // If there are more players than preallocated, enlarge the rooster
    if(gGraphics.spriteVecVec().size() < playersList.size())
    {
        auto curNumSpriteVecs = gGraphics.spriteVecVec().size();

        gLogging << "More players to load. Enlarging sprite buffer...";

        // Copy the sprites
        for( auto i=curNumSpriteVecs ; i< playersList.size() ; i++ )
        {
            gGraphics.appendSpriteVec(SpriteOrigVec);
        }
    }


    // For a list of players try to load the sprites
    int numSpriteVar = 0;
    for(const auto &player : playersList)
    {        
        auto &SpriteVecPlayer = gGraphics.getSpriteVec(numSpriteVar);

        const std::string curPlayerPath = JoinPaths(playersPathList, player);

        std::set<std::string> spriteList;
        FileListAdder spritefilesAdder;
        GetFileList(spriteList, spritefilesAdder,
                    curPlayerPath, false, FM_REG,
                    "*_sprite_*");

        filterList(spriteList, "_sprite_");

        for(const auto &spriteFile : spriteList)
        {
            const auto numStr = spriteFile.substr(spriteFile.length()-8,
                                                  spriteFile.length()-4);

            int idx = atoi(numStr.c_str());

            if(idx >= int(SpriteVecPlayer.size()))
            {
                gLogging << "Warning: Index " << idx << " out of reach.";
                continue;
            }

            const auto spriteFilePath = JoinPaths(curPlayerPath,
                                                  spriteFile);

            GsSprite &sprite = SpriteVecPlayer[size_t(idx)];

            if( sprite.loadHQSprite(spriteFilePath) )
            {
                sprite.applyTransparency();
            }
            else
            {
                gLogging << "Warning: " << spriteFile << " could not be loaded.";
            }
        }

        numSpriteVar++;
    }

    return true;
}



bool CEGAGraphicsGalaxy::readTexts()
{
    const auto ep = getEpisodeInfoIndex();

    gGameText.clear();

    for(unsigned int i = 0; i < EpisodeInfo[ep].NumTexts; i++)
    {
        ChunkStruct &thisChunk = m_egagraph.at(EpisodeInfo[ep].IndexTexts + i);

        if(thisChunk.data.size() == 0)
        {
            gLogging.ftextOut("bad text chunk (zero size) i=%u", i);
            return false;
        }

        if(thisChunk.data.at(0))
        {
            std::vector<char> chunkData(thisChunk.data.size()+1, 0);

            memcpy(chunkData.data(), thisChunk.data.data(), thisChunk.data.size());

            const auto *txtData = reinterpret_cast<char*>( chunkData.data() );
            std::string text(txtData);

            gGameText.addLine(text);
        }
    }

    return true;
}



// This sets up gGraphics.getMiscGsBitmap surfaces (0 for "COMMANDER", 1 for "KEEN").
bool CEGAGraphicsGalaxy::readMiscStuff()
{
    int width = 0; int height = 0;
    SDL_Color *Palette = gGraphics.Palette.m_Palette;

    size_t indexMisc = EpisodeInfo[getEpisodeInfoIndex()].IndexMisc;
    
    const auto &exefile = gKeenFiles.exeFile;
    const int  episode  = exefile.getEpisode();        

    // Looks for bitmap files lying around
    // If there are some load them as well
    const std::string gfxDir = "gfx";
    const std::string bmpsDir = "misc";
    const std::string bmpDirPath = JoinPaths(gKeenFiles.gameDir,
                                             gfxDir, bmpsDir);

    std::set<std::string> bmpFileList;
    FileListAdder bmpfilesAdder;
    GetFileList(bmpFileList, bmpfilesAdder,
                bmpDirPath, false, FM_REG);

    
    // Only position 1 and 2 are read. This will the terminator text.
    // Those are monochrom...
    for(size_t miscIdx = 0 ; miscIdx < EpisodeInfo[episode-4].NumMisc; miscIdx++)
    {
        const auto index = indexMisc + miscIdx;

        const auto &dataChunk = m_egagraph.at(index);

        const auto dataSize = dataChunk.data.size();

        if(dataSize < (sizeof(Uint16) * 3))
        {
            gLogging.ftextOut("bad misc data size=%u at index=%d misc=%d",
                              dataSize, index, miscIdx);
            return false;
        }

        Uint16 *dataPtr = nullptr;
        memcpy( &dataPtr, &(dataChunk.data), sizeof(Uint16 *) );

        memcpy(&height, dataPtr, sizeof(Uint16) );
        dataPtr++;
        memcpy(&width, dataPtr, sizeof(Uint16) );
        dataPtr++;

        gLogging.ftextOut("misc width=%d height=%d index=%d misc=%d",
                          width, height, index, miscIdx);
        if(miscIdx < 1 || miscIdx > 2)
        {
            // Skip these other misc entries.
            // todo: What are these other misc entries for?
            continue;
        }

        // Limit the height and width to ensure 32-bit safety further below.
        // The minima are set to avoid crashes elsewhere in the code.
        if(height < 4 || height > 10000)
        {
            gLogging.ftextOut("bad misc height=%d index=%d misc=%d",
                              height, index, miscIdx);
            return false;
        }
        if(width < 2 || width > 10000)
        {
            gLogging.ftextOut("bad misc width=%d index=%d misc=%d",
                              width, index, miscIdx);
            return false;
        }

        if(((2 + height) * sizeof(Uint16)) > dataSize)
        {
            gLogging.ftextOut("bad misc data size=%u for height=%d index=%d misc=%d",
                              dataSize, height, index, miscIdx);
            return false;
        }

        SDL_Rect bmpRect;

        GsBitmap &Bitmap = gGraphics.getMiscGsBitmap(miscIdx-1);
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

        switch(episode) // The color of the terminator depends on the episode.
        {
            case 6:  textColor = SDL_MapRGB(bmp->format, 0xff,0x55,0xff); break;
            case 5:  textColor = SDL_MapRGB(bmp->format, 0xff,0x55,0x55); break;
            default: textColor = SDL_MapRGB(bmp->format, 0xaa,0xaa,0xaa); break;
        }

        const Uint32 blackColor = SDL_MapRGB(bmp->format, 0,0,0);

        Uint32 currentColor = blackColor;

        const int expectedNumPixels = width * height;

        int pixelNum = 0;

        bool bad = false;
        while(pixelNum < expectedNumPixels)
        {
            /*
            if(rlepointer == dataEndPtr)
            {
                gLogging.ftextOut("bad misc rle data size=%u for pixelNum=%d width=%d height=%d index=%d misc=%d",
                                  dataSize, pixelNum, width, height, index, misc);
                bad = true;
                break;
            }*/

            Uint16 pixelCount = *rlepointer;
            if(pixelCount != 0xFFFF)
            {
                if(pixelNum + pixelCount > expectedNumPixels)
                {
                    gLogging.ftextOut("bad misc rle pixel count %u for pixelNum=%d expectedNumPixels=%u width=%d height=%d index=%d misc=%d",
                                      pixelCount, pixelNum, expectedNumPixels, width, height, index, miscIdx);
                    bad = true;
                    break;
                }

                for(int i=0 ; i<pixelCount ; i++)
                {
                    *sfcPtr = currentColor;
                    sfcPtr += bytePerPixel;
                }
                pixelNum += pixelCount;
            }

            currentColor =
                    (currentColor == blackColor) ?
                        textColor : blackColor;

            rlepointer++;
        }

        if(pixelNum != expectedNumPixels)
        {
            gLogging.ftextOut("Something is wrong with the number of read pixels in misc %d.\n", miscIdx);
            bad = true;
        }

        SDL_UnlockSurface(bmp);

        if(bad)
        {
            return false;
        }

        for(const auto &bmpFile : bmpFileList)
        {
            const auto numStr = bmpFile.substr(bmpFile.length()-8,
                                               bmpFile.length()-4);

            size_t idx = atoi(numStr.c_str());

            if((miscIdx-1) != idx)
                continue;

            if(idx >= EpisodeInfo[episode-4].NumMisc)
            {
                gLogging << "Warning: Index " << idx << " out of reach.";
                continue;
            }

            const auto bmpFilePath = JoinPaths(bmpDirPath, bmpFile);

            if( Bitmap.loadHQBitmap(bmpFilePath) )
            {
                gLogging << bmpFile << " got loaded correctly.";
            }
            else
            {
                gLogging << "Warning: " << bmpFile << " did not get loaded correctly.";
            }

        }
    }

    return true;
}

}
