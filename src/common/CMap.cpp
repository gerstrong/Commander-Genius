/*
 * CMap.cpp
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#include "CMap.h"
#include <iostream>
#include <fstream>
#include "../FindFile.h"
#include "../CLogFile.h"
#include "../include/fileio/rle.h"
#include "../sdl/CVideoDriver.h"

std::string formatPathString(const std::string& path);

CMap::CMap(SDL_Surface *p_scrollsurface, CTilemap *p_Tilemap) {
	 m_width = m_height = 0;
	 m_worldmap = false;

	 m_scrollx = m_scrolly = 0;

	 m_scrollx_buf = m_scrolly_buf = 0;

	 m_scrollpix = m_scrollpixy = 0;
	 m_mapx = m_mapy = 0;           // map X location shown at scrollbuffer row 0
	 m_mapxstripepos = m_mapystripepos = 0;  // X pixel position of next stripe row

	 mp_scrollsurface = p_scrollsurface;
	 mp_Tilemap = p_Tilemap;
	 mp_data = NULL;
}

CMap::~CMap() {
	if(mp_data) delete[] mp_data;
}

////////////////////////////
// Initialization Routine //
////////////////////////////
// Loads the map into the memory
bool CMap::loadMap( Uint8 episode, Uint8 level, const std::string& path )
{
	int t;
	Uint32 c=0;
	int numruns = 0;
	int resetcnt, resetpt;
	unsigned int planesize = 0;
	unsigned int curmapx=0, curmapy=0;

	std::string buffer = formatPathString(path);
	std::string fname;
	fname = buffer + "level";
	if(level < 10) fname += "0";
	fname += itoa(level) + ".ck" + itoa(episode);

	std::ifstream MapFile; OpenGameFileR(MapFile, fname, std::ios::binary);

	m_worldmap = (level == 80);

	if (!MapFile)
	{
		// only record this error message on build platforms that log errors
		// to a file and not to the screen.
		g_pLogFile->ftextOut("loadmap(): unable to open file %s<br>", fname.c_str());
		return false;
	}
	g_pLogFile->ftextOut("loadmap(): file %s opened. Loading...<br>", fname.c_str());

    // decompress map RLEW data
	std::vector<Uint16> filebuf;
	unsigned long finsize; 		   // Final size

	MapFile.seekg (0, std::ios::beg);
	finsize = unRLEW(MapFile, filebuf);

	if(finsize == 0) // The data is already decompressed. Read it normally
	{
		Uint16 value = 0;
		Uint8 high_byte, low_byte;

		MapFile.seekg(0);
		MapFile.clear();

		while(!MapFile.eof())
		{
			low_byte = MapFile.get();
			high_byte = MapFile.get();
			value = (high_byte<<8) | low_byte;

			filebuf.push_back( value );
			c++;
		}
	}

	m_width = filebuf[2];
	m_height = filebuf[3];

	Uint32 mapsize;

	mapsize = ((m_width+32)*(m_height+32));
	mp_data = new Uint16[mapsize];

	memset(mp_data,0,mapsize*sizeof(Uint16));


	if ( !mp_data ) // Is this necessary ?
	{
		g_pLogFile->textOut(RED,"loadmap(): not enought memory to load map<br>");
		return false;
	}

	planesize = filebuf[9];
	planesize /= 2; // Size of two planes, but we only need one

	for( c=18 ; c<planesize+18 ; c++ ) // Check against Tilesize
	{
		t = filebuf[c];

		addTile(t, curmapx, curmapy);

		curmapx++;
		if (curmapx >= m_width)
		{
			curmapx = 0;
			curmapy++;
			if (curmapy >= m_height) break;
		}

		if(t > 255)
			t=0; // If there are some invalid values in the file
	}

	// now do the sprites
	// get sprite data
	curmapx = curmapy = numruns = 0;
    resetcnt = resetpt = 0;

    for( c=planesize+18 ; c<2*planesize+18 ; c++ )
    {
    	t = filebuf[c];

        //if (m_worldmap) addobjectlayertile(t,  episode,  levels_completed);
        //else addenemytile(t, episode, chglevelto);

        curmapx++;
        if (curmapx >= m_width)
        {
          curmapx = 0;
          curmapy++;
          if (curmapy >= m_height) break;
        }

        if (++resetcnt==resetpt) curmapx=curmapy=0;
    }
    filebuf.clear();
    MapFile.close();

    return true;
}

void CMap::addTile( Uint16 t, Uint16 x, Uint16 y )
{
	// Special cases. Those happen normally, when levels are replayed.
	// For example if one player has battery, the level won't show that item
	/*for(unsigned int cp=0 ; cp<numplayers ; cp++)
	{
		if( (TileProperty[t][BEHAVIOR] == 11 && player[cp].inventory.HasJoystick) ||
			(TileProperty[t][BEHAVIOR] == 12 && player[cp].inventory.HasBattery) ||
			(TileProperty[t][BEHAVIOR] == 13 && player[cp].inventory.HasVacuum) ||
			(TileProperty[t][BEHAVIOR] == 14 && player[cp].inventory.HasWiskey) ||
			(TileProperty[t][BEHAVIOR] == 16 && player[cp].inventory.HasPogo)	)
				t = tiles[t].chgtile; // is it a battery!!
	}*/

	// Now set this this tile at pos(curmapx, curmapy)
	setTile(x, y, t);
}

/////////////////////////
// Getters and Setters //
/////////////////////////
Uint16 CMap::at(Uint16 x, Uint16 y)
{
	return mp_data[y*m_width + x];
}

bool CMap::setTile(Uint16 x, Uint16 y, Uint16 t)
{
	if( x<m_width && y<m_height )
	{
		mp_data[y*m_width + x] = t;
		return true;
	}
	else
		return false;
}

////
// Scrolling Routines
////
bool CMap::gotoPos(Uint16 x, Uint16 y)
{
	if((m_width<<4) < x || (m_height<<4) < y )
		return false;
	else
	{
		int dx,dy;
		dx = x - m_scrollx;
		dy = y - m_scrolly;

		if(dx > 0)
			for(int scrollx=0 ; scrollx<dx ;scrollx++) scrollRight();
		else if(dx < 0)
			for(int scrollx=0 ; scrollx>dx ;scrollx--) scrollLeft();

		if(dy > 0)
			for(int scrolly=0 ; scrolly<dy ;scrolly++) scrollDown();
		else if(dy < 0)
			for(int scrolly=0 ; scrolly>dy ;scrolly--) scrollUp();
		return true;
	}
}

// scrolls the map one pixel right
void CMap::scrollRight(void)
{
	m_scrollx++;
     if(m_scrollx_buf>=511) m_scrollx_buf=0; else m_scrollx_buf++;

     m_scrollpix++;
     if (m_scrollpix>15)
     {  // need to draw a new stripe
       drawVstripe(m_mapxstripepos, m_mapx + 32);
       m_mapx++;
       m_mapxstripepos += 16;
       if (m_mapxstripepos >= 512) m_mapxstripepos = 0;
       m_scrollpix = 0;
     }
}

// scrolls the map one pixel left
void CMap::scrollLeft(void)
{
	m_scrollx--;
     if(m_scrollx_buf==0) m_scrollx_buf=511; else m_scrollx_buf--;

     if (m_scrollpix==0)
     {  // need to draw a new stripe
    	 m_mapx--;
       if (m_mapxstripepos == 0)
       {
    	   m_mapxstripepos = (512 - 16);
       }
       else
       {
    	   m_mapxstripepos -= 16;
       }
       drawVstripe(m_mapxstripepos, m_mapx);

       m_scrollpix = 15;
     } else m_scrollpix--;
}

void CMap::scrollDown(void)
{
  m_scrolly++;
  if(m_scrolly_buf>=511) m_scrolly_buf=0; else m_scrolly_buf++;

  m_scrollpixy++;
     if (m_scrollpixy>15)
     {  // need to draw a new stripe
       drawHstripe(m_mapystripepos, m_mapy + 32);
       m_mapy++;
       m_mapystripepos += 16;
       if (m_mapystripepos >= 512) m_mapystripepos = 0;
       m_scrollpixy = 0;
     }
}

void CMap::scrollUp(void)
{
  m_scrolly--;
  if(m_scrolly_buf==0) m_scrolly_buf=511; else m_scrolly_buf--;

     if (m_scrollpixy==0)
     {  // need to draw a new stripe
    	 m_mapy--;
       if (m_mapystripepos == 0)
       {
    	   m_mapystripepos = (512 - 16);
       }
       else
       {
    	   m_mapystripepos -= 16;
       }
       drawHstripe(m_mapystripepos, m_mapy);

       m_scrollpixy = 15;
     } else m_scrollpixy--;
}

//////////////////////
// Drawing Routines //
//////////////////////
// Draws the entire map to the scroll buffer
// called at start of level to draw the upper-left corner of the map
// onto the scrollbuffer...from then on the map will only be drawn
// in stripes as it scrolls around.
void CMap::drawAll()
{
	int y;
	int num_h_tiles= mp_scrollsurface->h/16;

    for(y=0;y<num_h_tiles;y++)
    {
      drawHstripe(y<<4, y);
    }
}

// draw a horizontal stripe, for vertical scrolling
void CMap::drawHstripe(unsigned int y, unsigned int mpy)
{
int x,c;
int num_v_tiles= mp_scrollsurface->w/16;

  for(x=0;x<num_v_tiles;x++)
  {
      c = mp_data[mpy*m_width + x+m_mapx];

	  mp_Tilemap->drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511, y, c);
	  mp_Tilemap->registerAnimation( ((x<<4)+m_mapxstripepos)&511, y, c );
  }
}

// draws a vertical stripe from map position mapx to scrollbuffer position x
void CMap::drawVstripe(unsigned int x, unsigned int mpx)
{
int y,c;
int num_h_tiles= mp_scrollsurface->h/16;
  for(y=0;y<num_h_tiles;y++)
  {
      c = mp_data[(y+m_mapy)*m_width + mpx];
      mp_Tilemap->drawTile(mp_scrollsurface, x, ((y<<4)+m_mapystripepos)&511, c);
      mp_Tilemap->registerAnimation( x, ((y<<4)+m_mapxstripepos)&511, c );
  }
}
