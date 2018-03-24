/*
 * CEGAGraphics.h
 *
 *  Created on: 27.12.2009
 *      Author: gerstrong
 */

#ifndef CGALAXY_EGAGRAPHICS_H_
#define CGALAXY_EGAGRAPHICS_H_

#include <string>
#include <vector>
#include <array>
#include <map>
#include <SDL.h>
#include "fileio/CExeFile.h"
#include "graphics/GsTilemap.h"

#include "EGAStructs.h"

namespace galaxy
{


class CEGAGraphicsGalaxy
{
public:

	CEGAGraphicsGalaxy();

	int getNumSprites();
	short getNumTiles();

    /**
     * @brief	load the data into the structure
     * @return 	returns true, if loading was successful
     */
	bool loadData();


    /**
     * @brief   This function extracts a picture from the galaxy graphics map, and converts it properly to a
     *          SDL Surface
     */
	void extractPicture(SDL_Surface *sfc,
			std::vector<unsigned char> &data, size_t Width, size_t Height,
			bool masked=false);

	void extractTile(SDL_Surface *sfc, std::vector<unsigned char> &data,
			Uint16 size, Uint16 columns, size_t tile, bool usetileoffset);
	void extractMaskedTile(SDL_Surface *sfc, std::vector<unsigned char> &data,
			Uint16 size, Uint16 columns, size_t tile, bool usetileoffset);

    std::vector<unsigned long> readOutLenVec(const int ep,
                                             const std::vector<unsigned char> &compEgaGraphData);

	bool begin();
	Uint8 getBit(unsigned char data, Uint8 leftshift);
	bool readEGAHead();
	bool readTables();
	bool readfonts();
	bool readBitmaps();
	bool readMaskedBitmaps();
    bool readTilemaps(const size_t NumTiles, size_t pbasetilesize,
            size_t rowlength, size_t IndexOfTiles,
            GsTilemap &Tilemap, bool tileoff );
	bool readMaskedTilemaps( size_t NumTiles, size_t pbasetilesize,
			size_t rowlength, size_t IndexOfTiles,
			GsTilemap &Tilemap, bool tileoff );

    /**
     * @brief readTexts This reads the Texts used in Computer Wrist and Ending as Beginning Pages. In-Level texts are not handled here
     * @return true if everything went fine, otherwise false.
     */
	bool readTexts();

    /**
     * @brief readMiscStuff Stuff like Demos and Terminator text
     *        is read here...
     * @return true if everything went fine, otherwise false.
     */
    bool readMiscStuff();

    /**
     * @brief readSprites   Reads all the used sprites by the game
     * @param numSprites    Number of sprites to read
     * @param indexSprite   Index where reading of sprites start.
     * @return true if everything went fine, otherwise false.
     */
    bool readSprites(const size_t numSprites,
                     const size_t indexSprite );

	void createBitmapsIDs();

private:

    size_t getEpisodeInfoIndex();

	std::vector<unsigned long> m_egahead;
	std::vector<ChunkStruct> m_egagraph;


    std::array<std::array<std::string, 1000>, 4> m_BitmapNameMap;
    std::array<std::array<std::string, 1000>, 4> m_SpriteNameMap;
};

}

#endif /* CGALAXY_EGAGRAPHICS_H_ */
