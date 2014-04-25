/*
 * CVorticonMapLoader.cpp
 *
 *  Created on: 09.10.2009
 *      Author: gerstrong
 */

#include "CVorticonMapLoader.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>
#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CRLE.h"
#include "engine/core/CBehaviorEngine.h"
#include "graphics/GsGraphics.h"
#include "engine/core/CResourceLoader.h"
#include "fileio/Oldsavegamestructs.h"

#include "ai/CYorp.h"
#include "ai/CGarg.h"
#include "ai/CAutoRay.h"
#include "ai/CVorticon.h"
#include "ai/CManglingMachine.h"
#include "ai/CVortiKid.h"
#include "ai/CVorticonElite.h"
#include "ai/CVortiMom.h"
#include "ai/CVortiNinja.h"
#include "ai/CBallJack.h"
#include "ai/CButler.h"
#include "ai/CDoor.h"
#include "ai/CFoob.h"
#include "ai/CTank.h"
#include "ai/CGuardRobot.h"
#include "ai/CTeleporter.h"
#include "ai/CMessie.h"
#include "ai/CMeep.h"
#include "ai/CPlatform.h"
#include "ai/CRope.h"
#include "ai/CScrub.h"
#include "ai/CIceCannon.h"
#include "ai/CSpark.h"


CVorticonMapLoaderBase::CVorticonMapLoaderBase( std::shared_ptr<CMap> &map ) :
mpMap(map)
{}

CVorticonMapLoader::CVorticonMapLoader( std::shared_ptr<CMap> &map,
					std::vector<std::unique_ptr<CVorticonSpriteObject> > &spriteObjectContainer) :
CVorticonMapLoaderBase(map),
m_NessieAlreadySpawned(false),
mpSpriteObjectContainer(spriteObjectContainer)
{}

CVorticonMapLoaderWithPlayer::CVorticonMapLoaderWithPlayer(std::shared_ptr<CMap> &map,
			    std::vector<CPlayer> &playerContainer,
			    std::vector<std::unique_ptr<CVorticonSpriteObject> > &spriteObjectContainer) :
CVorticonMapLoader(map, spriteObjectContainer),
m_checkpointset(false),
mPlayerContainer(playerContainer)
{}



void CVorticonMapLoaderBase::blitPlaneToMap(std::vector<Uint16> &planeitems, const Uint16 planesize, const Uint16 planeID, const Uint16 tilemapID)
{
    unsigned int curmapx = 0, curmapy = 0;

    const unsigned int startOffest = planesize*planeID+17;

    // Some mods seem to incorrectly read the planes, so if there is no data left, just break wiht
    // this trick.
    const unsigned int realSize = planeitems.size()-startOffest;
    const unsigned int desiredSize = planesize;
    const unsigned int obtainedSize = std::min(realSize, desiredSize);

    for( size_t c=0 ; c<obtainedSize ; c++ ) // Check against Tilesize
    {
        int t = planeitems.at(startOffest+c);

        mpMap->setTile(curmapx, curmapy, t, false, tilemapID);

        curmapx++;
        if (curmapx >= mpMap->m_width)
        {
            curmapx = 0;
            curmapy++;
            if (curmapy >= mpMap->m_height) break;
        }

        /*if(t > 255)
        {
            t=0; // If there are some invalid values in the file, set them to zero.
        }*/
    }
}



bool CVorticonMapLoaderBase::loadBase(  Uint8 episode, 
					Uint8 level, 
					const std::string& path, 
					bool loadNewMusic )
{
    
	std::vector<Uint16> planeitems;
    
    std::string levelname = "level";
	if(level < 10) levelname += "0";
	levelname += itoa(level) + ".ck" + itoa(episode);

    mpMap->resetScrolls();
	mpMap->m_gamepath = path;
	mpMap->m_worldmap = (level == 80);

	// HQ Music. Load Music for a level if you have HQP
	if(loadNewMusic)
	{
		g_pMusicPlayer->stop();

		// If no music from the songlist could be loaded try the normal table which
		// has another format. It is part of HQP
		if(!g_pMusicPlayer->LoadfromSonglist(path, level))
			g_pMusicPlayer->LoadfromMusicTable(path, levelname);
	}

	// decompress map RLEW data
	std::ifstream MapFile;
	bool fileopen = OpenGameFileR(MapFile, getResourceFilename(levelname,path,true,false), std::ios::binary);

	if (!fileopen)
	{
		// only record this error message on build platforms that log errors
		// to a file and not to the screen.
		gLogging.ftextOut("MapLoader: unable to open file %s<br>", levelname.c_str());
		return false;
	}
	gLogging.ftextOut("MapLoader: file %s opened. Loading...<br>", levelname.c_str());

	MapFile.seekg (0, std::ios::beg);

	// load the compressed data into the memory
	std::vector<Uint8>	compdata;
	while( !MapFile.eof() )
	{
		compdata.push_back(static_cast<Uint8>(MapFile.get()));
	}

	MapFile.close();

	CRLE RLE;
    RLE.expandSwapped(planeitems, compdata, 0xFEFE);

	// Here goes the memory allocation function
	const Uint16 w =  planeitems.at(1);
	const Uint16 h =  planeitems.at(2);
	mpMap->createEmptyDataPlane(0, w, h);
	mpMap->createEmptyDataPlane(1, w, h);
	mpMap->createEmptyDataPlane(2, w, h);

	unsigned int planesize = 0;
	planesize = planeitems.at(8);
	planesize /= 2; // We have two planes

	blitPlaneToMap( planeitems, planesize, 0, 0);
	blitPlaneToMap( planeitems, planesize, 0, 1);
	blitPlaneToMap( planeitems, planesize, 1, 2);
	
	mpMap->collectBlockersCoordiantes();
	return true;
}

void CVorticonMapLoaderWithPlayer::loadSprites( Uint8 episode, 
						Uint8 level )
{
	if( !mpSpriteObjectContainer.empty() )
	    mpSpriteObjectContainer.clear();
	
	mpSpriteObjectContainer.reserve(2000);
	
	for( size_t curmapx = 0; curmapx<mpMap->m_width ; curmapx++ )
	{
        for( size_t curmapy = 0; curmapy<mpMap->m_height ; curmapy++ )
        {
            const size_t t = mpMap->getPlaneDataAt(2, curmapx<<CSF, curmapy<<CSF);

            if (mpMap->m_worldmap)
                addWorldMapObject(t, curmapx, curmapy,  episode );
            else
                addSpriteObject(t, curmapx, curmapy, episode, level);
        }

	}	
}


// Loads the map into the memory
bool CVorticonMapLoaderBase::load( Uint8 episode, 
				    Uint8 level, 
				    const std::string& path, 
				    bool loadNewMusic )
{	
	std::vector<Uint16> planeitems;
	
	if( !loadBase( episode, level, path, loadNewMusic) )
	{
	    return false;
	}
	
	// Set Map Delegation Object and refresh whole level
    mpMap->drawAll();
    gVideoDriver.updateScrollBuffer( mpMap->m_scrollx, mpMap->m_scrolly );

	return true;
}

bool CVorticonMapLoaderWithPlayer::load( Uint8 episode, 
                                         Uint8 level,
                                         const std::string& path,
                                         bool loadNewMusic,
                                         bool stategame )
{	
    if( !loadBase( episode, level, path, loadNewMusic ) )
    {
        return false;
    }

    if( !stategame )
    {
        loadSprites(episode, level );
    }

    // Set Map Delegation Object and refresh whole level
    mpMap->drawAll();
    gVideoDriver.updateScrollBuffer( mpMap->m_scrollx, mpMap->m_scrolly );

    return true;
}

void CVorticonMapLoaderWithPlayer::addWorldMapObject(unsigned int t, Uint16 x, Uint16 y, int episode)
{
    // This function add sprites on the map. Most of the objects are invisible.
    // TODO : Please convert this into ifs. There are more conditions than just switch.agree
    switch(t)
    {
    case 0: break;       // blank
    case 255:            // player start
        if(!m_checkpointset)
        {
            for( auto &player : mPlayerContainer )
            {
                player.exists = false;
                player.moveToForce(x<<CSF, y<<CSF);
            }
        }

        for( auto &player : mPlayerContainer )
        {
            player.setupforLevelPlay();
            player.solid = player.godmode;
        }

        break;
    case NESSIE_PATH:          // spawn nessie at first occurance of her path
        if (episode==3)
        {
            if (!m_NessieAlreadySpawned)
            {
                std::unique_ptr<CMessie> messie(new CMessie(mpMap.get(), x<<CSF, y<<CSF));
                m_NessieAlreadySpawned = true;
                mpSpriteObjectContainer.push_back(move(messie));
            }
        }
        break;
    default:             // level marker

        // Taken from the original CloneKeen. If hard-mode chosen, swap levels 5 and 9 Episode 1
        if(episode == 1 && g_pBehaviorEngine->mDifficulty >= HARD)
        {
            if(t == 5)
                t = 9;
            else if(t == 9)
                t = 5;
        }

        if(!mPlayerContainer.empty())
        {

            if ((t&0x7fff) <= MAX_LEVELS_VORTICON && mPlayerContainer.front().mp_levels_completed[t&0x00ff])
            {
                // Change the level tile to a done sign
                int newtile = g_pBehaviorEngine->getTileProperties()[mpMap->at(x,y)].chgtile;

                // Consistency check! Some Mods have issues with that.
                if(episode == 1 || episode == 2)
                {
                    //Use default small tile
                    newtile = 77;

                    // try to guess, if it is a 32x32 (4 16x16) Tile
                    if(mpMap->at(x-1,y-1) == (unsigned int) newtile &&
                            mpMap->at(x,y-1) == (unsigned int) newtile  &&
                            mpMap->at(x-1,y) == (unsigned int) newtile)
                    {
                        mpMap->setTile(x-1, y-1, 78);
                        mpMap->setTile(x, y-1, 79);
                        mpMap->setTile(x-1, y, 80);
                        newtile = 81; // br. this one
                    }
                }
                else if(episode == 3)
                {
                    newtile = 56;
                    // try to guess, if it is a 32x32 (4 16x16) Tile
                    if(mpMap->at(x-1, y-1) == (unsigned int) newtile &&
                            mpMap->at(x, y-1) == (unsigned int) newtile  &&
                            mpMap->at(x-1, y) == (unsigned int) newtile)
                    {
                        mpMap->setTile(x-1, y-1, 52);
                        mpMap->setTile(x, y-1, 53);
                        mpMap->setTile(x-1, y, 54);
                        newtile = 55;
                    }
                }
                mpMap->setTile(x, y, newtile);
            }
        }
        break;
	}
}

void CVorticonMapLoaderWithPlayer::addSpriteObject(unsigned int t, Uint16 x, Uint16 y, int episode, int level)
{
	if(t)
	{
		if (t==255) // The player in the level!
		{
			if(x >= mpMap->m_width-2) // Edge bug. Keen would fall in some levels without this.
				x = 4;

			if(y >= mpMap->m_height-2) // Edge bug. Keen would fall in some levels without this.
				y = 4;
			
			for( auto &player : mPlayerContainer )
			{
				player.exists = false;
				player.moveToForce(x<<CSF, y<<CSF);
				player.alignToTile();
				player.setupforLevelPlay();
			}
		}
		else
		{
			CVorticonSpriteObject *enemyobject = NULL;

			switch(t)
			{
			case 0: break;
			//case -1: break;
			case 1:  // yorp (ep1) vort (ep2&3)
				if (episode == 1)
				{
					enemyobject = new CYorp( mpMap.get(), x<<CSF, y<<CSF );
				}
				else if(episode == 2 || episode == 3)
				{
					enemyobject = new CVorticon( mpMap.get(), x<<CSF, y<<CSF);
				}
				break;
			case 2:    // garg (ep1) baby vorticon (ep2&3)
				if (episode == 1)
				{
					enemyobject = new CGarg( mpMap.get(), x<<CSF, y<<CSF );
				}
				else
				{
					enemyobject = new CVortikid( mpMap.get(), x<<CSF, y<<CSF );
				}

				break;
			case 3:    // vorticon (ep1) Vorticon Commander (ep2)
				if (episode==1)
				{
					CPhysicsSettings &Phy = g_pBehaviorEngine->getPhysicsSettings();

					size_t health = (level==16) ? Phy.vorticon.commander_hp : Phy.vorticon.default_hp;
					enemyobject = new CVorticon( mpMap.get(), x<<CSF, y<<CSF, health );
				}
				else if (episode==2)
				{
					enemyobject = new CVorticonElite( mpMap.get(), x<<CSF, y<<CSF);
				}
				else if (episode==3)
				{
					enemyobject = new CVortiMom( mpMap.get(), x<<CSF, y<<CSF);
				}
				break;
			case 4:    // butler (ep1) or scrub (ep2) or meep (ep3)
				if (episode==1)
					enemyobject = new CButler( mpMap.get(), x<<CSF, y<<CSF);
				else if (episode==2)
					enemyobject = new CScrub( mpMap.get(), x<<CSF, y<<CSF);
				else if (episode==3)
					enemyobject = new CMeep( mpMap.get(), x<<CSF, y<<CSF);
				break;
			case 5:    // tank robot (ep1&2) vorticon ninja (ep3)
				if (episode==1)
					enemyobject = new CTank( mpMap.get(), x<<CSF, y<<CSF);
				else if (episode==2)
					enemyobject = new CGuardRobot( mpMap.get(), x<<CSF, y<<CSF);
				else if (episode==3)
					enemyobject = new CVortiNinja( mpMap.get(), x<<CSF, y<<CSF);
				break;
			case 6:    // up-right-flying ice chunk (ep1) horiz platform (ep2)
				// foob (ep3)
				if (episode==1)
				{
					enemyobject = new CIceCannon( mpMap.get(), x<<CSF, y<<CSF, 1, -1 );
				}
				else if (episode==2)
				{
					enemyobject = new CPlatform( mpMap.get(),x<<CSF, (y<<CSF)-(4<<STC));
				}
				else if (episode==3)
				{
					enemyobject = new CFoob( mpMap.get(), x<<CSF, y<<CSF);
				}
				break;
			case 7:   // spark (ep2) ball (ep3) ice cannon upwards (ep1)
				if (episode==1)
				{
					enemyobject = new CIceCannon( mpMap.get(),x<<CSF, y<<CSF, 0, -1);
				}
				else if (episode==2)
				{
					enemyobject = new CSpark( mpMap.get(), x<<CSF, y<<CSF);
				}
				else if (episode==3)
				{
					enemyobject = new CBallJack( mpMap.get(), x<<CSF, y<<CSF, OBJ_BALL);
				}
				break;
			case 8:    // jack (ep3) and ice cannon down (ep1)
				if (episode==1)
				{
					enemyobject = new CIceCannon( mpMap.get(), x<<CSF, y<<CSF,0,1);
				}
				else if (episode==3)
				{
					enemyobject = new CBallJack( mpMap.get(), x<<CSF, y<<CSF, OBJ_JACK);
				}
				break;
			case 9:    // up-left-flying ice chunk (ep1) horiz platform (ep3)
				if (episode==1)
				{
					enemyobject = new CIceCannon( mpMap.get(), x<<CSF, y<<CSF,-1,-1);
				}
				else if (episode==3)
				{
					enemyobject = new CPlatform( mpMap.get(), x<<CSF, (y<<CSF)-(4<<STC));
				}
				break;
			case 10:   // rope holding the stone above the final vorticon (ep1)
				// vert platform (ep3)
				if (episode==1)
				{
					enemyobject = new CRope( mpMap.get(), x<<CSF, y<<CSF);
				}
				else if (episode==3)
				{
					enemyobject = new CPlatformVert( mpMap.get(), x<<CSF, y<<CSF);
				}
				break;
			case 11:   // jumping vorticon (ep3)
				if (episode==3)
				{
					enemyobject = new CVorticon( mpMap.get(), x<<CSF, y<<CSF );
				}
				break;
			case 12:   // sparks in mortimer's machine
				enemyobject = new CManglingMachine( mpMap.get(), x<<CSF, y<<CSF,SE_MORTIMER_SPARK);
				enemyobject->solid = false;
				break;
			case 13:   // mortimer's heart
				enemyobject = new CManglingMachine( mpMap.get(), x<<CSF, y<<CSF,SE_MORTIMER_HEART);
				enemyobject->solid = false;
				break;
			case 14:   // right-pointing raygun (ep3)
				enemyobject = new CAutoRay(mpMap.get(), x<<CSF, y<<CSF, CAutoRay::HORIZONTAL);
				break;
			case 15:   // vertical raygun (ep3)
				enemyobject = new CAutoRay(mpMap.get(), x<<CSF, y<<CSF, CAutoRay::VERTICAL);
				break;
			case 16:  // mortimer's arms
				enemyobject = new CManglingMachine( mpMap.get(), x<<CSF, y<<CSF, SE_MORTIMER_ARM );
				enemyobject->solid = false;
				break;
			case 17:  // mortimer's left leg
				enemyobject = new CManglingMachine( mpMap.get(), x<<CSF, y<<CSF, SE_MORTIMER_LEG_LEFT );
				enemyobject->solid = false;
				break;
			case 18:  // mortimer's right leg
				enemyobject = new CManglingMachine( mpMap.get(), x<<CSF, y<<CSF, SE_MORTIMER_LEG_RIGHT );
				enemyobject->solid = false;
				break;
			default:
				gLogging.ftextOut(PURPLE,"unknown enemy type %d at (%d,%d)<br>", t, x, y);
				break;
			}


			if(enemyobject != NULL)
			{
			    std::unique_ptr<CVorticonSpriteObject> obj(enemyobject);
			    mpSpriteObjectContainer.push_back(move(obj));
			}
		}
	}
}

/**
 * \brief As the original Commander Keen games have bugs, this function will fix them. it only will trigger.
 * \param currentTile	Number of the tile that will be modified to get the bug fixed.
 * \param curmapX		X-Coordinate of the map not CSFed
 * \param curmapY		Y-Coordinate of the map not CSFed
 * \param episode		Episode of the game
 */
/*void CVorticonMapLoaderBase::fixLevelTiles(int &currentTile, const Uint16 curmapX, const Uint16 curmapY, const int episode, const int level)
{
	if( episode == 1 && level == 14 )
	{
		if( (curmapX == 14 && curmapY == 10) || (curmapX == 13 && curmapY == 13) )
			currentTile = 143;
		else if( (curmapX == 14 && curmapY == 11) || (curmapX == 13 && curmapY == 14) )
			currentTile = 331;
	}
	else if( episode == 2 && level == 15 )
	{
		if( curmapX == 50 && curmapY == 18 )
			currentTile = 187;
	}
	else if( episode == 3 && level == 8 )
	{
		if( ( curmapX == 77 && curmapY == 52 ) ||
				( ( curmapX == 94 || curmapX == 95 || curmapX == 96 ) && curmapY == 15 ) )
			currentTile = 169;
	}
	else if( episode == 3 && level == 15 )
	{
		if( ( curmapX == 32 || curmapX == 33 ) && curmapY == 113 )
			currentTile = 482;
	}
}*/
