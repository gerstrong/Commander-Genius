/*
 * CPlayGame.cpp
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 * See CPlayGame.h for more information
 */

#include "CPlayGame.h"
#include "../../keen.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/sound/CSound.h"
#include "../../sdl/CInput.h"
#include "../../common/CMapLoader.h"
#include "../../graphics/CGfxEngine.h"
#include "../../StringUtils.h"

////
// Creation Routine
////
CPlayGame::CPlayGame( char episode, char level,
					 char numplayers, char difficulty,
					 std::string &gamepath, stOption *p_option,
					 bool finale) {
	m_Episode = episode;
	m_Level = level;
	m_NumPlayers = numplayers;
	m_Difficulty = difficulty;
	m_level_command = (level==80) ? GOTO_WORLD_MAP : START_LEVEL;
	m_NumSprites = g_pGfxEngine->getNumSprites();
	m_Gamepath = gamepath;
	m_exitgame = false;
	m_endgame = false;
	m_startgame = false;
	m_gameover = false;
	m_alldead = false;
	m_hideobjects = false;
	mp_Map = NULL;
	mp_Menu = NULL;
	mp_Finale = NULL;
	mp_gameoverbmp = NULL;
	mp_option = p_option;
	m_checkpoint_x = m_checkpoint_y = 0;
	m_checkpointset = false;

	// Create the Player
	if(m_NumPlayers == 0) m_NumPlayers = 1;
	
	mp_Player = new CPlayer[m_NumPlayers];
	
	// Create completed level list
	memset(mp_level_completed,false,MAX_LEVELS*sizeof(bool));

	// Player are tied to objects like enemys, sprites, etc, so they
	// can be drawn the same way.
	createPlayerObjects();

	m_theplayer = 0;
	m_paused = false;
	m_showPauseDialog = false;
	if(finale) m_level_command = GOTO_FINALE;
}

bool CPlayGame::init()
{
	// Create an empty map
	mp_Map = new CMap( g_pVideoDriver->getScrollSurface(), g_pGfxEngine->Tilemap);
	CMapLoader MapLoader( mp_Map, mp_Player );
	MapLoader.m_checkpointset = m_checkpointset;
	MapLoader.mp_objvect = &m_Object;
	
	// load level map
	if( !mp_Map ) return false;
	if( !MapLoader.load( m_Episode, m_Level, m_Gamepath ) ) return false;
	
	//// If those worked fine, continue the initialization
	// draw level map
	mp_Map->drawAll();
	
	// Now Scroll to the position of the player and center him
	
	mp_Map->gotoPos( 32, 64 ); // Assure that the edges are never seen
	
	m_showKeensLeft=false;
	for (int i=0 ; i<m_NumPlayers ; i++)
	{
		if( m_Level == WORLD_MAP_LEVEL )
		{
			mp_Player[i].m_playingmode = CPlayer::WORLDMAP;
			m_showKeensLeft |= ( mp_Player[i].pdie == PDIE_DEAD );
			mp_Player[i].pdie = PDIE_NODIE;
		}
		else
		{
			mp_Player[i].pdie = PDIE_NODIE;
			mp_Player[i].m_playingmode = CPlayer::LEVELPLAY;
		}
		
		mp_Player[i].w = g_pGfxEngine->Sprite[PSTANDFRAME]->getWidth()<<(CSF-4);
		mp_Player[i].h = g_pGfxEngine->Sprite[PSTANDFRAME]->getHeight()<<(CSF-4);
		
		// Set the pointers to the map and object data
		mp_Player[i].setMapData(mp_Map);
	}

	while(mp_Player[0].scrollTriggers());   // Scroll the map to players position

	// Well, all players are living because they were newly spawn.
	g_pTimer->ResetSecondsTimer();

	g_pInput->flushAll();
	
	// Initialize the AI
	mp_ObjectAI = new CObjectAI(mp_Map, &m_Object, mp_Player, mp_option,
								m_NumPlayers, m_Episode, m_Level, m_Difficulty);

	// Check if Player meets the conditions to show a cutscene. This also happens, when finale of episode has reached
	verifyCutscenes();

	if(m_level_command == GOTO_FINALE)
		createFinale();
	else
		if(m_showKeensLeft)	g_pSound->playSound(SOUND_KEENSLEFT, PLAY_NOW);

	return true;
}

void CPlayGame::createPlayerObjects()
{
	// tie puppy objects so the player can interact in the level
	for (int i=0 ; i<m_NumPlayers ; i++)
	{
		CObject object;
		mp_Player[i].setDatatoZero();
		mp_Player[i].m_player_number = i;
		mp_Player[i].m_episode = m_Episode;
		mp_Player[i].mp_levels_completed = mp_level_completed;

	    object.exists = true;
		object.onscreen = true;
		object.honorPriority = true;
		object.m_type = OBJ_PLAYER;
		mp_Player[i].mp_option = mp_option;
		m_Object.push_back(object);
		mp_Player[i].mp_object=&m_Object;
	}
}

////
// Process Routine
////
void CPlayGame::process()
{
	// If the menu is open process it!
	if(mp_Menu)
	{
		if( mp_Menu->mustBeClosed() || mp_Menu->getExitEvent() || mp_Menu->mustEndGame() || mp_Menu->mustStartGame() )
		{
			if( mp_Menu->getExitEvent() )
				m_exitgame = true;
			
			if( mp_Menu->mustEndGame() )
				m_endgame = true;
			
			if( mp_Menu->mustStartGame() )
				m_startgame = true;
			
			mp_Menu->cleanup();
			delete mp_Menu;
			mp_Menu = NULL;
			m_hideobjects = false;
		}
		else
		{
			mp_Menu->process();
			m_hideobjects = mp_Menu->m_hideobjects;
		}
	}
	else if(!m_paused) // Game is not paused
	{
		if (!mp_Finale) // Has the game been finished?
		{
			// Perform AIs
			mp_ObjectAI->process();
			
			/// The following functions must be worldmap dependent
			if( m_Level == WORLD_MAP_LEVEL )
			{
				processOnWorldMap();
			}
			else
			{
				processInLevel();
			}

			// Did the someone press 'p' for Pause?

			// Does one of the players need to pause the game?
			for( int i=0 ; i<m_NumPlayers ; i++ )
			{
				// Did he open the status screen?
				if(mp_Player[i].m_showStatusScreen)
					m_paused = true; // this is processed in processPauseDialogs!
				
				// TODO: Did he hit a hint box, like yorp statue in Episode 1?

				// Handle the Scrolling here!
				mp_Player[i].scrollTriggers();
			}
		}
		else // In this case the Game has been finished, goto to the cutscenes
		{
			mp_Finale->process();

			m_endgame = mp_Finale->getHasFinished();
		}
	}
	else // In this case the game is paused
	{
		// Finally draw Dialogs like status screen, game paused, etc.
		processPauseDialogs();
	}
	// Animate the tiles of the map
	mp_Map->animateAllTiles();
	
	// Blit the background
	g_pVideoDriver->blitScrollSurface();

	// Draw objects to the screen
	drawObjects();
	
	// Check if we are in gameover mode. If yes, than show the bitmaps and block the FKeys().
	// Only confirmation button is allowes
	if(m_gameover) // game over mode
	{
		if(mp_gameoverbmp != NULL)
		{
			mp_gameoverbmp->process();

			if( g_pInput->getPressedKey(KENTER) || g_pInput->getPressedAnyCommand() )
				m_endgame = true;
		}
		else // Bitmap must first be created
		{
			CBitmap *pBitmap = g_pGfxEngine->getBitmap("GAMEOVER");
			g_pSound->playSound(SOUND_GAME_OVER, PLAY_NOW);
			mp_gameoverbmp = new CEGABitmap(g_pVideoDriver->BlitSurface, pBitmap);
			mp_gameoverbmp->setScrPos( 160-(pBitmap->getWidth()/2), 100-(pBitmap->getHeight()/2) );
		}
	}
	else // No game over
	{
		// Handle special functional key for god mode, all items, etc.
		handleFKeys();
	}

	if (g_pVideoDriver->showfps)
	{
		std::string tempbuf;
		SDL_Surface *sfc = g_pVideoDriver->FGLayerSurface;
#ifdef DEBUG
//		tempbuf = " FPS: " + itoa(g_pTimer->getFramesPerSec()) +
	//		"; x = " + itoa(mp_Player[0].x) + " ; y = " + itoa(mp_Player[0].y);

		tempbuf = "Playpushed_x: " + itoa(mp_Player[0].playpushed_x);

#else
		tempbuf = " FPS: " + itoa(g_pTimer->getFramesPerSec());
#endif
		g_pGfxEngine->Font->drawFont( sfc, tempbuf, 320-3-(tempbuf.size()<<3), 3, 1);
	}
	
	
	// Open the Main Menu if ESC Key pressed and mp_Menu not opened
	if(!mp_Menu && g_pInput->getPressedKey(KQUIT))
	{
		// Open the menu
		mp_Menu = new CMenu( CMenu::ACTIVE, m_Gamepath, m_Episode, *mp_Map );
		mp_Menu->init();
	}
}


void CPlayGame::handleFKeys()
{
	int i;
	
    if (g_pInput->getHoldedKey(KC) &&
		g_pInput->getHoldedKey(KT) &&
		g_pInput->getHoldedKey(KSPACE))
	{
		g_pInput->flushAll();
		for(i=0;i<m_NumPlayers;i++)
		{
			mp_Player[i].pfiring = false;
			if (mp_Player[i].m_playingmode)
			{
				mp_Player[i].give_keycard(DOOR_YELLOW);
				mp_Player[i].give_keycard(DOOR_RED);
				mp_Player[i].give_keycard(DOOR_GREEN);
				mp_Player[i].give_keycard(DOOR_BLUE);
				
				mp_Player[i].inventory.charges = 999;
				mp_Player[i].inventory.HasPogo = 1;
				mp_Player[i].inventory.lives = 10;
				
				// Show a message like in the original game
				/*char **text;
				 int i;
				 
				 text = (char**) malloc(4*sizeof(char*));
				 
				 for(i=0;i<4;i++)
				 {
				 static const int MAX_STRING_LENGTH = 256;
				 text[i]= (char*) malloc(MAX_STRING_LENGTH*sizeof(char));
				 }
				 
				 std::string Text;
				 
				 Text = 	"You are now cheating!\n";
				 Text +=	"You got more lives\n";
				 Text +=	"all the key cards, and\n";
				 Text +=	"lots of ray gun charges!\n";
				 
				 showTextMB(Text);*/
			}
		}
		g_pVideoDriver->AddConsoleMsg("All items cheat");
	}
	
	/*
	 // GOD cheat -- toggle god mode
	 if (g_pInput->getHoldedKey(KG) && g_pInput->getHoldedKey(KO) && g_pInput->getHoldedKey(KD))
	 {
	 for(i=0;i<MAX_PLAYERS;i++)
	 player[i].godmode ^= 1;
	 
	 g_pVideoDriver->DeleteConsoleMsgs();
	 if (player[0].godmode)
	 g_pVideoDriver->AddConsoleMsg("God mode ON");
	 else
	 g_pVideoDriver->AddConsoleMsg("God mode OFF");
	 
	 g_pSound->playSound(SOUND_GUN_CLICK, PLAY_FORCE);
	 
	 // Show a message like in the original game
	 showTextMB(player[0].godmode ? "Godmode enabled" : "Godmode disabled");
	 }
	 
	 if (pCKP->Option[OPT_CHEATS].value)
	 {
	 if (g_pInput->getHoldedKey(KTAB)) // noclip/revive
	 {
	 // resurrect any dead players. the rest of the KTAB magic is
	 // scattered throughout the various functions.
	 for(i=0;i<MAX_PLAYERS;i++)
	 {
	 if (player[i].pdie)
	 {
	 player[i].pdie = PDIE_NODIE;
	 player[i].y -= (8<<CSF);
	 }
	 player[i].pfrozentime = 0;
	 }
	 }
	 
	 // F9 - exit level immediately
	 if(g_pInput->getPressedKey(KF9))
	 {
	 endlevel(1, &(pCKP->Control.levelcontrol) );
	 }
	 }*/
	
    // F10 - change primary player
    /*if(g_pInput->getPressedKey(KF10))
	 {
	 primaryplayer++;
	 if (primaryplayer>=numplayers) primaryplayer=0;
	 }
	 // F3 - save game
	 if (g_pInput->getPressedKey(KF3))
	 game_save_interface(pCKP);*/
	
}

// The Ending and mortimer cutscenes for example
void CPlayGame::verifyCutscenes()
{
	// first we need to know which Episode we were on
	if(m_Episode == 1)
	{
		bool hasBattery, hasWiskey, hasJoystick, hasVaccum;
		hasBattery = hasWiskey = hasJoystick = hasVaccum = false;

		// Check if one of the Players has the items
		for( int i=0 ;i < m_NumPlayers ; i++)
		{
			hasBattery |= mp_Player[i].inventory.HasBattery;
			hasWiskey |= mp_Player[i].inventory.HasWiskey;
			hasJoystick |= mp_Player[i].inventory.HasJoystick;
			hasVaccum |= mp_Player[i].inventory.HasVacuum;
		}

		// If they have have the items, we can go home
		if(hasBattery && hasWiskey && hasJoystick && hasVaccum)
			createFinale();
	}
	/*else if(m_Episode == 2)
	{
		mp_Finale = new CEndingEp2();
	}
	else if(m_Episode == 2)
	{
		mp_Finale = new CEndingEp3();
	}*/
}

void CPlayGame::createFinale()
{
	if(m_Episode == 1)
		mp_Finale = new CEndingEp1(mp_Map, mp_Player);
	/*else if(m_Episode == 2)
		mp_Finale = new CEndingEp2(mp_Map, mp_Player);
	else if(m_Episode == 3)
		mp_Finale = new CEndingEp3(mp_Map, mp_Player);*/
}

// This function draws the objects that need to be seen on the screen
void CPlayGame::drawObjects()
{
	int i;
	int x,y,o,tl,xsize,ysize;
	int xa,ya;
	
	if(m_hideobjects) return;

	// copy player data to their associated objects show they can get drawn
	// in the object-drawing loop with the rest of the objects
	for( i=0 ;i < m_NumPlayers ; i++)
	{
		o = mp_Player[i].m_player_number;
		
		if (!mp_Player[i].hideplayer)
			m_Object.at(o).sprite = mp_Player[i].playframe;
		else
			m_Object.at(o).sprite = m_NumSprites-1;
		
		m_Object.at(o).x = mp_Player[i].x;
		m_Object.at(o).y = mp_Player[i].y;
	}
	
	// draw all objects. drawn in reverse order because the player sprites
	// are in the first few indexes and we want them to come out on top.
	CObject *p_object;
	for ( i=m_Object.size()-1 ; i>=0 ; i--)
	{
		p_object = &m_Object[i];
		
		if (p_object->exists && p_object->onscreen)
		{
			CSprite *Sprite = g_pGfxEngine->Sprite[p_object->sprite];
			p_object->scrx = (p_object->x>>STC)-mp_Map->m_scrollx;
			p_object->scry = (p_object->y>>STC)-mp_Map->m_scrolly;

			Sprite->drawSprite( g_pVideoDriver->BlitSurface, p_object->scrx, p_object->scry );

			p_object->bboxX1 = Sprite->m_bboxX1;
			p_object->bboxX2 = Sprite->m_bboxX2;
			p_object->bboxY1 = Sprite->m_bboxY1;
			p_object->bboxY2 = Sprite->m_bboxY2;

	        if (p_object->honorPriority)
	        {
	        	CSprite *sprite = g_pGfxEngine->Sprite[p_object->sprite];
	            // handle priority tiles and tiles with masks
	            // get the upper-left coordinates to start checking for tiles
	            x = (p_object->x>>CSF);
	            y = (p_object->y>>CSF);
				
	            // get the xsize/ysize of this sprite--round up to the nearest 16
	            xsize = ((sprite->getWidth()>>4)<<4);
	            if (xsize != sprite->getWidth()) xsize+=16;
				
	            ysize = ((g_pGfxEngine->Sprite[p_object->sprite]->getHeight()>>4)<<4);
	            if (ysize != sprite->getHeight()) ysize+=16;
				
	            tl = mp_Map->at(x,y);
	            x<<=4;
	            y<<=4;
				
	            // now redraw any priority/masked tiles that we covered up
	            // with the sprite
	            SDL_Surface *sfc = g_pVideoDriver->BlitSurface;
	            SDL_Rect sfc_rect;
	            sfc_rect.w = sfc_rect.h = 16;
				
	            for(ya=0;ya<=ysize;ya+=16)
	            {
					for(xa=0;xa<=xsize;xa+=16)
					{
						tl = mp_Map->at((x+xa)>>4,(y+ya)>>4);
						if(mp_Map->mp_tiles[tl].behaviour == -2)
							g_pGfxEngine->Tilemap->drawTile(sfc, x+xa-mp_Map->m_scrollx, y+ya-mp_Map->m_scrolly, tl+1);
						else if (mp_Map->mp_tiles[tl].behaviour == -1)
							g_pGfxEngine->Tilemap->drawTile(sfc, x+xa-mp_Map->m_scrollx, y+ya-mp_Map->m_scrolly, tl);
					}
	            }
	        }
		}
	}
}

////
// Cleanup Routine
////
void CPlayGame::cleanup()
{
	delete mp_Map; mp_Map = NULL;
	delete mp_ObjectAI; mp_ObjectAI = NULL;
}

CPlayGame::~CPlayGame() {
	if(mp_Player) delete [] mp_Player;
	mp_Player=NULL;
	if(mp_Finale) delete mp_Finale;
	mp_Finale = NULL;
	if(mp_gameoverbmp) delete mp_gameoverbmp;
	mp_gameoverbmp = NULL;
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

