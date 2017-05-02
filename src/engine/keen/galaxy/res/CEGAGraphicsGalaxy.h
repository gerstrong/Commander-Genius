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
#include <map>
#include <SDL.h>
#include "fileio/CExeFile.h"
#include "engine/core/CEGAGraphics.h"
#include "graphics/GsTilemap.h"

#include "EGAStructs.h"

namespace galaxy
{

//
class CEGAGraphicsGalaxy : public CEGAGraphics
{
public:

    /**
     * \brief	This creates the class for reading the graphical
     * \param	episode		Episode of the chosen game
     * \param	path		Path to where the game is found on the media
     * \param	ExeFile		Object to ExeFile in which the
     */
	CEGAGraphicsGalaxy(CExeFile &ExeFile);

	int getNumSprites();
	short getNumTiles();

    /**
     * \brief	load the data into the structure
     * \return 	returns true, if loading was successful
     */
	bool loadData();


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

    bool readMiscStuff();

	bool readSprites( size_t NumSprites, size_t IndexSprite );

	void createBitmapsIDs();

private:
	std::vector<unsigned long> m_egahead;
	std::vector<ChunkStruct> m_egagraph;

	std::string m_BitmapNameMap[4][1000];
	std::string m_SpriteNameMap[4][1000];

	CExeFile &m_Exefile;
};

}

#endif /* CGALAXY_EGAGRAPHICS_H_ */
