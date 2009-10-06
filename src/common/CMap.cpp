/*
 * CMap.cpp
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "CMap.h"
#include <iostream>
#include <fstream>
#include "../FindFile.h"
#include "../CLogFile.h"
#include "../include/fileio/rle.h"
#include "../sdl/CVideoDriver.h"

std::string formatPathString(const std::string& path);

CMap::CMap(SDL_Surface *p_scrollsurface, CTilemap *p_Tilemap, CPlayer *p_Player) {
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
	 mp_Player = p_Player;
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

        //if (m_worldmap) addObjectlayerTile(t,  episode,  levels_completed);
    	if (m_worldmap) addWorldMapObject(t, curmapx, curmapy,  episode,  NULL);
        //else addEnemyObject(t, curmapx, curmapy, episode, chglevelto);

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

	// Now set this this tile at pos(curmapx, curmapy)
	setTile(x, y, t);
}

//bool NessieAlreadySpawned;
void CMap::addWorldMapObject(unsigned int t, Uint16 x, Uint16 y, int episode, bool *levels_completed)
{
  //int o;
  switch(t)
  {
   case 0: break;       // blank
   case 255:            // player start
	   mp_Player[0].x = x << CSF;
	   mp_Player[0].y = y << CSF;
	   m_objectlayer[x][y] = 0;
     break;
   /*case NESSIE_PATH:          // spawn nessie at first occurance of her path
     if (episode==3)
     {
       if (!NessieAlreadySpawned)
       {
         o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_NESSIE);
         objects[o].hasbeenonscreen = 1;
         NessieAlreadySpawned = 1;
         NessieObjectHandle = o;
       }
     }
     goto levelmarker;
   break;*/
   default:             // level marker
//levelmarker: ;
     /*if ((t&0x7fff) < 256 && levels_completed[t&0x00ff])
     {
    	 if(!options[OPT_LVLREPLAYABILITY].value)
    		m_objectlayer[x][y] = 0;
    	 else
    		 m_objectlayer[x][y] = t;

    	 int newtile = tiles[map.mapdata[x][y]].chgtile;

    	 // Consistency check! Some Mods have issues with that.
    	 if(episode == 1 || episode == 2)
    	 {
    		 if(newtile<77 || newtile>81)
    			 // something went wrong. Use default tile
    			 newtile = 77;

    		 // try to guess, if it is a 32x32 (4 16x16) Tile
    		 if(mp_data[x-1][y-1] == (unsigned int) newtile &&
    				 mp_data[x][y-1] == (unsigned int) newtile  &&
    				 mp_data[x-1][y] == (unsigned int) newtile)
    		 {
    			 mp_data[x-1][y-1] = 78; //ul
    			 mp_data[x][y-1] = 79; //ur
    			 mp_data[x-1][y] = 80; //bl
    			 newtile = 81; // br. this one
    		 }
    	 }
    	 else if(episode == 3)
    	 {
    		 if(newtile<52 || newtile>56)
    			 // something went wrong. Use default tile
    			 newtile = 56;
    		 // try to guess, if it is a 32x32 (4 16x16) Tile
    		 if(mp_data[x-1][y-1] == (unsigned int) newtile &&
    				 mp_data[x][y-1] == (unsigned int) newtile  &&
    				 mp_data[x-1][y] == (unsigned int) newtile)
    		 {
    			 mp_data[x-1][y-1] = 52; //bl
    			 mp_data[x][y-1] = 53; //ur
    			 mp_data[x-1][y] = 54; //ul
    			 newtile = 55; // br. this one
    		 }
    	 }
 		mp_data[x][y] = newtile;
     }
     else
     {*/
       m_objectlayer[x][y] = t;
     //}
     break;
  }
}

void CMap::addEnemyObject(unsigned int t, Uint16 x, Uint16 y, int episode, int chglevelto)
{
//int o,x;
  m_objectlayer[x][y] = t;

  if (t)
  {
    if (t==255)
    {
        if(x >= m_width-2) // Edge bug. Keen would fall in some levels without this.
      	   x = 4;

        if(y >= m_height-2) // Edge bug. Keen would fall in some levels without this.
      	   x = 4;

       mp_Player[0].x = x << 9;
       mp_Player[0].y = ((y << 4) + 8) << 5;
    }
    /*else
    {
      switch(t)
      {
      case 0: break;
      case -1: break;
      case 1:  // yorp (ep1) vort (ep2&3)
           if (episode==1)
           {
              x = curmapx;

              if (TileProperty[map.mapdata[x][curmapy+1]][BLEFT]) x--;
              spawn_object(x<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_YORP);
           }
           else
           {
              // in ep2 level 16 there a vorticon embedded in the floor for
              // some reason! that's what the if() is for--to fix it.
        	  // TODO: Is this still needed?
        	  if (TileProperty[map.mapdata[curmapx][curmapy+1]][BLEFT])
              {
                spawn_object(curmapx<<4<<CSF, ((curmapy<<4)-16)<<CSF, OBJ_VORT);
              }
              else
              {
                spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_VORT);
              }
           }
           break;
      case 2:    // garg (ep1) baby vorticon (ep2&3)
           if (episode==1)
           {
             // those bastards. sometimes embedding garg's in the floor in
             // the original maps.
        	 if(TileProperty[map.mapdata[curmapx+1][curmapy+1]][BLEFT])
             {
               if (chglevelto==7)
               {
                 spawn_object(curmapx<<4<<CSF, (curmapy-1)<<4<<CSF, OBJ_GARG);
               }
               else
               {
                 spawn_object((curmapx-1)<<4<<CSF, (curmapy)<<4<<CSF, OBJ_GARG);
               }
             }
             else
             {
               spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_GARG);
             }
           }
           else
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_BABY);
           }
           break;
      case 3:    // vorticon (ep1) Vorticon Commander (ep2)
           if (episode==1)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_VORT);
           }
           else if (episode==2)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_VORTELITE);
           }
           else if (episode==3)
           {
              spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_MOTHER);
           }
           break;
      case 4:    // butler (ep1) OR scrub (ep2) OR meep (ep3)
           if (episode==1)
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_BUTLER);
           else if (episode==2)
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SCRUB);
           else if (episode==3)
             spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_MEEP);
           break;
      case 5:    // tank robot (ep1&2) karate bear (ep3)
           if (episode==1)
           {
              o = spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_TANK);
              // set tank robot guarding bonus level to be active at startup
              if (chglevelto==13)
              {
                objects[o].hasbeenonscreen = 1;
              }
           }
           else if (episode==2)
              spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+0)<<CSF, OBJ_TANKEP2);
           else if (episode==3)
           {
        	  if(TileProperty[map.mapdata[curmapx][curmapy+1]][BLEFT])
              {
                spawn_object(curmapx<<4<<CSF, (curmapy-1)<<4<<CSF, OBJ_NINJA);
              }
              else
              {
                spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_NINJA);
              }
           }
           break;
      case 6:    // up-right-flying ice chunk (ep1) horiz platform (ep2)
                 // foob (ep3)
           if (episode==1)
           {
              o = spawn_object((((curmapx+1)<<4)+4)<<CSF, ((curmapy<<4)-4)<<CSF, OBJ_ICECANNON);
              objects[o].ai.icechunk.vector_x = 1;
              objects[o].ai.icechunk.vector_y = -1;
           }
           else if (episode==2)
           {
              o = spawn_object(curmapx<<4<<CSF, ((curmapy<<4)-3)<<CSF, OBJ_PLATFORM);
           }
           else if (episode==3)
           {
              o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_FOOB);
           }
           break;
      case 7:   // spark (ep2) ball (ep3)
           if (episode==2)
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_SPARK);
           }
           else if (episode==3)
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_BALL);
              objects[o].hasbeenonscreen = 1;
           }
           break;
      case 8:    // jack (ep3)
           if (episode==3)
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_JACK);
              objects[o].hasbeenonscreen = 1;
           }
           break;
      case 9:    // up-left-flying ice chunk (ep1) horiz platform (ep3)
           if (episode==1)
           {
             o = spawn_object(((curmapx<<4)-4)<<CSF, ((curmapy<<4)-4)<<CSF, OBJ_ICECANNON);
             objects[o].ai.icechunk.vector_x = -1;
             objects[o].ai.icechunk.vector_y = -1;
           }
           else if (episode==3)
           {
             o = spawn_object(curmapx<<4<<CSF, (((curmapy)<<4)-4)<<CSF, OBJ_PLATFORM);
           }
           break;
      case 10:   // rope holding the stone above the final vorticon (ep1)
                 // vert platform (ep3)
           if (episode==1)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_ROPE);
           }
           else if (episode==3)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_PLATVERT);
           }
           break;
      case 11:   // jumping vorticon (ep3)
           if (episode==3)
           {
             spawn_object(curmapx<<4<<CSF, ((curmapy<<4)-8)<<CSF, OBJ_VORT);
           }
           break;
      case 12:   // sparks in mortimer's machine
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_SPARK;
          objects[o].hasbeenonscreen = 1;
          break;
      case 13:   // mortimer's heart
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_HEART;
          objects[o].hasbeenonscreen = 1;
          break;
      case 14:   // right-pointing raygun (ep3)
           if (episode==3)
           {
             o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_AUTORAY);
           }
           break;
      case 15:   // vertical raygun (ep3)
           if (episode==3)
           {
        	   o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_AUTORAY_V);
           }
           break;
      case 16:  // mortimer's arms
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_ARM;
          objects[o].hasbeenonscreen = 1;
      break;
      case 17:  // mortimer's left leg
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_LEG_LEFT;
          objects[o].hasbeenonscreen = 1;
      break;
      case 18:  // mortimer's right leg
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_LEG_RIGHT;
          objects[o].hasbeenonscreen = 1;
      break;
      default:
           g_pLogFile->ftextOut(PURPLE,"unknown enemy type %d at (%d,%d)<br>", t, curmapx, curmapy);
           break;
      }
    }*/
  }
}

/////////////////////////
// Getters and Setters //
/////////////////////////
// return the tile which is set at the given coordinates
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
