/*
 * CPlayGameWorldMap.cpp
 *
 *  Created on: 05.11.2009
 *      Author: gerstrong
 */

#include "CPlayGameVorticon.h"
#include <base/GsTimer.h>
#include <base/CInput.h>
#include <base/audio/Audio.h>
#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>
#include <graphics/CColorMerge.h>
#include <base/audio/Audio.h>
#include "engine/core/VGamepads/vgamepadsimple.h"
#include "../ai/CTeleporter.h"
#include "../ai/CMessie.h"

const int LVLS_SHIP = 20;

void CPlayGameVorticon::processOnWorldMap()
{            
    const int numPlayers = static_cast<int>(m_Player.size());
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
						
                        if( !mpLevelCompleted[useobject & 0x7fff] || gBehaviorEngine.mOptions[GameOption::LVLREPLAYABILITY].value )
						{
							// Create the special merge effect
							CColorMerge *pColorMergeFX = new CColorMerge(8);

							m_level_command = START_LEVEL;
							m_Level = useobject & 0x7fff;
							gMusicPlayer.stop();
                            player.playSound(GameSound::ENTER_LEVEL);
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


#ifdef USE_VIRTUALPAD
    if( gVideoDriver.VGamePadEnabled() )
    {
        VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
        assert(vkc);
        vkc->hideAllButtons();
        vkc->mDPad.invisible = false;
        vkc->mMenuButton.invisible = false;
        vkc->mStartButton.invisible = false;
    }
#endif

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
	gMusicPlayer.stop();
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

        const int varId = player->getSpriteVariantIdx();

        // Now, that the level is complete, sprite can be shown again, and now goto the world map!
		int width = player->w>>(CSF-4);

		if(width > 0)
		{
			int frame = player->playerbaseframe;
			if(gBehaviorEngine.getEpisode() == 3) frame++;

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
	std::unique_ptr<CMessageBoxVort> MessageBox( new CMessageBoxVort(gBehaviorEngine.getString("EP1_SHIP")) );

	bool joy, bat, vac, wis;
	joy = bat = vac = wis = false;

	// The Multiplayer support for this dialog. You collect those parts together if more than one player.
    for( auto &player : m_Player)
	{
        joy |= player.inventory.HasJoystick;
        bat |= player.inventory.HasBattery;
        vac |= player.inventory.HasVacuum;
        wis |= player.inventory.HasWiskey;
	}

	// draw needed parts
    if (!joy) MessageBox->addTileAt(321, 4<<3,  4<<3);
    if (!bat) MessageBox->addTileAt(322, 8<<3,  4<<3);
    if (!vac) MessageBox->addTileAt(323, 12<<3, 4<<3);
    if (!wis) MessageBox->addTileAt(324, 16<<3, 4<<3);
	mMessageBoxes.push_back(move(MessageBox));
}

void CPlayGameVorticon::ShipEp3()
{
	// get one of four random strings and display it!!
	std::string strname = "EP3_SHIP"+ itoa((rand()%4)+1);
	std::unique_ptr<CMessageBoxVort> msg( new CMessageBoxVort(gBehaviorEngine.getString(strname)) );
	mMessageBoxes.push_back( move(msg) );
}

void CPlayGameVorticon::showKeensLeft()
{    
    const int numPlayers = m_Player.size();

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    const int scW = gameRes.dim.x/320;
    const int scH = gameRes.dim.y/200;

    const unsigned int KEENSLEFT_X = 7*scW;
    const unsigned int KEENSLEFT_Y = 10*scH;

    if(mKeenLeftSfc.empty())
	{
		int x,y,i,p;
		int boxY, boxH;
		auto &Font = gGraphics.getFontLegacy(1);

		const unsigned int KEENSLEFT_W = 24;
		const unsigned int KEENSLEFT_H = 4;

        boxY = KEENSLEFT_Y - numPlayers;
        boxH = KEENSLEFT_H + numPlayers*2;

		SDL_Rect rect;
		rect.x = (KEENSLEFT_X+1)*8;	rect.y = (boxY+2)*8;
        rect.w = (KEENSLEFT_W+1)*8;	rect.h = (boxH)*8;

        mKeenLeftSfc.createRGBSurface(rect);

		rect.x = 8;	rect.y = 16;
        rect.w = (KEENSLEFT_W-1)*8;	rect.h = (boxH-3)*8;

		Uint8 r, g, b;
		Font.getBGColour(&r, &g, &b, true);
        const auto color = mKeenLeftSfc.mapRGB(r, g, b);

        gGraphics.drawDialogBox( mKeenLeftSfc.getSDLSurface(),
                                 0, 0,
                                 KEENSLEFT_W, boxH,
                                 color );
        mKeenLeftSfc.fillRGB(rect, r, g, b);

		Font.getBGColour(&r, &g, &b, false);

        mKeenLeftSfc.fillRGB(rect, r, g, b);

        Font.drawFont( mKeenLeftSfc, gBehaviorEngine.getString("LIVES_LEFT"), 36, 8, true);

		y = 20;
        for(p=0; p<numPlayers ; p++)
		{
			x = 12;
			for( i=0 ; i<m_Player[p].inventory.lives && i<=10 ; i++ )
            {
                GsSprite &livesSprite = gGraphics.getSprite(p,m_Player[p].playerbaseframe+pMapDOWNFRAME);

                const int liveW = livesSprite.getWidth();
                const int liveH = livesSprite.getHeight();
                livesSprite.drawSprite(mKeenLeftSfc.getSDLSurface(),
                                       x, y,
                                       liveW, liveH );

                x+=16;
			}
			y += 16;
		}

        mKeenLeftSfc.makeBlitCompatible();
	}
	else
	{
		keenleft_rect.x = (KEENSLEFT_X+1)*8;
        keenleft_rect.y = (KEENSLEFT_Y - numPlayers + 2)*8;
        keenleft_rect.w = mKeenLeftSfc.width();
        keenleft_rect.h = mKeenLeftSfc.height();


		if( gTimer.HasTimeElapsed(3000) || gInput.getPressedAnyCommand() )
		{
			m_showKeensLeft = false;
            mKeenLeftSfc.tryToDestroy();
			gInput.flushAll();
		}
		else
		{
            GsWeakSurface weakBlit(gVideoDriver.getBlitSurface());
            mKeenLeftSfc.blitTo(weakBlit, keenleft_rect);
		}
	}
}

int CPlayGameVorticon::getTeleporterInfo(int objectID)
{
    if(m_Episode == 1)
    {
		if( objectID > 33 && objectID < 47 ) return objectID;
	}
    else if(m_Episode == 3)
    {
		if( (objectID & 0xF00) == 0xF00) return objectID;
	}
	return 0;
}


/**
 * Teleporter part
 */
void CPlayGameVorticon::teleportPlayer(int objectID, CPlayer &player)
{
    int destx=0, desty=0;
    int origx=0, origy=0;
	mMap->findObject(objectID, &origx, &origy);
    std::unique_ptr<CTeleporter> teleporter( new CTeleporter( mMap, m_Player,origx<<CSF, origy<<CSF) );
	teleporter->solid = false;
	teleporter->direction = TELEPORTING_IN;
	if(m_Episode == 1)
    {
		readTeleportDestCoordinatesEP1(objectID, destx, desty);
    }
	else if(m_Episode == 3)
    {
		readTeleportDestCoordinatesEP3(objectID, destx, desty);
    }
	teleporter->destx = destx>>TILE_S;
	teleporter->desty = desty>>TILE_S;
	teleporter->whichplayer = player.m_index;
	mSpriteObjectContainer.push_back(move(teleporter));
}

void CPlayGameVorticon::readTeleportDestCoordinatesEP1(int objectID, int &destx, int &desty)
{
	destx = desty = 0;

	std::vector<stTeleporterTable>::iterator TTable =
			gBehaviorEngine.getTeleporterTable().begin();
	size_t i = 0;
	for( ; TTable != gBehaviorEngine.getTeleporterTable().end() ; TTable++, i++ )
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
