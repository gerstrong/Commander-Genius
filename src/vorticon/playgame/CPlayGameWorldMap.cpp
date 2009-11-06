/*
 * CPlayGameWorldMap.cpp
 *
 *  Created on: 05.11.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "../CTeleporter.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CInput.h"
#include "../../sdl/sound/CSound.h"
#include "../../sdl/CVideoDriver.h"
#include "../../graphics/CGfxEngine.h"

void CPlayGame::processOnWorldMap()
{
	int useobject;

	// Perform player Objects...
	for( int i=0 ; i<m_NumPlayers ; i++ )
	{
		mp_Player[i].processWorldMap();

		// entered a level, used ship, teleporter, etc.
		useobject = mp_Player[i].getNewObject();
		if( useobject != 0 )
		{	// A new object was chosen by the player
			CTeleporter Teleporter(*mp_Map, m_Episode);

			// If it is teleporter, make the Player teleport
			if(Teleporter.readTeleporterInfo(useobject) == true)
			{
				Teleporter.teleportPlayer(m_Object, mp_Player[i]);
			}
			else
			{
				// If it is level, change the playgame mode and load the new map. Nessie is
				// a special case in Episode 3
				switch(useobject)
				{
					case NESSIE_PATH: break;
					case NESSIE_PAUSE: break;
					case NESSIE_MOUNTPOINT: break;

					case LVLS_SHIP:
						if (m_Episode==1)
						{
							//YourShipNeedsTheseParts(pCKP);
						}
						else
						{
							//ShipEp3(pCKP);
						}
						break;

					default:      // a regular level
						m_level_command = START_LEVEL;
						m_Level = useobject & 0x7fff;
						//g_pMusicPlayer->stop();
						g_pSound->playStereofromCoord(SOUND_ENTER_LEVEL, PLAY_NOW, m_Object[mp_Player[i].m_player_number].scrx);
						// save where on the map, the player entered. This is a checkpoint!
						m_checkpoint_x = mp_Player[i].x;
						m_checkpoint_y = mp_Player[i].y;
						m_checkpointset = true;
						cleanup();
						init();
						break;
				}
			}
		}

		// in episode 3 he can ride on nessie
		if (m_Episode==3)
		{
			mp_Player[i].AllowMountUnmountNessie();
		}
	}

	if(m_showKeensLeft)	showKeensLeft();
}

void CPlayGame::goBacktoMap()
{
	// before he can go back to map, he must tie up the objects.
	// This means, all objects except the puppy ones of the player....
	m_Object.clear();

	// Recreate the Players and tie them to the objects
	createPlayerObjects();

	m_level_command = START_LEVEL;
	m_Level = WM_MAP_NUM;
	//g_pMusicPlayer->stop();
	//g_pSound->playStereofromCoord(SOUND_ENTER_LEVEL, PLAY_NOW, m_Object[mp_Player[i].useObject].scrx);
	// Now that the new level/map will be loaded, the players aren't dead anymore!
	for( int i=0 ; i<m_NumPlayers ; i++ )
	{
		mp_Player[i].level_done = LEVEL_NOT_DONE;
		// Restore checkpoint
		mp_Player[i].x = mp_Player[i].goto_x = m_checkpoint_x;
		mp_Player[i].y = mp_Player[i].goto_y = m_checkpoint_y;
	}
	cleanup();
	init();
}


void CPlayGame::showKeensLeft()
{
int x,y,i,p;
int boxY, boxH;
SDL_Surface *boxsurface = g_pVideoDriver->FGLayerSurface;

	 #define KEENSLEFT_X        7
	 #define KEENSLEFT_Y        11
	 #define KEENSLEFT_W        24
	 #define KEENSLEFT_H        4

	 boxY = KEENSLEFT_Y - m_NumPlayers*2;
	 boxH = KEENSLEFT_H + m_NumPlayers*3;

	 if( g_pTimer->HasTimeElapsed(3000) || g_pInput->getPressedAnyCommand() )
		 m_showKeensLeft = false;

	 g_pGfxEngine->drawDialogBox( boxsurface, KEENSLEFT_X, boxY,KEENSLEFT_W,boxH);
	 g_pGfxEngine->Font->drawFont( boxsurface, getstring("LIVES_LEFT_BACKGROUND"),(KEENSLEFT_X+1)*8,(boxY+1)*8,0);
	 g_pGfxEngine->Font->drawFont( boxsurface, getstring("LIVES_LEFT"),((KEENSLEFT_X+7)*8)+4,(boxY+1)*8,0);

	 y = ((boxY+2)*8)+4;
	 for(p=0; p<m_NumPlayers ; p++)
	 {
		 x = ((KEENSLEFT_X+1)*8)+4;
		 for(i=0;i<mp_Player[0].inventory.lives&&i<=10;i++)
		 {
			 g_pGfxEngine->Sprite[mp_Player[0].playerbaseframe]->drawSprite(g_pVideoDriver->FGLayerSurface, x, y );
			 x+=16;
		 }
		 y += 32;
	 }
}

