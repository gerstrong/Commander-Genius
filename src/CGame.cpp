/*
 * CGame.cpp
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 *  This Game-engine stripes down the main function
 *  and provides more dynamic control over the game
 */

#include <stdlib.h>

#include "keen.h"
#include "CGame.h"
#include "common/CMenu.h"
#include "include/game.h"
#include "include/misc.h"
#include "include/main.h"
#include "include/CStartScreen.h"
#include "include/fileio/story.h"
#include "vorticon/CHighScores.h"
#include "vorticon/CIntro.h"
#include "vorticon/CCredits.h"
#include "CLogFile.h"
#include "fileio.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "sdl/CTimer.h"
#include "vorticon/COrderingInfo.h"

CGame::CGame() {
}

CGame::~CGame() {
}

//////////////////////////////////
// Initialize Game Engine here! //
//////////////////////////////////
bool CGame::init()
{
	CSettings Settings;

	// Check if there are settings on the PC, otherwise use defaults.
	if(!Settings.loadDrvCfg())
	{
		g_pLogFile->textOut(PURPLE,"First time message: CKP didn't find the driver config file. However, it generated some default values and will save them now.<br>");
		Settings.saveDrvCfg();
	}

	// Setup the Hardware using the settings we have loaded
	g_pLogFile->textOut(GREEN,"Loading hardware settings...<br>");
	if(!loadCKPDrivers())
	{
		g_pLogFile->textOut(RED,"The game cannot start, because you do not meet the hardware requirements.<br>");
		return false;
	}

	// Initialize the way the launcher is started
	m_GameControl.init();

	return true;
}

/////////////////////////////
// Process Game Engine here! //
/////////////////////////////
void CGame::run()
{
	do
	{
	   // Poll Inputs
	   g_pInput->pollEvents();

	   // Process Game Control
	   m_GameControl.process();

	   if( g_pTimer->TimeToRender() )
	   {
		   // Render the Screen
		   g_pVideoDriver->update_screen();

		   // wait the time until next frame
		   g_pTimer->TimeToDelay();
	   }
	} while(!m_GameControl.mustShutdown());
}

///////////////////////////////
// Cleanup Game Engine here! //
///////////////////////////////
void CGame::cleanup()
{
	m_GameControl.cleanup();
}


///////////////////////////////////////////////
// Depreciated Fnctions. Might be removed soon!
///////////////////////////////////////////////
/*
short CGame::runCycle(stCloneKeenPlus *pCKP)
{
	  int opt = MAINMNU_1PLAYER;
	  //int retval;
	  int defaultopt = 0;

	  //initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
	  //initgame( &(pCKP->Control.levelcontrol) );

	  g_pLogFile->ftextOut("Game starting...<br>");

	  if(!pCKP->Control.skipstarting)
	  {
		  CIntro Intro;
		  //Intro.Render(pCKP);
		  pCKP->Control.skipstarting=0;
	  }

	  do
	  {
	    if (QuitState==QUIT_TO_TITLE) QuitState = NO_QUIT;

	    if(pCKP->Control.storyboard == 1) // Show the story of the game
	    {
	    	char *text;
	    	int textsize;

	    	textsize = readStoryText(&text,
					pCKP->Control.levelcontrol.episode,
					pCKP->Resources.GameDataDirectory); // Read text from
																				   // and store it at the text pointer

	    	if(textsize > 0)
	    	{
	    		showmapatpos(90, STORYBOARD_X, STORYBOARD_Y, pCKP);
				showPage(text,textsize);

				free(text);
			}
	    	else if(textsize == 0)
	    	{
	    		g_pLogFile->ftextOut("readStoryText(): Error reading story text. Are you sure that there is any story text?");
	    	}
	    	else if(textsize == 0)
	    	{
	    		g_pLogFile->ftextOut("readStoryText(): Error reading story text. The version appears to be incompatible");
	    	}
	    	pCKP->Control.storyboard = 0;
	    }

	    if(pCKP->Control.levelcontrol.command != LVLC_START_LEVEL)
	    {
	    	g_pLogFile->ftextOut("calling mainmenu()<br>");
	    	CMenu Mainmenu;

	    	opt = Mainmenu.getChoice(pCKP, defaultopt); // Read option from the main menu
														// of the game.
			pCKP->Control.skipstarting=0;

			g_pLogFile->ftextOut("gcl: opt = %d<br>", opt);
	    }

	    defaultopt = 0;
	    IntroCanceled = 0;
	    switch(opt)
	      {
	      case MAINMNU_1PLAYER:
	        numplayers = 1;
	        defaultopt = 0;
	        //current_demo = 1;
	        //initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
	        loadinggame = 0;
	        playgame_levelmanager(pCKP);
	        break;
	      case MAINMNU_2PLAYER:
	        defaultopt = 0;
	        //current_demo = 1;
	        numplayers = 2;
	        //initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
	        loadinggame = 0;
	        playgame_levelmanager(pCKP);
	        break;
	      case MAINMNU_LOADGAME:
	        if (loadslot)
	        {
	           loadinggame = 1;
	           defaultopt = 0;
	           //current_demo = 1;
	           numplayers = 1; // here was 2. Why was that? I don't understand
	           //initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
	           playgame_levelmanager(pCKP);
	        }
	        break;

	      case MAINMNU_HIGHSCORES:
	    	  CHighScores *pHighscores;
	    	  pHighscores = new CHighScores(g_pVideoDriver->FGLayerSurface, pCKP);
	    	  if(pHighscores->showHighScore())
	    	  {
	    		  g_pLogFile->ftextOut("Error processing Highscore!!<br>");
	    	  }
	    	  delete pHighscores;
	    	  break;

	      case MAINMNU_NEW_GAME:
	    	  pCKP->shutdown = SHUTDOWN_NEW_GAME;
	    	break;

	      case MAINMNU_ABOUT:
	    	  CCredits *pCredit;
	    	  pCredit = new CCredits;
	    	  pCredit->Render(pCKP);
	    	  delete pCredit;
	    	  break;

	      case MAINMNU_ORDERING_INFO:
	    	  COrderingInfo *OrderingInfo;
	    	  OrderingInfo = new COrderingInfo(g_pVideoDriver->FGLayerSurface,
											  pCKP->Control.levelcontrol.episode,
											  pCKP->Resources.GameDataDirectory);
	    	  OrderingInfo->Render(pCKP);
	    	  delete OrderingInfo;
	    	  break;

	      case MAINMNU_TIMEOUT:
	      case MAINMNU_DEMO:

	    	  //retval = play_demo(current_demo, pCKP, EGAGraphics->getNumSprites());

	    	  if (retval==DEMO_RESULT_FILE_BAD)
	    	  {
	    		  // we tried to play a demo that did not exist--assume we
	    		  // reached the last demo and go back to the intro
	    		  //intro(pCKP);
	    		  CIntro *pIntro;
	    		  pIntro = new CIntro();
	    		  delete pIntro;
	    		  pIntro = NULL;
	    		  //current_demo = 0;
	    	  }
	    	  else if (retval==DEMO_RESULT_CANCELED)
	    	  { // user hit a key to cancel demo
	    		  IntroCanceled = 1;            // pop up menu
	    	  }

	    	  if (IntroCanceled)
	    	  {   // user canceled out of demo (or intro if at end of demos)
	    		  // if user selected "demo" have it selected when he comes back
	    		  if (opt==MAINMNU_DEMO)
	    			  defaultopt = MAINMNU_DEMO;

	    	  }
	    	  current_demo++;
	      break;
	      case RESTART_GAME:
	        g_pLogFile->ftextOut("********************<br>");
	        g_pLogFile->ftextOut(" Restarting game...<br>");
	        g_pLogFile->ftextOut("********************<br>\n");
	        cleanup();
	        pCKP->shutdown = SHUTDOWN_RESTART;
	        return 0;
	      break;
	      case BACK2MAINMENU:

	      default: break;
	      }

		  if(pCKP->shutdown == SHUTDOWN_NEW_GAME) return 0;

	      g_pLogFile->ftextOut("bottom of game control loop opt=%d crashflag=%d<br>", opt, crashflag);
	      if(pCKP->shutdown == SHUTDOWN_EXIT) break;
	    } while(opt != MAINMNU_QUIT && opt != MAINMNU_NEW_GAME && !crashflag );

		return 0;
}

short CGame::ingamerunCycle(stCloneKeenPlus *pCKP)
{
	int opt = MAINMNU_1PLAYER;
	//int retval;
	int defaultopt = 0;
		
	do
	{
	    if (QuitState==QUIT_TO_TITLE) QuitState = NO_QUIT;
		
	    if(pCKP->Control.storyboard == 1) // Show the story of the game
	    {
	    	char *text;
	    	int textsize;
			
	    	textsize = readStoryText(&text,
									 pCKP->Control.levelcontrol.episode,
									 pCKP->Resources.GameDataDirectory); // Read text from
			// and store it at the text pointer
			
	    	if(textsize > 0)
	    	{
	    		showmapatpos(90, STORYBOARD_X, STORYBOARD_Y, pCKP);
				showPage(text,textsize);
				
				free(text);
			}
	    	else if(textsize == 0)
	    	{
	    		g_pLogFile->ftextOut("readStoryText(): Error reading story text. Are you sure that there is any story text?");
	    	}
	    	else if(textsize == 0)
	    	{
	    		g_pLogFile->ftextOut("readStoryText(): Error reading story text. The version appears to be incompatible");
	    	}
	    	pCKP->Control.storyboard = 0;
	    }
		
	    if(pCKP->Control.levelcontrol.command != LVLC_START_LEVEL)
	    {
	    	g_pLogFile->ftextOut("calling mainmenu()<br>");
	    	CMenu Mainmenu;

	    	opt = Mainmenu.getChoice(pCKP, defaultopt); // Read option from the main menu
														// of the game.
			pCKP->Control.skipstarting=0;
			
			g_pLogFile->ftextOut("gcl: opt = %d<br>", opt);
	    }
		
	    defaultopt = 0;
	    IntroCanceled = 0;
	    switch(opt)
		{
			case MAINMNU_1PLAYER:
				numplayers = 1;
				defaultopt = 0;
				//current_demo = 1;
				//initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
				loadinggame = 0;
				playgame_levelmanager(pCKP);
				break;
			case MAINMNU_2PLAYER:
				defaultopt = 0;
				//current_demo = 1;
				numplayers = 2;
				//initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
				loadinggame = 0;
				playgame_levelmanager(pCKP);
				break;
			case MAINMNU_LOADGAME:
				if (loadslot)
				{
					loadinggame = 1;
					defaultopt = 0;
					//current_demo = 1;
					numplayers = 1; // here was 2. Why was that? I don't understand
					//initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
					playgame_levelmanager(pCKP);
				}
				break;
				
			case MAINMNU_STORY:
				pCKP->Control.storyboard=1;
				break;
				
			case MAINMNU_HIGHSCORES:
				CHighScores *pHighscores;
				pHighscores = new CHighScores(g_pVideoDriver->FGLayerSurface, pCKP);
				if(pHighscores->showHighScore())
				{
					g_pLogFile->ftextOut("Error processing Highscore!!<br>");
				}
				delete pHighscores;
				break;
			case MAINMNU_NEW_GAME:
				//pCKP->shutdown = SHUTDOWN_NEW_GAME;
				QuitState = QUIT_TO_TITLE;
				return 0;
				break;
			case MAINMNU_ABOUT:
				CCredits *pCredit;
				pCredit = new CCredits;
				pCredit->Render(pCKP);
				delete pCredit;
				break;
				
			case MAINMNU_ORDERING_INFO:
				COrderingInfo *OrderingInfo;
				OrderingInfo = new COrderingInfo(g_pVideoDriver->FGLayerSurface,
												 pCKP->Control.levelcontrol.episode,
												 pCKP->Resources.GameDataDirectory);
				OrderingInfo->Render(pCKP);
				delete OrderingInfo;
				break;
				
			case MAINMNU_TIMEOUT:
			case MAINMNU_DEMO:
				
				QuitState = NO_QUIT;
				return 0;
				
				retval = play_demo(current_demo, pCKP, EGAGraphics->getNumSprites());
				
				if (retval==DEMO_RESULT_FILE_BAD)
				{
					// we tried to play a demo that did not exist--assume we
					// reached the last demo and go back to the intro
					//intro(pCKP);
					CIntro *pIntro;
					pIntro = new CIntro();
					delete pIntro;
					pIntro = NULL;
					current_demo = 0;
				}
				else if (retval==DEMO_RESULT_CANCELED)
				{ // user hit a key to cancel demo
					IntroCanceled = 1;            // pop up menu
				}
				
				if (IntroCanceled)
				{   // user canceled out of demo (or intro if at end of demos)
					// if user selected "demo" have it selected when he comes back
					if (opt==MAINMNU_DEMO)
						defaultopt = MAINMNU_DEMO;
					
				}
				current_demo++;
				break;
			case RESTART_GAME:
				g_pLogFile->ftextOut("********************<br>");
				g_pLogFile->ftextOut(" Restarting game...<br>");
				g_pLogFile->ftextOut("********************<br>\n");
				cleanup();
				pCKP->shutdown = SHUTDOWN_RESTART;
				return 0;
				break;
			case BACK2MAINMENU:
				
			default: break;
		}
		
		g_pLogFile->ftextOut("bottom of game control loop opt=%d crashflag=%d<br>", opt, crashflag);
		if(pCKP->shutdown == SHUTDOWN_EXIT) break;
	} while(opt != MAINMNU_QUIT && opt != MAINMNU_NEW_GAME && !crashflag );
	
	return 0;
}

void CGame::preallocateCKP(stCloneKeenPlus *pCKP)
{
	// This function prepares the CKP Structure so that the it is allocated in the memory.
	pCKP->numGames = 0;
	pCKP->Resources.GameSelected = 0;

	pCKP->GameData = NULL;
	pCKP->GameData = new stGameData[1];

	pCKP->Control.levelcontrol.demomode = DEMO_NODEMO;
	//current_demo = 1;

	pCKP->Joystick = NULL;

	acceleratemode = 0;
	g_pVideoDriver->showFPS(false);

	player[0].x = player[0].y = 0;
}*/
