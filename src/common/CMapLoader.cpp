/*
 * CMapLoader.cpp
 *
 *  Created on: 09.10.2009
 *      Author: gerstrong
 */

#include "CMapLoader.h"
#include "../keen.h"
#include "../vorticon/ai/se.h"
#include "../game.h"
#include <iostream>
#include <fstream>
#include "../FindFile.h"
#include "../CLogFile.h"
#include "../fileio.h"
#include "../include/fileio/rle.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CVideoDriver.h"

CMapLoader::CMapLoader(CMap* p_map, std::vector<CPlayer> *p_PlayerVect) :
mp_vec_Player(p_PlayerVect)
{
	mp_objvect = NULL;
	mp_map = p_map;
	m_checkpointset = false;
	m_NessieAlreadySpawned = false;
}

// Loads the map into the memory
bool CMapLoader::load( Uint8 episode, Uint8 level, const std::string& path, bool loadNewMusic )
{
	int t;
	Uint32 c=0;
	int numruns = 0;
	int resetcnt, resetpt;
	unsigned int planesize = 0;
	unsigned int curmapx=0, curmapy=0;
	
	std::string buffer = formatPathString(path);
	std::string levelname;
	levelname = "level";
	if(level < 10) levelname += "0";
	levelname += itoa(level) + ".ck" + itoa(episode);
	std::string fname = buffer + levelname;
	
	std::ifstream MapFile; OpenGameFileR(MapFile, fname, std::ios::binary);
	
	mp_map->resetScrolls();
	mp_map->m_gamepath = path;
	mp_map->m_worldmap = (level == 80);
	
	// HQ Music. Load Music for a level if you have HQP
	if(loadNewMusic)
	{
		g_pMusicPlayer->stop();
		g_pMusicPlayer->LoadfromMusicTable(path, levelname);
	}

	if (!MapFile)
	{
		// only record this error message on build platforms that log errors
		// to a file and not to the screen.
		g_pLogFile->ftextOut("MapLoader: unable to open file %s<br>", fname.c_str());
		return false;
	}
	g_pLogFile->ftextOut("MapLoader: file %s opened. Loading...<br>", fname.c_str());
	
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
	
	mp_map->m_width = filebuf[2];
	mp_map->m_height = filebuf[3];
	
	Uint32 mapsize;
	mapsize = ((mp_map->m_width+32)*(mp_map->m_height+32));

	if(mp_map->mp_data)	{
		delete mp_map->mp_data;
	}
	mp_map->mp_data = new Uint16[mapsize];
	
	memset(mp_map->mp_data,0,mapsize*sizeof(Uint16));
	
	
	if ( !mp_map->mp_data ) // Is this necessary ?
	{
		g_pLogFile->textOut(RED,"loadmap(): not enough memory to load the map<br>");
		return false;
	}
	
	planesize = filebuf[9];
	planesize /= 2; // Size of two planes, but we only need one
	
	for( c=18 ; c<planesize+18 ; c++ ) // Check against Tilesize
	{
		t = filebuf[c];
		
		addTile(t, curmapx, curmapy);
		
		curmapx++;
		if (curmapx >= mp_map->m_width)
		{
			curmapx = 0;
			curmapy++;
			if (curmapy >= mp_map->m_height) break;
		}
		
		if(t > 255)
			t=0; // If there are some invalid values in the file
	}
	
	// now do the sprites
	// get sprite data
	curmapx = curmapy = numruns = 0;
    resetcnt = resetpt = 0;
	
	if(mp_objvect)
	{
		for( c=planesize+18 ; c<2*planesize+18 ; c++ )
		{
			t = filebuf[c];
			
			if (mp_map->m_worldmap) addWorldMapObject(t, curmapx, curmapy,  episode );
			else addEnemyObject(t, curmapx, curmapy, episode, level);

			curmapx++;
			if (curmapx >= mp_map->m_width)
			{
				curmapx = 0;
				curmapy++;
				if (curmapy >= mp_map->m_height) break;
			}
			
			if (++resetcnt==resetpt) curmapx=curmapy=0;
		}
	}
    filebuf.clear();
    MapFile.close();
	
    // Do some post calculations
    // Limit the scroll screens so the blocking (blue in EP1) tiles are3 never seen
    SDL_Rect gamerect = g_pVideoDriver->getGameResolution();
    mp_map->m_maxscrollx = (mp_map->m_width<<4) - gamerect.w - 36;
    mp_map->m_maxscrolly = (mp_map->m_height<<4) - gamerect.h - 36;

    // Set Scrollbuffer
    g_pVideoDriver->setScrollBuffer(&mp_map->m_scrollx_buf, &mp_map->m_scrolly_buf);
	
    return true;
}

void CMapLoader::addTile( Uint16 t, Uint16 x, Uint16 y )
{
	// Special cases. Those happen normally, when levels are replayed.
	// For example if one player has battery, the level won't show that item
	
	// Now set this this tile at pos(curmapx, curmapy)
	mp_map->setTile(x, y, t);
}

void CMapLoader::addWorldMapObject(unsigned int t, Uint16 x, Uint16 y, int episode)
{
	// This function add sprites on the map. Most of the objects are invisible.
	// TODO : Please convert this into ifs. There are more conditions than just switch.agree
	switch(t)
	{
		case 0: break;       // blank
		case 255:            // player start
			if(!m_checkpointset)
			{
				std::vector<CPlayer>::iterator it_player = mp_vec_Player->begin();
				for(; it_player != mp_vec_Player->end() ; it_player++ )
				{
					it_player->goto_x = it_player->x = x << CSF;
					it_player->goto_y = it_player->y = y << CSF;
				}
				mp_map->m_objectlayer[x][y] = 0;
			}
			break;
		case NESSIE_PATH:          // spawn nessie at first occurance of her path
			if (episode==3)
			{
				if (!m_NessieAlreadySpawned)
				{
					CObject nessie;

					nessie.setIndex(mp_objvect->size());
					nessie.spawn(x<<CSF, y<<CSF, OBJ_NESSIE, 3);
					nessie.onscreen = true;
					m_NessieAlreadySpawned = true;
					mp_objvect->push_back(nessie);
				}
				mp_map->m_objectlayer[x][y] = NESSIE_PATH;
			}
			break;
		default:             // level marker
			std::vector<CPlayer>::iterator it_player = mp_vec_Player->begin();
			for(; it_player != mp_vec_Player->end() ; it_player++ )
			{
				if ((t&0x7fff) <= 16 && it_player->mp_levels_completed[t&0x00ff] )
				{
					mp_map->m_objectlayer[x][y] = t;

					// Change the level tile to a done sign
					int newtile = g_pGfxEngine->Tilemap->mp_tiles[mp_map->at(x,y)].chgtile;

					// Consistency check! Some Mods have issues with that.
					if(episode == 1 || episode == 2)
					{
						//Use default small tile
						newtile = 77;

						// try to guess, if it is a 32x32 (4 16x16) Tile
						if(mp_map->at(x-1,y-1) == (unsigned int) newtile &&
								mp_map->at(x,y-1) == (unsigned int) newtile  &&
								mp_map->at(x-1,y) == (unsigned int) newtile)
						{
							mp_map->setTile(x-1, y-1, 78);
							mp_map->setTile(x, y-1, 79);
							mp_map->setTile(x-1, y, 80);
							newtile = 81; // br. this one
						}
					}
					else if(episode == 3)
					{
						newtile = 56;
						// try to guess, if it is a 32x32 (4 16x16) Tile
						if(mp_map->at(x-1, y-1) == (unsigned int) newtile &&
								mp_map->at(x, y-1) == (unsigned int) newtile  &&
								mp_map->at(x-1, y) == (unsigned int) newtile)
						{
							mp_map->setTile(x-1, y-1, 52);
							mp_map->setTile(x, y-1, 53);
							mp_map->setTile(x-1, y, 54);
							newtile = 55;
						}
					}
					mp_map->setTile(x, y, newtile);
				}
				else
				{
					mp_map->m_objectlayer[x][y] = t;
				}
			}
			break;
	}
}

void CMapLoader::addEnemyObject(unsigned int t, Uint16 x, Uint16 y, int episode, int level)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
	mp_map->m_objectlayer[x][y] = t;
	
	if (t)
	{
		if (t==255) // The player in the level!
		{
			if(x >= mp_map->m_width-2) // Edge bug. Keen would fall in some levels without this.
				x = 4;
			
			if(y >= mp_map->m_height-2) // Edge bug. Keen would fall in some levels without this.
				x = 4;

			std::vector<CPlayer>::iterator it_player = mp_vec_Player->begin();
			for(; it_player != mp_vec_Player->end() ; it_player++ )
			{
				it_player->goto_x = it_player->x = (x<<CSF);
				it_player->goto_y = it_player->y = (y<<CSF);
				it_player->plastfalling = true;
				it_player->pfalling = true;
				it_player->pshowdir = RIGHT;
			}
		}
		else
		{
			CObject enemyobject;
			enemyobject.setIndex(mp_objvect->size());
			
			switch(t)
			{
				case 0: break;
				case -1: break;
				case 1:  // yorp (ep1) vort (ep2&3)
					if (episode==1)
					{
						if ( TileProperty[mp_map->at(x ,y+1)].bleft ) x--;
						enemyobject.spawn(x<<CSF, y<<CSF, OBJ_YORP, episode);
						break;
					}
					else
					{
						// in ep2 level 16 there a vorticon embedded in the floor for
						// some reason! that's what the if() is for--to fix it.
						// I believe, that the rest of the vorticons are supposed to fall!
						enemyobject.spawn(x<<CSF, (y-1)<<CSF, OBJ_VORT, episode);
					 }
					 break;
				case 2:    // garg (ep1) baby vorticon (ep2&3)
					 if (episode==1)
					 {
						 // those bastards. sometimes embedding garg's in the floor in
						 // the original maps.
						 if(TileProperty[mp_map->at(x+1, y+1)].bleft)
						 {
							 if (level==7)
								 enemyobject.spawn(x<<CSF, (y-1)<<CSF, OBJ_GARG, episode);
							 else
								 enemyobject.spawn((x-1)<<CSF, y<<CSF, OBJ_GARG, episode);
						 }
						 else
							 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_GARG, episode);
					 }
					 else
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_BABY, episode);

						 break;
				case 3:    // vorticon (ep1) Vorticon Commander (ep2)
						 if (episode==1)
							 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_VORT, episode);
						 else if (episode==2)
							 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_VORTELITE, episode);
						 else if (episode==3)
							 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_MOTHER, episode);
						 break;
				case 4:    // butler (ep1) OR scrub (ep2) OR meep (ep3)
					 if (episode==1)
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_BUTLER, episode);
					 else if (episode==2)
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_SCRUB, episode);
					 else if (episode==3)
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_MEEP, episode);
					 break;
				case 5:    // tank robot (ep1&2) vorticon ninja (ep3)
					 if (episode==1)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_TANK, episode);
						 // set tank robot guarding bonus level to be active at startup
						 if (level==13)
							 enemyobject.hasbeenonscreen = true;
					 }
					 else if (episode==2)
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_TANKEP2, episode);
					 else if (episode==3)
					 {
						 if(TileProperty[mp_map->at(x,y+1)].bleft)
							 enemyobject.spawn(x<<CSF, (y-1)<<CSF, OBJ_NINJA, episode);
						 else
							 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_NINJA, episode);
					 }
					 break;
				case 6:    // up-right-flying ice chunk (ep1) horiz platform (ep2)
					 // foob (ep3)
					 if (episode==1)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_ICECANNON, episode);
						 enemyobject.ai.icechunk.vector_x = 1;
						 enemyobject.ai.icechunk.vector_y = -1;
					 }
					 else if (episode==2)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_PLATFORM, episode);
					 }
					 else if (episode==3)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_FOOB, episode);
					 }
					 break;
				case 7:   // spark (ep2) ball (ep3)
					 if (episode==2)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF,OBJ_SPARK, episode);
					 }
					 else if (episode==3)
					 {
						 enemyobject.spawn(x<<CSF,y<<CSF,OBJ_BALL, episode);
						 enemyobject.hasbeenonscreen = 1;
					 }
					 break;
				case 8:    // jack (ep3)
					 if (episode==3)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF,OBJ_JACK, episode);
						 enemyobject.hasbeenonscreen = 1;
					 }
					 break;
				case 9:    // up-left-flying ice chunk (ep1) horiz platform (ep3)
					 if (episode==1)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_ICECANNON, episode);
						 enemyobject.ai.icechunk.vector_x = -1;
						 enemyobject.ai.icechunk.vector_y = -1;
					 }
					 else if (episode==3)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_PLATFORM, episode);
					 }
					 break;
				case 10:   // rope holding the stone above the final vorticon (ep1)
					 // vert platform (ep3)
					 if (episode==1)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_ROPE, episode);
					 }
					 else if (episode==3)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_PLATVERT, episode);
					 }
					 break;
				case 11:   // jumping vorticon (ep3)
					 if (episode==3)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_VORT, episode);
					 }
					 break;
				case 12:   // sparks in mortimer's machine
					 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_SECTOREFFECTOR, episode);
					 enemyobject.ai.se.type = SE_MORTIMER_SPARK;
					 enemyobject.hasbeenonscreen = 1;
					 break;
				case 13:   // mortimer's heart
					 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_SECTOREFFECTOR, episode);
					 enemyobject.ai.se.type = SE_MORTIMER_HEART;
					 enemyobject.hasbeenonscreen = 1;
					 break;
				case 14:   // right-pointing raygun (ep3)
					 if (episode==3)
					 {
						 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_AUTORAY, episode);
					 }
					 break;
				case 15:   // vertical raygun (ep3)
					 if (episode==3)
					 {
						enemyobject.spawn(x<<CSF, y<<CSF, OBJ_AUTORAY_V, episode);
					 }
					 break;
				case 16:  // mortimer's arms
					 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_SECTOREFFECTOR, episode);
					 enemyobject.ai.se.type = SE_MORTIMER_ARM;
					 enemyobject.hasbeenonscreen = 1;
					 break;
				case 17:  // mortimer's left leg
					 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_SECTOREFFECTOR, episode);
					 enemyobject.ai.se.type = SE_MORTIMER_LEG_LEFT;
					 enemyobject.hasbeenonscreen = 1;
					 break;
				case 18:  // mortimer's right leg
					 enemyobject.spawn(x<<CSF, y<<CSF, OBJ_SECTOREFFECTOR, episode);
					 enemyobject.ai.se.type = SE_MORTIMER_LEG_RIGHT;
					 enemyobject.hasbeenonscreen = 1;
					 break;
				default:
					g_pLogFile->ftextOut(PURPLE,"unknown enemy type %d at (%d,%d)<br>", t, x, y); break;
			}
			mp_objvect->push_back(enemyobject);
		}
	}
}

CMapLoader::~CMapLoader() {
	// TODO Auto-generated destructor stub
}

