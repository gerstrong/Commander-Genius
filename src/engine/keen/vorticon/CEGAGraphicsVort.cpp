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
#include <base/utils/FindFile.h>

#include <fileio/KeenFiles.h>

namespace vorticon
{

#define SAFE_SDL_FREE(x) if(x) { SDL_FreeSurface(x); x = nullptr; }

int CEGAGraphicsVort::getNumSprites()
{ return NumSprites; }
short CEGAGraphicsVort::getNumTiles()
{ return Num16Tiles; }

bool CEGAGraphicsVort::loadData( int version, unsigned char *p_exedata )
{
  std::vector<char> data;
  
  const std::string  &gamedir = gKeenFiles.gameDir;    
  
  const auto &exefile = gKeenFiles.exeFile;
  const int  episode  = exefile.getEpisode();        
  
  
  // assure that the last used resources are freed
  
  // Set the palette, so the proper colours are loaded
  gGraphics.Palette.setupColorPalettes(p_exedata, episode);
  
  std::ifstream HeadFile;
  OpenGameFileR(HeadFile, ((gamedir != "") ? gamedir + "/" : "") +
		          "egahead.ck" + itoa(episode), std::ios::binary);
  
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
  
  CTileLoader TileLoader( episode, false, version, p_exedata );
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
  
  m_Latch->loadHead( &data[0], episode );
  
  m_Latch->loadData( gamedir, episode, version, p_exedata, (compressed>>1) ); // The second bit tells, if latch is compressed.
  
  
  m_Sprit = new CEGASprit(SpritePlaneSize,
			  SpriteStart,
			  NumSprites,
			  SpriteLocation,
              gamedir, episode,
              mLoader);
  m_Sprit->loadHead(&data[0]);
  
  struct SpriteLoad: public Action
  {
      std::string buf;
      bool compressed;
      CEGASprit *m_Sprit;
      
      SpriteLoad(CEGASprit *Sprit, const std::string& _buf, bool _compressed):
	buf(_buf), compressed(_compressed), m_Sprit(Sprit) {}
      
      int handle()
      {
	m_Sprit->loadData(buf,compressed);
	return 1;
      }
  };
  
  SpriteLoad sprLoad(m_Sprit,
		     ((gamedir != "") ? gamedir + "/" : "") + 
		     "egasprit.ck" + itoa(episode),
		     (compressed>>1));
  
  sprLoad.handle();
  
  return true;
}

CEGAGraphicsVort::~CEGAGraphicsVort()
{
  if(m_Latch) delete m_Latch;
  if(m_Sprit) delete m_Sprit;
  m_Latch = nullptr;
  m_Sprit = nullptr;
  
  SAFE_SDL_FREE(m_FontSurface);
  SAFE_SDL_FREE(m_BigTileSurface);
  SAFE_SDL_FREE(m_TileSurface);
  SAFE_SDL_FREE(m_BitmapsSurface);
}

}
