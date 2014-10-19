/*
 * CEGAGraphicsVort.cpp
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#include "CEGAGraphicsVort.h"
#include "engine/core/CResourceLoader.h"

#include "fileio/CTileLoader.h"

#ifdef TARGET_WIN32
#include <dir.h>
#endif
#include <fstream>
#include <vector>
//#include "StringUtils.h"
#include <base/utils/FindFile.h>

namespace vorticon
{

#define SAFE_SDL_FREE(x) if(x) { SDL_FreeSurface(x); x = NULL; }

CEGAGraphicsVort::CEGAGraphicsVort(short episode, const std::string& path) :
CEGAGraphics(episode, path)
{
	// EGAHEAD Structure
	LatchPlaneSize = 0;
	SpritePlaneSize = 0;
	BitmapTableStart = 0;
	SpriteStart = 0;

	FontTiles = 0;
	FontLocation = 0;
	ScreenTiles = 0;
	ScreenLocation = 0;
	Num16Tiles = 0;
	Tiles16Location = 0;
	NumSprites = 0;
	NumBitmaps = 0;
	BitmapLocation = 0;
	SpriteLocation = 0;
	compressed = 0;

	m_Latch = NULL;
	m_Sprit = NULL;
	m_FontSurface = NULL;
	m_BigTileSurface = NULL;
	m_TileSurface = NULL;
	m_BitmapsSurface = NULL;
}

int CEGAGraphicsVort::getNumSprites()
{ return NumSprites; }
short CEGAGraphicsVort::getNumTiles()
{ return Num16Tiles; }

bool CEGAGraphicsVort::loadData( int version, unsigned char *p_exedata )
{
	std::vector<char> data;

	// assure that the last used resources are freed

	// Set the palette, so the proper colours are loaded
    gGraphics.Palette.setupColorPalettes(p_exedata, m_episode);

	std::ifstream HeadFile;
	OpenGameFileR(HeadFile, ((m_path != "") ? m_path + "/" : "") + "egahead.ck" + itoa(m_episode), std::ios::binary);

	if(!HeadFile)
		return false;

	char byte;
	while(!HeadFile.eof())
	{
		HeadFile.read(&byte,1);
		data.push_back(byte);
	}
	HeadFile.close();

	// Now copy the data to the EGAHEAD Structure
    memcpy(&LatchPlaneSize,&data[0],4);
    memcpy(&SpritePlaneSize,&data[0]+4,4);
    memcpy(&BitmapTableStart,&data[0]+8,4);
    memcpy(&SpriteStart,&data[0]+12,4);

    memcpy(&FontTiles,&data[0]+16,2);
    memcpy(&FontLocation,&data[0]+18,4);
    memcpy(&ScreenTiles,&data[0]+22,2);
    memcpy(&ScreenLocation,&data[0]+24,4);
    memcpy(&Num16Tiles,&data[0]+28,2);
    memcpy(&Tiles16Location,&data[0]+30,4);
    memcpy(&NumBitmaps,&data[0]+34,4);
    memcpy(&BitmapLocation,&data[0]+36,4);
    memcpy(&NumSprites,&data[0]+40,4);
    memcpy(&SpriteLocation,&data[0]+42,4);
    memcpy(&compressed,&data[0]+46,4);

	// First, retrieve the Tile properties so the tilemap gets properly formatted
	// Important especially for masks, and later in the game for the behaviours
    // of those objects

	CTileLoader TileLoader( m_episode, version, p_exedata );
	if(!TileLoader.load(0, Num16Tiles))
		return false;

    m_Latch = new CEGALatch(LatchPlaneSize,
							BitmapTableStart,
							FontTiles,
							FontLocation,
							ScreenTiles,
							ScreenLocation,
							Num16Tiles,
							Tiles16Location,
							NumBitmaps,
							BitmapLocation);

    m_Latch->loadHead( &data[0], m_episode );

    m_Latch->loadData( m_path, m_episode, version, p_exedata, (compressed>>1) ); // The second bit tells, if latch is compressed.


    m_Sprit = new CEGASprit(SpritePlaneSize,
							SpriteStart,
							NumSprites,
							SpriteLocation,
							m_path, m_episode);
    m_Sprit->loadHead(&data[0]);

    struct SpriteLoad: public Action
	{
    	std::string buf;
    	bool compressed;
    	CEGASprit *m_Sprit;
		SpriteLoad(CEGASprit *Sprit, const std::string& _buf, bool _compressed):
			buf(_buf), compressed(_compressed), m_Sprit(Sprit) {};
		int handle()
		{
			m_Sprit->loadData(buf,compressed);
			return 1;
		}
	};

    SpriteLoad sprLoad(m_Sprit,
                       ((m_path != "") ? m_path + "/" : "") + "egasprit.ck" + itoa(m_episode),
                       (compressed>>1));
	
    sprLoad.handle();
	
    return true;
}

CEGAGraphicsVort::~CEGAGraphicsVort()
{
	if(m_Latch) delete m_Latch;
	if(m_Sprit) delete m_Sprit;
	m_Latch = NULL;
	m_Sprit = NULL;

	SAFE_SDL_FREE(m_FontSurface);
	SAFE_SDL_FREE(m_BigTileSurface);
	SAFE_SDL_FREE(m_TileSurface);
	SAFE_SDL_FREE(m_BitmapsSurface);
}

}
