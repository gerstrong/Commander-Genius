/*
 * CPlayGame.cpp
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 * See CPlayGame.h for more information
 */

#include "CPlayGame.h"
#include "../sdl/CTimer.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"
#include "../graphics/CGfxEngine.h"

////
// Creation Routine
////
CPlayGame::CPlayGame( char episode, char level, 
			char numplayers, char difficulty,
			std::string &gamepath ) {
	m_Episode = episode;
	m_Level = level;
	m_NumPlayers = numplayers;
	m_Difficulty = difficulty;
	m_level_command = GOTO_WORLD_MAP;
	m_Gamepath = gamepath;
	m_exitgame = false;
	m_endgame = false;
	mp_Map = NULL;
	mp_Menu = NULL;

	// Create the Player
	if(m_NumPlayers != 0)
		mp_Player = new CPlayer[m_NumPlayers];
	else
		mp_Player = NULL;
}

bool CPlayGame::init()
{
	// load level map
	mp_Map = new CMap( g_pVideoDriver->getScrollSurface(), g_pGfxEngine->Tilemap, mp_Player );

	if( !mp_Map ) return false;
	if( !mp_Map->loadMap( m_Episode, m_Level, m_Gamepath ) ) return false;

	//// If those worked fine, continue the initialization

	// draw level map
	mp_Map->drawAll();

	// Now Scroll to the position of the player and center him
	mp_Map->gotoPos( (mp_Player[0].x>>5) - 160, (mp_Player[0].y>>5) - 100 );


	return true;
}

bool CPlayGame::loadGameState( std::string &statefile )
{
	// TODO: Add Code here!
	return false;
}

////
// Process Routine
////
void CPlayGame::process()
{
	// Handle Inputs

	/// The following function must be worldmap dependent
	if( m_Level == WORLD_MAP_LEVEL )
	{
		// Perform wm AIs

		// Check if player(s)...
		
		// entered level 
		
		// used teleport

		// in episode 3 rides on nessie 
	
	}
	else
	{
		// Perform AIs

		// Perform physics

		/// Check if player(s)...
		
		// finished the level

		// gets to bonus level

		// lost a life

		// end(s) the game.
	}

	// Animate the tiles of the map
	g_pGfxEngine->Tilemap->animateAllTiles(g_pVideoDriver->ScrollSurface);

	// Blit the background
	g_pVideoDriver->blitScrollSurface(mp_Map->m_scrollx_buf, mp_Map->m_scrolly_buf);

	// Open the Main Menu if ESC Key pressed and mp_Menu not opened
	if(!mp_Menu && g_pInput->getPressedKey(KQUIT))
	{
		// Open the menu
		mp_Menu = new CMenu( CMenu::ACTIVE );
		mp_Menu->init();
	}

	// If the menu is open process it!
	if(mp_Menu)
	{
		if( mp_Menu->mustBeClosed() || mp_Menu->getExitEvent() || mp_Menu->mustEndGame() )
		{
			if( mp_Menu->getExitEvent() )
				m_exitgame = true;

			if( mp_Menu->mustEndGame() )
				m_endgame = true;

			mp_Menu->cleanup();
			delete mp_Menu;
			mp_Menu = NULL;
		}
		else
		{
			mp_Menu->process();
		}
	}
}

////
// Cleanup Routine
////
void CPlayGame::cleanup()
{
	delete mp_Map;
	mp_Map = NULL;
}

CPlayGame::~CPlayGame() {
	if(mp_Player) delete [] mp_Player;
}

/////
// Old depreciated functions
// Will be removed soon!
/////

// Prototypes needed for playgame_levelmanager
/*int eseq2_TantalusRay(stCloneKeenPlus *pCKP);
void eseq2_vibrate();

void playgame_levelmanager(stCloneKeenPlus *pCKP)
{
  int i, o, wm, firsttime = 1;
  char levelname[80];
  int newlevel;

  stLevelControl *p_levelcontrol = &(pCKP->Control.levelcontrol);

  if( p_levelcontrol->command != LVLC_START_LEVEL )
  {
	  p_levelcontrol->command = LVLC_CHANGE_LEVEL;
	  p_levelcontrol->chglevelto = WM_MAP_NUM;
  }
  p_levelcontrol->tobonuslevel = 0;
  p_levelcontrol->success = 0;
  map.firsttime = 1;

  do
  {
		g_pGfxEngine->Palette.setFadeColour(SDL_MapRGB(g_pVideoDriver->FXSurface->format, 0, 0, 0));
		g_pGfxEngine->Palette.fadeto(255, FADE_SPEED_FAST);
		do
		{
			if( g_pTimer->TimeToRender() == false ) continue;
			g_pGfxEngine->Palette.applyFade();
			//g_pVideoDriver->sb_blit();
			g_pVideoDriver->update_screen();
			g_pTimer->TimeToDelay();
		} while(g_pGfxEngine->Palette.in_progress());

		initgame( &(pCKP->Control.levelcontrol) );

		newlevel = p_levelcontrol->chglevelto;
		if(p_levelcontrol->hardmode)
		{
			g_pGfxEngine->Palette.setdarkness(FADE_DARKNESS_HARD);
			if ( p_levelcontrol->episode==1 )
			{
				// in high-difficulity mode switch levels 5 & 9 so
				// you can't get the pogo stick until you make it
				// to the dark side of mars.
				if (newlevel==5) newlevel = 9;
				else if (newlevel==9) newlevel = 5;
			}
		}
		else g_pGfxEngine->Palette.setdarkness(FADE_DARKNESS);

		sprintf(levelname, "level%02d.ck%d", newlevel, p_levelcontrol->episode);

		if (p_levelcontrol->chglevelto==WORLD_MAP)
		  wm = 1;
		else
		  wm = 0;

		if (loadmap(levelname, pCKP->Resources.GameDataDirectory, newlevel, p_levelcontrol))
		{
		  crashflag = 1;
		  crashflag2 = p_levelcontrol->chglevelto;
		  why_term_ptr = "Unable to load the map (# shown in crashflag2).";
		}

		p_levelcontrol->curlevel = p_levelcontrol->chglevelto;

		if (firsttime)
		{
			int op;
			CSprite *obj_sprite;
			for(i=0;i<MAX_PLAYERS;i++)
			{
				op = player[i].useObject;
				obj_sprite = g_pGfxEngine->Sprite[objects[op].sprite];
				player[i].mapplayx = player[i].x;
				player[i].mapplayy = player[i].y;

				player[i].w = obj_sprite->getWidth()-4;
				player[i].h = obj_sprite->getHeight();
			}
		}
		firsttime = 0;

		p_levelcontrol->command = LVLC_NOCOMMAND;

		p_levelcontrol->dark = false;
		p_levelcontrol->usedhintmb = false;
		if (loadinggame)
		{
		  CSavedGame *SavedGame = new CSavedGame(p_levelcontrol, pCKP);

		  if ( !SavedGame->load(loadslot) )
		  {
			crashflag = 1;
			crashflag2 = loadslot;
			g_pLogFile->textOut("Error loading game! The save file may be corrupt or created by a different version of Commander Genius.");
			return;
		  }
		  delete SavedGame;

		  wm = (p_levelcontrol->curlevel==80) ? 1 : 0 ;
		}

		// HQ Music. Load Music for a level if you have HQP
		g_pMusicPlayer->stop();
		sprintf(levelname, "level%02d.ck%d",  p_levelcontrol->curlevel, p_levelcontrol->episode);
		g_pMusicPlayer->LoadfromMusicTable(levelname);

		// Didn't it work? Don't worry. HQP is optional, so continue
		g_pLogFile->ftextOut("Drawing map...\n");
		map_redraw();

		g_pInput->flushAll();

		// Check if we are in Demo-mode. If yes, add the upper logo as object
		if(pCKP->Control.levelcontrol.demomode)
		{
			objects[DemoObjectHandle].exists = 1;
			objects[DemoObjectHandle].onscreen = 1;
			objects[DemoObjectHandle].type = OBJ_DEMOMSG;
			objects[DemoObjectHandle].sprite = DemoSprite;
			objects[DemoObjectHandle].x = 0;
			objects[DemoObjectHandle].y = 0;
			objects[DemoObjectHandle].honorPriority = 0;
		}
		else
			objects[DemoObjectHandle].exists = 0;

		if (wm)
		{  // entering map from normal level, or first time around
		  if (!p_levelcontrol->tobonuslevel)
		  {
			if (!loadinggame)
			{
			  for(i=0;i<MAX_PLAYERS;i++)
			  {
				player[i].x = player[i].mapplayx;
				player[i].y = player[i].mapplayy;
			  }
			}
		  }
		  else
		  {  // respawn at the bonus level
			for(i=0;i<MAX_PLAYERS;i++)
			{
			  player[i].x = BONUSLEVEL_RESPAWN_X;
			  player[i].y = BONUSLEVEL_RESPAWN_Y;
			  if (player[i].isPlaying && player[i].inventory.lives)
			  {
				 player[i].hideplayer = 1;
				 o = spawn_object((player[i].x>>CSF>>4)<<CSF<<4,((player[i].y>>CSF>>4)+1)<<CSF<<4,OBJ_TELEPORTER);
				 objects[o].ai.teleport.direction = TELEPORTING_IN;
				 objects[o].ai.teleport.whichplayer = i;
				 objects[o].ai.teleport.baseframe = TELEPORT_RED_BASEFRAME_EP1;
				 objects[o].ai.teleport.idleframe = TELEPORT_RED_IDLEFRAME_EP1;
				 g_pSound->playStereofromCoord(SOUND_TELEPORT, PLAY_NOW, objects[player[i].useObject].scrx);
			  }
			}
		  }

		  if (!p_levelcontrol->success || firsttime)
		  {
			if (!p_levelcontrol->tobonuslevel) p_levelcontrol->dokeensleft = 1;
			// when you die you lose all keycards
			for(i=0;i<MAX_PLAYERS;i++)
			{
			  if (player[i].isPlaying)
			  {
				  take_keycard(DOOR_YELLOW, i);
				  take_keycard(DOOR_RED, i);
				  take_keycard(DOOR_GREEN, i);
				  take_keycard(DOOR_BLUE, i);
			   }
			}
		  }
		  else p_levelcontrol->dokeensleft = 0;

		  gameloop(pCKP);

		  for(i=0;i<MAX_PLAYERS;i++)
		  {
			player[i].mapplayx = player[i].x;
			player[i].mapplayy = player[i].y;
		  }

		}
		else
		{
		   // entering a normal level from map
		   p_levelcontrol->dokeensleft = 0;
		   gameloop(pCKP);

		   // after completion of a normal level check if the game is won
		   if (gameiswon(pCKP))
			  p_levelcontrol->command = LVLC_END_SEQUENCE;
		}
		g_pMusicPlayer->stop();

		if(QuitState==QUIT_PROGRAM)
		{
			pCKP->shutdown = SHUTDOWN_EXIT;
			break;
		}

		for(unsigned int i=0;i<numplayers;i++)
		{
			player[i].x = 0;
			player[i].y = 0;
		}
  } while(p_levelcontrol->command==LVLC_CHANGE_LEVEL && !crashflag);

  if (p_levelcontrol->command==LVLC_END_SEQUENCE)
  {
    //endsequence(pCKP);
    g_pLogFile->ftextOut("eseq complete<br>");
  }
  else if (p_levelcontrol->command==LVLC_TANTALUS_RAY)
  {
    eseq2_vibrate();
    eseq2_TantalusRay(pCKP);
    start_gameover(pCKP);

    IntroCanceled = 1;               // popup main menu immediately
  }
}*/

