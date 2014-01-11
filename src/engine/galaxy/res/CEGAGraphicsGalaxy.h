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
#include "engine/CEGAGraphics.h"
#include "graphics/GsTilemap.h"

#include "EGAStructs.h"

namespace galaxy
{

//
class CEGAGraphicsGalaxy : public CEGAGraphics
{
public:
	CEGAGraphicsGalaxy(CExeFile &ExeFile);

	int getNumSprites();
	short getNumTiles();

	bool loadData();
	void extractPicture(SDL_Surface *sfc,
			std::vector<unsigned char> &data, size_t Width, size_t Height,
			bool masked=false);
	void extractTile(SDL_Surface *sfc, std::vector<unsigned char> &data,
			Uint16 size, Uint16 columns, size_t tile, bool usetileoffset);
	void extractMaskedTile(SDL_Surface *sfc, std::vector<unsigned char> &data,
			Uint16 size, Uint16 columns, size_t tile, bool usetileoffset);

	bool begin();
	Uint8 getBit(unsigned char data, Uint8 leftshift);
	bool readEGAHead();
	bool readfonts();
	bool readBitmaps();
	bool readMaskedBitmaps();
	bool readTilemaps( size_t NumTiles, size_t pbasetilesize,
			size_t rowlength, size_t IndexOfTiles,
			GsTilemap &Tilemap, bool tileoff );
	bool readMaskedTilemaps( size_t NumTiles, size_t pbasetilesize,
			size_t rowlength, size_t IndexOfTiles,
			GsTilemap &Tilemap, bool tileoff );
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
