/*
 * CPlayGameWorldMap.cpp
 *
 *  Created on: 05.11.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CInput.h"
#include "../../sdl/sound/CSound.h"
#include "../../sdl/CVideoDriver.h"
#include "../../graphics/CGfxEngine.h"
#include "../../graphics/effects/CColorMerge.h"

void CPlayGame::processOnWorldMap()
{
	int useobject;

	// Perform player Objects...
	for( int i=0 ; i<m_NumPlayers ; i++ )
	{
		m_Player[i].processWorldMap();

		// entered a level, used ship, teleporter, etc.
		if( !m_Player[i].hideplayer && !m_Player[i].beingteleported )
		{
			useobject = m_Player[i].getNewObject();
			if( useobject != 0 )
			{
				// A new object was chosen by the player
				CTeleporter Teleporter(m_TeleporterTable, m_Episode);

				// If it is teleporter, make the Player teleport
				int TeleportID;
				if( (TeleportID = Teleporter.getTeleporterInfo(useobject)) != 0 )
				{
					Teleporter.teleportPlayer(TeleportID, m_Map,m_Object, m_Player[i]);
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
						m_Player[i].MountNessieIfAvailable();
						g_pInput->flushAll();
						break;

					case LVLS_SHIP:
						if (m_Episode==1)
						{
							YourShipNeedsTheseParts();
						}
						else if (m_Episode==3)
						{
							ShipEp3();
						}
						break;

					default: // a regular level
					// Check if Level has been completed or the Level-Replayability is enabled
						if( !mp_level_completed[useobject & 0x7fff] || mp_option[OPT_LVLREPLAYABILITY].value )
						{
							// Create the special merge effect
							CColorMerge *pColorMergeFX = new CColorMerge(8);

							m_level_command = START_LEVEL;
							m_Level = useobject & 0x7fff;
							g_pMusicPlayer->stop();
							g_pSound->playStereofromCoord(SOUND_ENTER_LEVEL, PLAY_NOW, m_Player[i].scrx);
							// save where on the map, the player entered. This is a checkpoint!
							m_checkpoint_x = m_Player[i].getXPosition();
							m_checkpoint_y = m_Player[i].getYPosition();
							m_checkpointset = true;
							cleanup();
							init();

							// Second Snapshot for merge
							g_pGfxEngine->pushEffectPtr(pColorMergeFX);
						}
						break;
					}
				}
			}

			// Check Collisions and only move player, if it is not blocked
			//checkPlayerCollisions(&m_Player[i]);
		}

		if(m_Player[i].mounted)
		{
			if(g_pInput->getPressedAnyCommand(i))
			{
				m_Player[i].UnmountNessie();
				g_pInput->flushAll();
			}
		}


		if(m_showKeensLeft)	showKeensLeft();
	}
}

void CPlayGame::goBacktoMap()
{
	// Create the special merge effect (Fadeout)
	CColorMerge *pColorMergeFX = new CColorMerge(8);

	// before he can go back to map, he must tie up the objects.
	// This means, all objects except the puppy ones of the player....
	m_Object.clear();

	// Recreate the Players and tie them to the objects
	//createPlayerObjects();

	m_level_command = START_LEVEL;
	m_Level = WM_MAP_NUM;
	g_pMusicPlayer->stop();
	// Now that the new level/map will be loaded, the players aren't dead anymore!
	for( int i=0 ; i<m_NumPlayers ; i++ )
	{
		m_Player[i].level_done = LEVEL_NOT_DONE;

		// Restore checkpoint
		m_Player[i].moveToForce(m_checkpoint_x,m_checkpoint_y);
		m_Player[i].inventory.HasCardYellow = 0;
		m_Player[i].inventory.HasCardBlue = 0;
		m_Player[i].inventory.HasCardGreen = 0;
		m_Player[i].inventory.HasCardRed = 0;
	}
	cleanup();
	init();

	// Second Snapshot for merge
	g_pGfxEngine->pushEffectPtr(pColorMergeFX);
}

void CPlayGame::YourShipNeedsTheseParts()
{
	CMessageBox *MessageBox = new CMessageBox(getstring("EP1_SHIP"));

	bool joy, bat, vac, wis;
	joy = bat = vac = wis = false;

	// The Multiplayer support for this dialog. You collect those parts together if more than one player.
	for(int i=0 ; i<m_NumPlayers ; i++)
	{
		joy |= m_Player[i].inventory.HasJoystick;
		bat |= m_Player[i].inventory.HasBattery;
		vac |= m_Player[i].inventory.HasVacuum;
		wis |= m_Player[i].inventory.HasWiskey;
	}

	// draw needed parts
	if (!joy) MessageBox->addTileAt(321,5<<3, 4<<3);
	if (!bat) MessageBox->addTileAt(322,8<<3, 4<<3);
	if (!vac) MessageBox->addTileAt(323,11<<3,4<<3);
	if (!wis) MessageBox->addTileAt(324,14<<3,4<<3);
	m_MessageBoxes.push_back(MessageBox);
}

void CPlayGame::ShipEp3()
{
	// get one of four random strings and display it!!
	std::string strname = "EP3_SHIP"+ itoa((rand()%4)+1);
	m_MessageBoxes.push_back(new CMessageBox(getstring(strname)));
}

void CPlayGame::showKeensLeft()
{
	int x,y,i,p;
	int boxY, boxH;
	SDL_Surface *boxsurface = g_pVideoDriver->FGLayerSurface;

#define KEENSLEFT_X        7
#define KEENSLEFT_Y        11
#define KEENSLEFT_W        24
#define KEENSLEFT_H        3

	boxY = KEENSLEFT_Y - m_NumPlayers*2;
	boxH = KEENSLEFT_H + m_NumPlayers*4;

	if( g_pTimer->HasTimeElapsed(3000) || g_pInput->getPressedAnyCommand() )
		m_showKeensLeft = false;

	SDL_Rect rect;
	rect.x = (KEENSLEFT_X+1)*8;	rect.y = (boxY+2)*8;
	rect.w = (KEENSLEFT_W-1)*8;	rect.h = (boxH-3)*8;
	g_pGfxEngine->drawDialogBox( boxsurface, KEENSLEFT_X, boxY,KEENSLEFT_W,boxH, SDL_MapRGB(boxsurface->format, 172, 172, 172));
	g_pGfxEngine->getFont().drawFont( boxsurface, getstring("LIVES_LEFT"),((KEENSLEFT_X+4)*8)+4,(boxY+1)*8, LETTER_TYPE_RED);
	SDL_FillRect(boxsurface, &rect, 0xFFFFFF);

	y = ((boxY+2)*8)+4;
	for(p=0; p<m_NumPlayers ; p++)
	{
		x = ((KEENSLEFT_X+1)*8)+4;
		for(i=0;i<m_Player[p].inventory.lives&&i<=10;i++)
		{
			g_pGfxEngine->getSprite(m_Player[p].playerbaseframe).drawSprite(g_pVideoDriver->FGLayerSurface, x, y );
			x+=16;
		}
		y += 32;
	}
}

