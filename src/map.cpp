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

unsigned int scrolly_buf = 0;    // amount the scroll buffer is scrolled(y)
unsigned char scrollpixy = 0;    // (0-7) for tracking when to draw a stripe
unsigned int mapy = 0;           // map Y location shown at scrollbuffer column 0
unsigned int mapystripepos = 0;  // Y pixel position of next stripe column

// note that the scroll buffer is 512x512, this is where all the 511 and 512
// numbers come from.

// scrolls the map one pixel right
void map_scroll_right(void)
{
     scroll_x++;
     if(scrollx_buf>=511) scrollx_buf=0; else scrollx_buf++;

     scrollpix++;
     if (scrollpix>15)
     {  // need to draw a new stripe
       map_draw_vstripe(mapxstripepos, mapx + 32);
       mapx++;
       mapxstripepos += 16;
       if (mapxstripepos >= 512) mapxstripepos = 0;
       scrollpix = 0;
     }
}

// scrolls the map one pixel left
void map_scroll_left(void)
{
     scroll_x--;
     if(scrollx_buf==0) scrollx_buf=511; else scrollx_buf--;

     if (scrollpix==0)
     {  // need to draw a new stripe
       mapx--;
       if (mapxstripepos == 0)
       {
         mapxstripepos = (512 - 16);
       }
       else
       {
         mapxstripepos -= 16;
       }
       map_draw_vstripe(mapxstripepos, mapx);

       scrollpix = 15;
     } else scrollpix--;
}

void map_scroll_down(void)
{
  scroll_y++;
  if(scrolly_buf>=511) scrolly_buf=0; else scrolly_buf++;

     scrollpixy++;
     if (scrollpixy>15)
     {  // need to draw a new stripe
       map_draw_hstripe(mapystripepos, mapy + 32);
       mapy++;
       mapystripepos += 16;
       if (mapystripepos >= 512) mapystripepos = 0;
       scrollpixy = 0;
     }
}

void map_scroll_up(void)
{
  scroll_y--;
  if(scrolly_buf==0) scrolly_buf=511; else scrolly_buf--;

     if (scrollpixy==0)
     {  // need to draw a new stripe
       mapy--;
       if (mapystripepos == 0)
       {
         mapystripepos = (512 - 16);
       }
       else
       {
         mapystripepos -= 16;
       }
       map_draw_hstripe(mapystripepos, mapy);

       scrollpixy = 15;
     } else scrollpixy--;
}


// draws a vertical stripe from map position mapx to scrollbuffer position x
void map_draw_vstripe(unsigned int x, unsigned int mpx)
{
int i,y,c;
  for(y=0;y<SCROLLBUF_NUMTILESY;y++)
  {
      c = map.mapdata[mpx][y+mapy];
	  g_pGfxEngine->Tilemap->drawTile(g_pVideoDriver->getScrollSurface(), x, ((y<<4)+mapystripepos)&511, c);

      if (AnimTileInUse[x>>4][(((y<<4)+mapystripepos)&511)>>4])
      { // we just drew over an animated tile which we must unregister
        animtiles[AnimTileInUse[x>>4][(((y<<4)+mapystripepos)&511)>>4]].slotinuse = 0;
        AnimTileInUse[x>>4][(((y<<4)+mapystripepos)&511)>>4] = 0;
      }
      if ( TileProperty[c][ANIMATION] > 1 )
      { // we just drew an animated tile which we will now register
          for(i=1;i<MAX_ANIMTILES-1;i++)
          {
            if ( !animtiles[i].slotinuse)
            {  // we found an unused slot
                animtiles[i].x = x;
                animtiles[i].y = (((y<<4)+mapystripepos)&511);
                animtiles[i].baseframe = c - tiles[c].animOffset;
                animtiles[i].offset = tiles[c].animOffset;
                animtiles[i].slotinuse = 1;
                AnimTileInUse[x>>4][((((y<<4)+mapystripepos)&511))>>4] = i;
                goto stop;             // and drop out of the loop
            }
          }
          stop: ;
      }
  }
}
// draw a horizontal stripe, for vertical scrolling
void map_draw_hstripe(unsigned int y, unsigned int mpy)
{
int i,x,c;
  for(x=0;x<SCROLLBUF_NUMTILESX;x++)
  {
      c = map.mapdata[x+mapx][mpy];
	  g_pGfxEngine->Tilemap->drawTile(g_pVideoDriver->getScrollSurface(), ((x<<4)+mapxstripepos)&511, y, c);

      if (AnimTileInUse[(((x<<4)+mapxstripepos)&511)>>4][y>>4])
      { // we just drew over an animated tile which we must unregister
        animtiles[AnimTileInUse[(((x<<4)+mapxstripepos)&511)>>4][y>>4]].slotinuse = 0;
        AnimTileInUse[(((x<<4)+mapxstripepos)&511)>>4][y>>4] = 0;
      }
      if ( TileProperty[c][ANIMATION] > 1 )
      { // we just drew an animated tile which we will now register

        for(i=1;i<MAX_ANIMTILES-1;i++)
        {
          if (!animtiles[i].slotinuse)
          {  // we found an unused slot
              animtiles[i].x = ((x<<4)+mapxstripepos)&511;
              animtiles[i].y = y;
              animtiles[i].baseframe = c - tiles[c].animOffset;
              animtiles[i].offset = tiles[c].animOffset;
              animtiles[i].slotinuse = 1;
              AnimTileInUse[(((x<<4)+mapxstripepos)&511)>>4][y>>4] = i;
              goto stop;            // and drop out of the loop
          }
        }
        stop: ;
      }
  }
}

// returns the map tile at map position (x,y)
unsigned int getmaptileat(unsigned int x, unsigned int y)
{
int xa = x>>4;
int ya = y>>4;

  if (xa<=255 && ya<=255)
  {
	  return map.mapdata[xa][ya];
  }
  else
  {
	  if(xa > 255)
		  xa = 255;

	  if(ya > 254)
		  ya = 255;

	  return map.mapdata[xa][ya];
  }
}
unsigned int getlevelat(unsigned int x, unsigned int y)
{
  return map.objectlayer[x>>4][y>>4];
}

// called at start of level to draw the upper-left corner of the map
// onto the scrollbuffer...from then on the map will only be drawn
// in stripes as it scrolls around.
void drawmap(void)
{
	int y;

    for(y=0;y<SCROLLBUF_NUMTILESY;y++)
      map_draw_hstripe(y<<4, y);
}

// changes the tile at (x,y) in real time
void map_chgtile(unsigned int x, unsigned int y, int newtile)
{
   map.mapdata[x][y] = newtile;

   if (x>=mapx && y>=mapy && x<mapx+64 && y<mapy+64)
   {
	   g_pGfxEngine->Tilemap->drawTile(g_pVideoDriver->getScrollSurface(),
			   ((mapxstripepos+((x-mapx)<<4))&511),
			   ((mapystripepos+((y-mapy)<<4))&511),
			   newtile);
   }

   map_redraw();
}

// searches for animated tiles at the map position (X,Y) and
// unregisters them from animtiles
void map_deanimate(int x, int y)
{
int px,py;
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
}

// tells if the object is animating at a given position
int map_isanimated(int x, int y)
{
int px,py;
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
      }

      return 0; // no
}

void map_unregister_all_animtiles(void)
{
int i;
  for(i=0;i<MAX_ANIMTILES-1;i++) animtiles[i].slotinuse = 0;
}

// after changing a map tile in real time with map_chgtile()
// from an unanimated to an animated tile, you must call this
// function to register the new animated tile.
void map_animate(int x, int y)
{
int px,py;
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
      why_term_ptr = "Unable to animate tile at x/y=crashflag1/2";
}

// searches the map's object layer for object OBJ.
// if it is found returns nonzero and places the
// coordinates of the first occurance of the object
// in (xout,yout)
char map_findobject(unsigned int obj, int *xout, int *yout)
{
unsigned int x,y;

  for(y=2;y<map.ysize-2;y++)
  {
    for(x=2;x<map.xsize-2;x++)
    {
      if (map.objectlayer[x][y]==obj)
      {
        *xout = x;
        *yout = y;
        return 1;
      }
    }
  }
  return 0;
}

// searches the map's tile layer for tile TILE.
// if it is found returns nonzero and places the
// coordinates of the first occurance of the tile
// in (xout,yout)
char map_findtile(unsigned int tile, int *xout, int *yout)
{
unsigned int x,y;

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
  }
  return 0;
}

// refreshes the map at the current scroll position
// (unlike drawmap() which does not honor the scroll and will
// glitch up if scrollx/y is != 0)
void map_redraw(void)
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
}
