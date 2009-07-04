/*
 * CGame.cpp
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#include <stdlib.h>

#include "keen.h"
#include "CGame.h"
#include "include/game.h"
#include "include/misc.h"
#include "include/main.h"
#include "include/menu.h"
#include "include/fileio/story.h"
#include "vorticon/CHighScores.h"
#include "vorticon/CIntro.h"
#include "vorticon/CCredits.h"
#include "CLogFile.h"
#include "fileio.h"
#include "sdl/sound/CSound.h"
#include "sdl/CVideoDriver.h"

CGame::CGame() {
	m_Episode = 0;
	memset(m_DataDirectory,0,256);
	Latch = new CLatch();
}

CGame::~CGame() {
	if(Latch){ delete Latch; Latch = NULL;}
}

short CGame::runCycle(stCloneKeenPlus *pCKP)
{
	  int opt = MAINMNU_1PLAYER;
	  int retval;
	  int eseq = 0;
	  int defaultopt = 0;

	  initgamefirsttime(pCKP, Latch->getLatchHeader()->NumSprites);
	  initgame(pCKP);

	  g_pLogFile->ftextOut("Game starting...<br>");

	  if (eseq)
	  {
		  endsequence(pCKP);
		  closeCKP(pCKP);
	  }

	  if(!pCKP->Control.skipstarting)
	  {
		  CIntro Intro;
		  Intro.Render(pCKP);
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
					pCKP->GameData[pCKP->Resources.GameSelected-1].Episode,
					pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory); // Read text from
																				   // and store it at the text pointer

	    	if(textsize > 0)
	    	{
					showPage(text,pCKP,textsize);
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

			opt = mainmenu(pCKP, defaultopt); // Read option from the main menu
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
	        current_demo = 1;
	        initgamefirsttime(pCKP, Latch->getLatchHeader()->NumSprites);
	        loadinggame = 0;
	        playgame_levelmanager(pCKP);
	        break;
	      case MAINMNU_2PLAYER:
	        defaultopt = 0;
	        current_demo = 1;
	        numplayers = 2;
	        initgamefirsttime(pCKP, Latch->getLatchHeader()->NumSprites);
	        loadinggame = 0;
	        playgame_levelmanager(pCKP);
	        break;
	      case MAINMNU_LOADGAME:
	        if (loadslot)
	        {
	           loadinggame = 1;
	           defaultopt = 0;
	           current_demo = 1;
	           numplayers = 1; // here was 2. Why was that? I don't understand
	           initgamefirsttime(pCKP, Latch->getLatchHeader()->NumSprites);
	           playgame_levelmanager(pCKP);
	        }
	        break;

	      case MAINMNU_STORY:
	          pCKP->Control.storyboard=1;
	          break;

	      case MAINMNU_HIGHSCORES:
	    	  CHighScores *pHighscores;
	    	  pHighscores = new CHighScores(pCKP);
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
	    	  pCredit = NULL;
	    	  break;

	      case MAINMNU_TIMEOUT:
	      case MAINMNU_DEMO:
	        retval = play_demo(current_demo, pCKP, Latch->getLatchHeader()->NumSprites);

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
	        { // user canceled out of demo (or intro if at end of demos)
	           // if user selected "demo" have it selected when he comes back
	           if (opt==MAINMNU_DEMO)
	           {
	             defaultopt = MAINMNU_DEMO;
	           }
	        }

	        current_demo++;
	      break;
	      case RESTART_GAME:
	        g_pLogFile->ftextOut("********************<br>");
	        g_pLogFile->ftextOut(" Restarting game...<br>");
	        g_pLogFile->ftextOut("********************<br>\n");
	        cleanup(pCKP);
	        pCKP->shutdown = SHUTDOWN_RESTART;
	        return 0;
	      break;
	      case BACK2MAINMENU:

	      default: break;
	      }

		  if(pCKP->shutdown == SHUTDOWN_NEW_GAME) return 0;

	      g_pLogFile->ftextOut("bottom of game control loop opt=%d crashflag=%d<br>", opt, crashflag);
	      if(pCKP->shutdown == SHUTDOWN_EXIT) break;
	    } while(opt != MAINMNU_QUIT && opt != MAINMNU_NEW_GAME && !crashflag);

		return 0;
}

int CGame::loadResources(unsigned short Episode, char *DataDirectory)
{
	m_Episode = Episode;
	memcpy(m_DataDirectory, DataDirectory, 256);

	// Decode the graphics for the game (EGALATCH, EGASPRIT)
	if (Latch->loadGraphics(Episode, DataDirectory)) return 1;

	loadstrings("strings.dat");

	// Load the sound data
	int ok;
	ok = g_pSound->loadSoundData(m_Episode, DataDirectory);
	if( ok ) return ok;
	return 0;
}

void CGame::preallocateCKP(stCloneKeenPlus *pCKP)
{
	// This function prepares the CKP Structure so that the it is allocated in the memory.
	pCKP->numGames = 0;
	pCKP->Resources.GameSelected = 0;

	TileProperty = NULL;

	pCKP->GameData = NULL;
	pCKP->GameData = new stGameData[1];

	memset(pCKP->GameData, 0, sizeof(stGameData));

	framebyframe = 0;

	demomode = DEMO_NODEMO;
	current_demo = 1;

	memset(&pCKP->Control, 0, sizeof(stControl));

	pCKP->Joystick = NULL;

	acceleratemode = 0;
	g_pVideoDriver->showFPS(false);

	player[0].x = player[0].y = 0;
}




CLatch *CGame::getLatch(void)
{	return Latch;	}
