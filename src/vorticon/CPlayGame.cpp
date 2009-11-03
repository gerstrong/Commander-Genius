/*
 * CPlayGame.cpp
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 * See CPlayGame.h for more information
 */

#include "CPlayGame.h"
#include "../keen.h"
#include "../sdl/CTimer.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/sound/CSound.h"
#include "../sdl/CInput.h"
#include "../common/CMapLoader.h"
#include "../graphics/CGfxEngine.h"
#include "../StringUtils.h"
#include "CTeleporter.h"

////
// Creation Routine
////
CPlayGame::CPlayGame( char episode, char level,
					 char numplayers, char difficulty,
					 std::string &gamepath, stOption *p_option ) {
	m_Episode = episode;
	m_Level = level;
	m_NumPlayers = numplayers;
	m_Difficulty = difficulty;
	m_level_command = (level==80) ? GOTO_WORLD_MAP : START_LEVEL;
	m_NumSprites = g_pGfxEngine->getNumSprites();
	m_Gamepath = gamepath;
	m_exitgame = false;
	m_endgame = false;
	m_gameover = false;
	mp_Map = NULL;
	mp_Menu = NULL;
	mp_option = p_option;
	m_checkpoint_x = m_checkpoint_y = 0;
	m_checkpointset = false;
	
	// Create the Player
	if(m_NumPlayers == 0) m_NumPlayers = 1;
	
	mp_Player = new CPlayer[m_NumPlayers];
	
	// Create completed level list
	memset(mp_level_completed,false,16*sizeof(bool));
	
	createPlayerObjects();

	m_theplayer = 0;
	m_paused = false;
	m_showPauseDialog = false;
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
	
	mp_Map->gotoPos( 32, 32 ); // Assure that the edges are never seen
	while(scrollTriggers()); // Scroll the map to players position
	// TODO: Must be changed. It is just a workaround while scrolling doesn't work.
	
	for (int i=0 ; i<m_NumPlayers ; i++)
	{
		if( m_Level == WORLD_MAP_LEVEL )
			mp_Player[i].m_playingmode = CPlayer::WORLDMAP;
		else
			mp_Player[i].m_playingmode = CPlayer::LEVELPLAY;
		
		mp_Player[i].w = g_pGfxEngine->Sprite[PSTANDFRAME]->getWidth()<<(CSF-4);
		mp_Player[i].h = g_pGfxEngine->Sprite[PSTANDFRAME]->getHeight()<<(CSF-4);
		
		// Set the pointers to the map and object data
		mp_Player[i].setMapData(mp_Map);
	}

	// Well, all players are living because they were newly spawn.
	m_alldead = false;

	g_pInput->flushAll();
	
	// Initialize the AI
	mp_ObjectAI = new CObjectAI(mp_Map, &m_Object, mp_Player, mp_option,
								m_NumPlayers, m_Episode, m_Difficulty);

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
	else if(!m_paused) // Game is not paused
	{
		// Perform AIs
		mp_ObjectAI->process();
		
		/// The following functions must be worldmap dependent
		if( m_Level == WORLD_MAP_LEVEL )
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
		}
		else
		{
			// Perform player Objects...
			for( int i=0 ; i<m_NumPlayers ; i++ )
			{
				// Process the other stuff like, items, jump, etc.
				mp_Player[i].processInLevel();
				
				// Process the falling physics of the player here.
				// We need to know the objects and tiles which could hinder the fall.
				// decide if player should fall
				if (!mp_Player[i].inhibitfall) processPlayerfallings(&mp_Player[i]);
				else
				{
					if(mp_Player[i].pjumping == PJUMPED)
						mp_Player[i].pfalling = 0;
					mp_Player[i].psupportingtile = 145;
					mp_Player[i].psupportingobject = 0;
				}
				
				// Check Collisions and only move player, if it is not blocked
				checkPlayerCollisions(&mp_Player[i]);
				
				// Check if the first player is dead, and if the other also are...
				if(i==0) m_alldead = (mp_Player[i].pdie == PDIE_DEAD);
				else m_alldead &= (mp_Player[i].pdie == PDIE_DEAD);
				
				// finished the level
				if(mp_Player[i].level_done == LEVEL_COMPLETE)
				{
			    	mp_level_completed[m_Level] = true;
					goBacktoMap();
				}
			}
			
			// gets to bonus level
			
			// Check if all player are dead. In that case, go back to map
			if(m_alldead) goBacktoMap();
			
			// end(s) the game.
		}
		
		// Handle the Scrolling here!
		scrollTriggers();
		
		// Did the someone press 'p' for Pause?
		
		// Does one of the players need to pause the game?
		for( int i=0 ; i<m_NumPlayers ; i++ )
		{
			// Did he open the status screen?
			if(mp_Player[i].m_showStatusScreen)
				m_paused = true; // this is processed in processPauseDialogs!
			
			// TODO: Did he hit a hint box, like yorp statue in Episode 1.
		}
		
	}
	else // In this case the game is paused
	{
		// Finally draw Dialogs like status screen, game paused, etc.
		processPauseDialogs();
	}
	
	// Animate the tiles of the map
	g_pGfxEngine->Tilemap->animateAllTiles(g_pVideoDriver->ScrollSurface);
	
	// Blit the background
	g_pVideoDriver->blitScrollSurface(mp_Map->m_scrollx_buf, mp_Map->m_scrolly_buf);
	
	// Draw objects to the screen
	drawObjects();
	
	handleFKeys();
	
	if (g_pVideoDriver->showfps)
	{
		std::string tempbuf;
		SDL_Surface *sfc = g_pVideoDriver->FGLayerSurface;
#ifdef DEBUG
		tempbuf = " FPS: " + itoa(g_pTimer->getFramesPerSec()) +
		"; x = " + itoa(mp_Player[0].x) + " ; y = " + itoa(mp_Player[0].y);

#else
		tempbuf = " FPS: " + itoa(g_pTimer->getFramesPerSec());
#endif
		g_pGfxEngine->Font->drawFont( sfc, tempbuf, 320-3-(tempbuf.size()<<3), 3, 1);
	}
	
	
	// Open the Main Menu if ESC Key pressed and mp_Menu not opened
	if(!mp_Menu && g_pInput->getPressedKey(KQUIT))
	{
		// Open the menu
		mp_Menu = new CMenu( CMenu::ACTIVE );
		mp_Menu->init();
	}
}

void CPlayGame::goBacktoMap()
{
	// before he can go back to map, he must tie up the objects.
	// This means, all objects except the puppy ones of the player....
	m_Object.clear();

	createPlayerObjects();

	m_level_command = START_LEVEL;
	m_Level = WM_MAP_NUM;
	//g_pMusicPlayer->stop();
	//g_pSound->playStereofromCoord(SOUND_ENTER_LEVEL, PLAY_NOW, m_Object[mp_Player[i].useObject].scrx);
	// Now that the new level/map will be loaded, the players aren't dead anymore!
	for( int i=0 ; i<m_NumPlayers ; i++ )
	{
		mp_Player[i].level_done = LEVEL_NOT_DONE;
		mp_Player[i].pdie = PDIE_NODIE;
		// Restore checkpoint
		mp_Player[i].x = mp_Player[i].goto_x = m_checkpoint_x;
		mp_Player[i].y = mp_Player[i].goto_y = m_checkpoint_y;
	}
	cleanup();
	init();
}

// called when a switch is flipped. mx,my is the pixel coords of the switch,
// relative to the upper-left corner of the map.
void CPlayGame::ExtendingPlatformSwitch(int x, int y)
{
	/*uint ppos;
	 int platx, platy;
	 signed char pxoff, pyoff;
	 int mapx, mapy;
	 int o;
	 
	 // convert pixel coords to tile coords
	 mapx = (x >> TILE_S);
	 mapy = (y >> TILE_S);
	 
	 // figure out where the platform is supposed to extend at
	 // (this is coded in the object layer...
	 // high byte is the Y offset and the low byte is the X offset,
	 // and it's relative to the position of the switch.)
	 ppos = getlevelat(x, y);
	 
	 if (!ppos || !p_levelcontrol->PlatExtending)
	 {
	 // flip switch
	 g_pSound->playStereofromCoord(SOUND_SWITCH_TOGGLE, PLAY_NOW, mapx);
	 if (getmaptileat(x, y)==TILE_SWITCH_DOWN)
	 map_chgtile(mapx, mapy, TILE_SWITCH_UP);
	 else
	 map_chgtile(mapx, mapy, TILE_SWITCH_DOWN);
	 }
	 
	 // if zero it means he hit the switch on a tantalus ray!
	 if (!ppos)
	 {
	 p_levelcontrol->success = 0;
	 p_levelcontrol->command = LVLC_TANTALUS_RAY;
	 return;
	 }
	 else
	 {
	 // it's a moving platform switch--don't allow player to hit it again while
	 // the plat is still moving as this will glitch
	 if (p_levelcontrol->PlatExtending) return;
	 p_levelcontrol->PlatExtending = 1;
	 }
	 
	 pxoff = (ppos & 0x00ff);
	 pyoff = (ppos & 0xff00) >> 8;
	 platx = mapx + pxoff;
	 platy = mapy + pyoff;
	 
	 // spawn a "sector effector" to extend/retract the platform
	 o = spawn_object(mapx<<TILE_S<<CSF,mapy<<TILE_S<<CSF,OBJ_SECTOREFFECTOR);
	 objects[o].ai.se.type = SE_EXTEND_PLATFORM;
	 objects[o].ai.se.platx = platx;
	 objects[o].ai.se.platy = platy;*/
}

void CPlayGame::losePlayer(CPlayer *p_player)
{
    if (p_player->inventory.lives<0)
    {
		m_gameover = true;
		g_pSound->playSound(SOUND_GAME_OVER, PLAY_NOW);
		
		// TODO: The commented out stuff must get an object
		/*
		 CBitmap *bm_gameover = g_pGfxEngine->getBitmap("GAMEOVER");
		 // figure out where to center the gameover bitmap and draw it
		 int x = (g_pVideoDriver->getGameResRect().w/2)-(bm_gameover->getWidth()/2);
		 int y = (g_pVideoDriver->getGameResRect().h/2)-(bm_gameover->getHeight()/2);
		 bm_gameover->draw(g_pVideoDriver->BlitSurface, x, y);
		 */
    }
    else
    {
		m_Level = WM_MAP_NUM;
		init();
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
				/*give_keycard(DOOR_YELLOW, i);
				 give_keycard(DOOR_RED, i);
				 give_keycard(DOOR_GREEN, i);
				 give_keycard(DOOR_BLUE, i);*/
				
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

// This function draws the objects that need to be seen on the screen
void CPlayGame::drawObjects()
{
	int i;
	int x,y,o,tl,xsize,ysize;
	int xa,ya;
	
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
	//std::vector<CObject>::reverse_iterator p_object;
	CObject *p_object;
	for ( i=m_Object.size()-1 ; i>=0 ; i--)
	{
		p_object = &m_Object[i];
		
		if (p_object->exists && p_object->onscreen)
		{
			p_object->scrx = (p_object->x>>(CSF-4))-mp_Map->m_scrollx;
			p_object->scry = (p_object->y>>(CSF-4))-mp_Map->m_scrolly;
			
			//g_pGraphics->drawSprite(objects[i].scrx, objects[i].scry, objects[i].sprite, i);
			
			g_pGfxEngine->Sprite[p_object->sprite]->drawSprite( g_pVideoDriver->BlitSurface,
															   p_object->scrx, p_object->scry );
			
			
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
						if(g_pGfxEngine->Tilemap->mp_tiles[tl].behaviour == -2)
							g_pGfxEngine->Tilemap->drawTile(sfc, x+xa-mp_Map->m_scrollx, y+ya-mp_Map->m_scrolly, tl+1);
						else if (g_pGfxEngine->Tilemap->mp_tiles[tl].behaviour == -1)
							g_pGfxEngine->Tilemap->drawTile(sfc, x+xa-mp_Map->m_scrollx, y+ya-mp_Map->m_scrolly, tl);
					}
	            }
	        }
		}
	}
}

// scroll triggers
#define SCROLLTRIGGERRIGHT     194
#define SCROLLTRIGGERLEFT      110
#define SCROLLTRIGGERUP        80
#define SCROLLTRIGGERDOWN      114

bool CPlayGame::scrollTriggers()
{
	int px, py;
	bool scrollchanged=false;
	int scroll_x, scroll_y;
	int max_scroll_x, max_scroll_y;
	
	scroll_x = mp_Map->m_scrollx;
	scroll_y = mp_Map->m_scrolly;
	max_scroll_x = mp_Map->m_maxscrollx<<4;
	max_scroll_y = mp_Map->m_maxscrolly<<4;
	
	if (mp_Player[m_theplayer].pdie) return false;
	
	px = (mp_Player[m_theplayer].x>>5)-scroll_x;
	py = (mp_Player[m_theplayer].y>>5)-scroll_y;
	
	// left-right scrolling
	if(px > SCROLLTRIGGERRIGHT && scroll_x < max_scroll_x)
	{
		do{
			scroll_x = mp_Map->m_scrollx;
			px = (mp_Player[m_theplayer].x>>5)-scroll_x;
			mp_Map->scrollRight();
		}while(px > 226);
		scrollchanged = true;
	}
	else if(px < SCROLLTRIGGERLEFT && scroll_x > 32)
	{
		do{
			scroll_x = mp_Map->m_scrollx;
			px = (mp_Player[m_theplayer].x>>5)-scroll_x;
			mp_Map->scrollLeft();
		}while(px < 80);
		scrollchanged = true;
	}
	
	// up-down scrolling
	if (py > SCROLLTRIGGERDOWN && scroll_y < max_scroll_y)
	{
		mp_Map->scrollDown();
		scrollchanged = true;
	}
	else if (py < SCROLLTRIGGERUP && scroll_y > 32)
	{
		mp_Map->scrollUp();
		scrollchanged = true;
	}
	return scrollchanged;
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

