/*
 * CPlayGameWorldMap.cpp
 *
 *  Created on: 05.11.2009
 *      Author: gerstrong
 */

#include "CPlayGameVorticon.h"
#include "../../../sdl/CTimer.h"
#include "../../../sdl/CInput.h"
#include "../../../sdl/sound/CSound.h"
#include "../../../sdl/CVideoDriver.h"
#include "../../../graphics/CGfxEngine.h"
#include "../../../graphics/effects/CColorMerge.h"
#include "../../../sdl/sound/CSound.h"
#include "../ai/CTeleporter.h"

void CPlayGameVorticon::processOnWorldMap()
{
	int useobject;

	// Perform player Objects...
	m_Player[0].processWorldMap();

	// entered a level, used ship, teleporter, etc.
	if( !m_Player[0].hideplayer && !m_Player[0].beingteleported )
	{
		useobject = m_Player[0].getNewObject();
		if( useobject != 0 )
		{
			// If it is teleporter, make the Player teleport
			int TeleportID;
			if( (TeleportID = getTeleporterInfo(useobject)) != 0 )
			{
				teleportPlayer(TeleportID, m_Player[0]);
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
					m_Player[0].MountNessieIfAvailable();
					g_pInput->flushAll();
					break;

				case LVLS_SHIP:
					if (m_Episode==1)
						YourShipNeedsTheseParts();
					else if (m_Episode==3)
						ShipEp3();

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
						g_pSound->playStereofromCoord(SOUND_ENTER_LEVEL, PLAY_NOW, m_Player[0].scrx);
						// save where on the map, the player entered. This is a checkpoint!
						m_checkpoint_x = m_Player[0].getXPosition();
						m_checkpoint_y = m_Player[0].getYPosition();
						m_checkpointset = true;
						cleanup();
						init();

						g_pGfxEngine->pushEffectPtr(pColorMergeFX);
					}
					break;
				}
			}
		}
	}

	if(m_Player[0].mounted)
	{
		if(g_pInput->getPressedAnyCommand(0))
		{
			m_Player[0].UnmountNessie();
			g_pInput->flushAll();
		}
	}

	if(m_showKeensLeft)	showKeensLeft();

}

void CPlayGameVorticon::goBacktoMap()
{
	// Create the special merge effect (Fadeout)
	CColorMerge *pColorMergeFX = new CColorMerge(8);

	// before he can go back to map, he must tie up the objects.
	// This means, all objects except the puppy ones of the player....
	m_Object.clear();

	m_level_command = START_LEVEL;
	m_Level = WM_MAP_NUM;
	g_pMusicPlayer->stop();
	// Now that the new level/map will be loaded, the players aren't dead anymore!
	std::vector<CPlayer>::iterator player= m_Player.begin();
	for( ; player != m_Player.end() ; player++ )
	{
		player->level_done = LEVEL_NOT_DONE;
		player->HealthPoints = 1;

		// Restore checkpoint
		player->moveToForce(m_checkpoint_x,m_checkpoint_y);
		player->inventory.HasCardYellow = 0;
		player->inventory.HasCardBlue = 0;
		player->inventory.HasCardGreen = 0;
		player->inventory.HasCardRed = 0;
	}
	cleanup();
	init();

	// Second Snapshot for merge
	g_pGfxEngine->pushEffectPtr(pColorMergeFX);
}

void CPlayGameVorticon::YourShipNeedsTheseParts()
{
	CMessageBox *MessageBox = new CMessageBox(g_pBehaviorEngine->getString("EP1_SHIP"));

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

void CPlayGameVorticon::ShipEp3()
{
	// get one of four random strings and display it!!
	std::string strname = "EP3_SHIP"+ itoa((rand()%4)+1);
	m_MessageBoxes.push_back(new CMessageBox(g_pBehaviorEngine->getString(strname)));
}

void CPlayGameVorticon::showKeensLeft()
{
	const unsigned int KEENSLEFT_X = 7;
	const unsigned int KEENSLEFT_Y = 10;

	if(!mp_KeenLeftSfc)
	{
		SDL_Surface *p_blitSurface = g_pVideoDriver->FGLayerSurface;
		const Uint32 rmask = p_blitSurface->format->Rmask;
		const Uint32 gmask = p_blitSurface->format->Gmask;
		const Uint32 bmask = p_blitSurface->format->Bmask;
		const Uint32 amask = p_blitSurface->format->Amask;
		const Uint8 bpp = p_blitSurface->format->BitsPerPixel;

		int x,y,i,p;
		int boxY, boxH;
		CFont &Font = g_pGfxEngine->getFont(0);

		const unsigned int KEENSLEFT_W = 24;
		const unsigned int KEENSLEFT_H = 4;

		boxY = KEENSLEFT_Y - m_NumPlayers;
		boxH = KEENSLEFT_H + m_NumPlayers*2;

		SDL_Rect rect;
		rect.x = (KEENSLEFT_X+1)*8;	rect.y = (boxY+2)*8;
		rect.w = (KEENSLEFT_W+1)*8;	rect.h = (boxH)*8;
		SDL_Surface *boxsurface = SDL_CreateRGBSurface(p_blitSurface->flags, rect.w, rect.h, bpp, rmask, gmask, bmask, amask);

		rect.x = 8;	rect.y = 16;
		rect.w = (KEENSLEFT_W-1)*8;	rect.h = (boxH-3)*8;
		g_pGfxEngine->drawDialogBox( boxsurface, 0, 0, KEENSLEFT_W, boxH, Font.getBGColour(true));
		SDL_FillRect(boxsurface, &rect, Font.getBGColour(false));
		Font.drawFont( boxsurface, g_pBehaviorEngine->getString("LIVES_LEFT"), 36, 8, true);

		y = 20;
		for(p=0; p<m_NumPlayers ; p++)
		{
			x = 12;
			for(i=0;i<m_Player[p].inventory.lives&&i<=10;i++)
			{
				g_pGfxEngine->getSprite(m_Player[p].playerbaseframe+PMAPDOWNFRAME).drawSprite(boxsurface, x, y );
				x+=16;
			}
			y += 16;
		}

		mp_KeenLeftSfc = SDL_DisplayFormat(boxsurface);
		SDL_FreeSurface(boxsurface);
	}
	else
	{
		SDL_Rect local_rect;
		local_rect.x = (KEENSLEFT_X+1)*8;
		local_rect.y = (KEENSLEFT_Y - m_NumPlayers + 2)*8;
		local_rect.w = mp_KeenLeftSfc->w;
		local_rect.h = mp_KeenLeftSfc->h;

		SDL_BlitSurface(mp_KeenLeftSfc, NULL, g_pVideoDriver->FGLayerSurface, &local_rect);

		if( g_pTimer->HasTimeElapsed(3000) || g_pInput->getPressedAnyCommand() )
		{
			m_showKeensLeft = false;
			SDL_FreeSurface(mp_KeenLeftSfc);
			mp_KeenLeftSfc = NULL;
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
	m_Map.findObject(objectID, &origx, &origy);
	CTeleporter *teleporter = new CTeleporter(&m_Map, m_Player,origx<<CSF, origy<<CSF);
	teleporter->solid = false;
	teleporter->direction = TELEPORTING_IN;
	if(m_Episode == 1)
		readTeleportDestCoordinatesEP1(objectID, destx, desty);
	else if(m_Episode == 3)
		readTeleportDestCoordinatesEP3(objectID, destx, desty);
	teleporter->destx = destx>>TILE_S;
	teleporter->desty = desty>>TILE_S;
	teleporter->whichplayer = player.m_index;
	m_Object.push_back(teleporter);
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
		if(m_Map.findObject(i, &destx, &desty))
		{
			destx <<= TILE_S;
			desty <<= TILE_S;
			return;
		}
	}
}




