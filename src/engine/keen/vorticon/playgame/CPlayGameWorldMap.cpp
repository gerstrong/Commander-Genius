/*
 * CPlayGameWorldMap.cpp
 *
 *  Created on: 05.11.2009
 *      Author: gerstrong
 */

#include "CPlayGameVorticon.h"
#include <base/GsTimer.h>
#include <base/CInput.h>
#include "sdl/audio/Audio.h"
#include <base/video/CVideoDriver.h>
#include "graphics/GsGraphics.h"
#include "graphics/effects/CColorMerge.h"
#include "sdl/audio/Audio.h"
#include "../ai/CTeleporter.h"
#include "../ai/CMessie.h"

const int LVLS_SHIP = 20;

void CPlayGameVorticon::processOnWorldMap()
{
    const int numPlayers = m_Player.size();
    for( int i=0 ; i<numPlayers ; i++ )
	{
		CPlayer &player = m_Player[i];

		// Perform player Objects...
		player.processWorldMap();
		
		if(m_showKeensLeft)
		  break;

		// entered a level, used ship, teleporter, etc.
		if( !player.hideplayer && !player.beingteleported )
		{
			int useobject = player.getNewObject();
			if( useobject != 0 )
			{
				// If it is teleporter, make the Player teleport
				int TeleportID;
				if( (TeleportID = getTeleporterInfo(useobject)) != 0 )
				{
					teleportPlayer(TeleportID, player);
				}
				else
				{
					// If it is level, change the playgame mode and load the new map. Nessie is
					// a special case in Episode 3
					switch(useobject)
					{
					case NESSIE_WEED:
					case NESSIE_PATH: break;
					case NESSIE_LAND:
						gEventManager.add(new CPlayer::Mount(player));
						gInput.flushAll();
						break;

					case LVLS_SHIP:
					{
						if (m_Episode==1)
							YourShipNeedsTheseParts();
						else if (m_Episode==3)
							ShipEp3();

						gInput.flushCommands();

					}
					break;

					default: // a regular level
						// Check if Level has been completed or the Level-Replayability is enabled
						if( useobject>32 )
						    break;
						
						if( !mpLevelCompleted[useobject & 0x7fff] || mp_option[OPT_LVLREPLAYABILITY].value )
						{
							// Create the special merge effect
							CColorMerge *pColorMergeFX = new CColorMerge(8);

							m_level_command = START_LEVEL;
							m_Level = useobject & 0x7fff;
							g_pMusicPlayer->stop();
							player.playSound(SOUND_ENTER_LEVEL);
							// save where on the map, the player entered. This is a checkpoint!
							m_checkpoint_x = player.getXPosition();
							m_checkpoint_y = player.getYPosition();
							m_checkpointset = true;
							cleanup();
							init();

                            gEffectController.setupEffect(pColorMergeFX);
						}
						break;
					}
				}
			}
		}
	}
}

void CPlayGameVorticon::goBacktoMap()
{
	// Create the special merge effect (Fadeout)
	CColorMerge *pColorMergeFX = new CColorMerge(8);

	// before he can go back to map, he must tie up the objects.
	// This means, all objects except the puppy ones of the player....
	mSpriteObjectContainer.clear();

	m_level_command = START_LEVEL;
	m_Level = WM_MAP_NUM;
	g_pMusicPlayer->stop();
	// Now that the new level/map will be loaded, the players aren't dead anymore!
	std::vector<CPlayer>::iterator player= m_Player.begin();
	for( ; player != m_Player.end() ; player++ )
	{
		player->level_done = LEVEL_NOT_DONE;
		player->mHealthPoints = 1;

		// Restore checkpoint
		player->moveToForce(m_checkpoint_x,m_checkpoint_y);
		player->inventory.HasCardYellow = 0;
		player->inventory.HasCardBlue = 0;
		player->inventory.HasCardGreen = 0;
		player->inventory.HasCardRed = 0;

        const int varId = player->getSpriteVariantId();

        // Now, that the level is complete, sprite can be shown again, and now goto the world map!
		int width = player->w>>(CSF-4);

		if(width > 0)
		{
			int frame = player->playerbaseframe;
			if(g_pBehaviorEngine->getEpisode() == 3) frame++;

            gGraphics.getSprite(varId,frame+0).setWidth(width);
            gGraphics.getSprite(varId,frame+1).setWidth(width);
            gGraphics.getSprite(varId,frame+2).setWidth(width);
            gGraphics.getSprite(varId,frame+3).setWidth(width);
		}


	}
	cleanup();
	init();

	// Second Snapshot for merge
    gEffectController.setupEffect(pColorMergeFX);
}

void CPlayGameVorticon::YourShipNeedsTheseParts()
{
	std::unique_ptr<CMessageBoxVort> MessageBox( new CMessageBoxVort(g_pBehaviorEngine->getString("EP1_SHIP")) );

	bool joy, bat, vac, wis;
	joy = bat = vac = wis = false;

	// The Multiplayer support for this dialog. You collect those parts together if more than one player.
    const int numPlayers = g_pBehaviorEngine->mPlayers;
    for(int i=0 ; i<numPlayers ; i++)
	{
		joy |= m_Player[i].inventory.HasJoystick;
		bat |= m_Player[i].inventory.HasBattery;
		vac |= m_Player[i].inventory.HasVacuum;
		wis |= m_Player[i].inventory.HasWiskey;
	}

	// draw needed parts
    if (!joy) MessageBox->addTileAt(321,4<<3, 4<<3);
    if (!bat) MessageBox->addTileAt(322,8<<3, 4<<3);
    if (!vac) MessageBox->addTileAt(323,12<<3,4<<3);
    if (!wis) MessageBox->addTileAt(324,16<<3,4<<3);
	mMessageBoxes.push_back(move(MessageBox));
}

void CPlayGameVorticon::ShipEp3()
{
	// get one of four random strings and display it!!
	std::string strname = "EP3_SHIP"+ itoa((rand()%4)+1);
	std::unique_ptr<CMessageBoxVort> msg( new CMessageBoxVort(g_pBehaviorEngine->getString(strname)) );
	mMessageBoxes.push_back( move(msg) );
}

void CPlayGameVorticon::showKeensLeft()
{    
    const int numPlayers = g_pBehaviorEngine->mPlayers;

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    const int scW = gameRes.w/320;
    const int scH = gameRes.h/200;

    const unsigned int KEENSLEFT_X = 7*scW;
    const unsigned int KEENSLEFT_Y = 10*scH;

	if(!mpKeenLeftSfc)
	{
		int x,y,i,p;
		int boxY, boxH;
		GsFont &Font = gGraphics.getFont(1);

		const unsigned int KEENSLEFT_W = 24;
		const unsigned int KEENSLEFT_H = 4;

        boxY = KEENSLEFT_Y - numPlayers;
        boxH = KEENSLEFT_H + numPlayers*2;

		SDL_Rect rect;
		rect.x = (KEENSLEFT_X+1)*8;	rect.y = (boxY+2)*8;
		rect.w = (KEENSLEFT_W+1)*8;	rect.h = (boxH)*8;
		SDL_Surface *boxsurface = SDL_CreateRGBSurface( SDL_SWSURFACE, rect.w, rect.h, RES_BPP, 0, 0, 0, 0 );

		rect.x = 8;	rect.y = 16;
		rect.w = (KEENSLEFT_W-1)*8;	rect.h = (boxH-3)*8;

		Uint8 r, g, b;
		Font.getBGColour(&r, &g, &b, true);
		Uint32 color = SDL_MapRGB( boxsurface->format, r, g, b);

		gGraphics.drawDialogBox( boxsurface, 0, 0, KEENSLEFT_W, boxH, color );
		SDL_FillRect(boxsurface, &rect, color );
		Font.getBGColour(&r, &g, &b, false);
		SDL_FillRect(boxsurface, &rect, SDL_MapRGB( boxsurface->format, r, g, b) );
		Font.drawFont( boxsurface, g_pBehaviorEngine->getString("LIVES_LEFT"), 36, 8, true);


		y = 20;
        for(p=0; p<numPlayers ; p++)
		{
			x = 12;
			for( i=0 ; i<m_Player[p].inventory.lives && i<=10 ; i++ )
            {
                GsSprite &livesSprite = gGraphics.getSprite(p,m_Player[p].playerbaseframe+PMAPDOWNFRAME);

                const int liveW = livesSprite.getWidth();
                const int liveH = livesSprite.getHeight();
                livesSprite.drawSprite(boxsurface, x, y, liveW, liveH );
				x+=16;
			}
			y += 16;
		}

		const SDL_Surface *blit = gVideoDriver.mpVideoEngine->getBlitSurface();
		mpKeenLeftSfc.reset(SDL_ConvertSurface( boxsurface, blit->format, blit->flags ), &SDL_FreeSurface);
		SDL_FreeSurface(boxsurface);
	}
	else
	{
		keenleft_rect.x = (KEENSLEFT_X+1)*8;
        keenleft_rect.y = (KEENSLEFT_Y - numPlayers + 2)*8;
		keenleft_rect.w = mpKeenLeftSfc->w;
		keenleft_rect.h = mpKeenLeftSfc->h;


		if( gTimer.HasTimeElapsed(3000) || gInput.getPressedAnyCommand() )
		{
			m_showKeensLeft = false;
			mpKeenLeftSfc.reset();
			gInput.flushAll();
		}
		else
		{
            BlitSurface(mpKeenLeftSfc.get(), nullptr, gVideoDriver.getBlitSurface(), &keenleft_rect);
		}
	}
}

int CPlayGameVorticon::getTeleporterInfo(int objectID)
{
	if(m_Episode == 1) {
		if( objectID > 33 && objectID < 47 ) return objectID;
	}
	else if(m_Episode == 3) {
		if( (objectID & 0xF00) == 0xF00) return objectID;
	}
	return 0;
}


/**
 * Teleporter part
 */
void CPlayGameVorticon::teleportPlayer(int objectID, CPlayer &player)
{
	int destx, desty;
	int origx, origy;
	mMap->findObject(objectID, &origx, &origy);
	std::unique_ptr<CTeleporter> teleporter( new CTeleporter( mMap.get(), m_Player,origx<<CSF, origy<<CSF) );
	teleporter->solid = false;
	teleporter->direction = TELEPORTING_IN;
	if(m_Episode == 1)
		readTeleportDestCoordinatesEP1(objectID, destx, desty);
	else if(m_Episode == 3)
		readTeleportDestCoordinatesEP3(objectID, destx, desty);
	teleporter->destx = destx>>TILE_S;
	teleporter->desty = desty>>TILE_S;
	teleporter->whichplayer = player.m_index;
	mSpriteObjectContainer.push_back(move(teleporter));
}

void CPlayGameVorticon::readTeleportDestCoordinatesEP1(int objectID, int &destx, int &desty)
{
	destx = desty = 0;

	std::vector<stTeleporterTable>::iterator TTable =
			g_pBehaviorEngine->getTeleporterTable().begin();
	size_t i = 0;
	for( ; TTable != g_pBehaviorEngine->getTeleporterTable().end() ; TTable++, i++ )
	{
		if(TTable->objectnumber2 == objectID || TTable->objectnumber1 == objectID)
		{
			destx = TTable->x;
			desty = TTable->y;
			break;
		}
	}
}

void CPlayGameVorticon::readTeleportDestCoordinatesEP3(int objectID, int &destx, int &desty)
{
	destx = desty = 0;

	int newObject = objectID & 0x00F;
	newObject <<= 4;
	newObject += 0xF00; // Now its a teleporter, we only need to find the right one on the map

	for(int i=newObject; i<newObject+0x10 ; i++)
	{
		if(mMap->findObject(i, &destx, &desty))
		{
			destx <<= TILE_S;
			desty <<= TILE_S;
			return;
		}
	}
}
