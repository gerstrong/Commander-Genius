/* MAP.C
  Functions that deal with the level map. Most notably in here
  you'll find the 4-way scrolling engine.
*/

#include "keen.h"
#include "sdl/CVideoDriver.h"
#include "graphics/CGfxEngine.h"

unsigned long scroll_x = 0;      // total amount of X scroll
unsigned int scrollx_buf = 0;    // amount the scroll buffer is scrolled(x)
unsigned char scrollpix = 0;     // (0-7) for tracking when to draw a stripe
unsigned int mapx = 0;           // map X location shown at scrollbuffer row 0
unsigned int mapxstripepos = 0;  // X pixel position of next stripe row

unsigned int scroll_y = 0;
unsigned int scrolly_buf = 0;    // amount the scroll buffer is scrolled(y)
unsigned char scrollpixy = 0;    // (0-7) for tracking when to draw a stripe
unsigned int mapy = 0;           // map Y location shown at scrollbuffer column 0
unsigned int mapystripepos = 0;  // Y pixel position of next stripe column

unsigned int getlevelat(unsigned int x, unsigned int y)
{
  //return map.objectlayer[x>>4][y>>4];
	return 0;
}

// searches for animated tiles at the map position (X,Y) and
// unregisters them from animtiles
void map_deanimate(int x, int y)
{
/*int px,py;
int i;
      // figure out pixel position of map tile (x,y)
      px = ((mapxstripepos+((x-mapx)<<4))&511);
      py = ((mapystripepos+((y-mapy)<<4))&511);

      // find it!
      for(i=1;i<MAX_ANIMTILES-1;i++)
      {
        if (animtiles[i].x == px && animtiles[i].y == py)
        {
          animtiles[i].slotinuse = 0;
          animtiles[i].offset = 0;
          AnimTileInUse[px>>4][py>>4] = 0;
          return;
        }
      }
      */
}

// tells if the object is animating at a given position
int map_isanimated(int x, int y)
{
/*int px,py;
int i;
      // figure out pixel position of map tile (x,y)
      px = ((mapxstripepos+((x-mapx)<<4))&511);
      py = ((mapystripepos+((y-mapy)<<4))&511);

      // find it!
      for(i=1;i<MAX_ANIMTILES-1;i++)
      {
        if (animtiles[i].x == px && animtiles[i].y == py)
        {
        	if(animtiles[i].slotinuse != 0)
        		return 1;
        }
      }*/

      return 0; // no
}

void map_unregister_all_animtiles(void)
{
//int i;
  //for(i=0;i<MAX_ANIMTILES-1;i++) animtiles[i].slotinuse = 0;
}

// after changing a map tile in real time with map_chgtile()
// from an unanimated to an animated tile, you must call this
// function to register the new animated tile.
void map_animate(int x, int y)
{
/*int px,py;
int c, i;

      // figure out pixel position of map tile (x,y)
      px = ((mapxstripepos+((x-mapx)<<4))&511);
      py = ((mapystripepos+((y-mapy)<<4))&511);
      c = map.mapdata[x][y];
      if ( TileProperty[c][ANIMATION] == 1 ) // In case the tile mustn't be animated
      {
          crashflag = x;
          crashflag2 = y;
    	  why_term_ptr = "sorry, but tile at x/y=crashflag1/2 isn't supposed to be animated!";
          crashflag = 0;
          crashflag2 = 0;
          // TODO: Try to remove the crashflags. They really mess up the system!

        return;
      }

      // don't reanimate a tile that's already registered--then we'd
      // have multiple entries for it in animtiles[] (that's not good).
      if (AnimTileInUse[px>>4][py>>4])
        return;

      // find an unused slot in animtiles
      for(i=1;i<MAX_ANIMTILES-1;i++)
      {
        if ( !animtiles[i].slotinuse )
        {  // we found an unused slot
           animtiles[i].x = px;
           animtiles[i].y = py;
           animtiles[i].baseframe = c - tiles[c].animOffset;
           animtiles[i].offset = tiles[c].animOffset;
           animtiles[i].slotinuse = 1;
           AnimTileInUse[px>>4][py>>4] = i;
           return;
        }
      }

      crashflag = 1;
      crashflag2 = x;
      crashflag3 = y;
      why_term_ptr = "Unable to animate tile at x/y=crashflag1/2";*/
}

// searches the map's tile layer for tile TILE.
// if it is found returns nonzero and places the
// coordinates of the first occurance of the tile
// in (xout,yout)
char map_findtile(unsigned int tile, int *xout, int *yout)
{
/*unsigned int x,y;

  for(y=2;y<map.ysize-2;y++)
  {
    for(x=2;x<map.xsize-2;x++)
    {
      if (map.mapdata[x][y]==tile)
      {
        *xout = x;
        *yout = y;
        return 1;
      }
    }
  }*/
  return 0;
}

// refreshes the map at the current scroll position
// (unlike drawmap() which does not honor the scroll and will
// glitch up if scrollx/y is != 0)
/*void map_redraw(void)
{
int x,mpx;

	SDL_FillRect(g_pVideoDriver->BlitSurface, NULL, g_pVideoDriver->BlitSurface->format->colorkey);
	x = mapxstripepos;
	for(mpx=0;mpx<32;mpx++)
	{
		map_draw_vstripe(x,mpx+mapx);
		x+=16;
		x&=511;
	}
}*/
