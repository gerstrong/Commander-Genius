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
#include "fileio/CExeFile.h"
#include "fileio/CTileLoader.h"
#include "fileio/CPatcher.h"
#include "sdl/sound/CSound.h"
#include "sdl/CVideoDriver.h"
#include "vorticon/COrderingInfo.h"

CGame::CGame() {
	m_Episode = 0;

	TileLoader = NULL;
    EGAGraphics = NULL;
    m_Messages = NULL;
}

CGame::~CGame() {
	if(EGAGraphics) delete EGAGraphics;
	if(TileLoader) delete TileLoader;
	if(m_Messages) delete m_Messages;
}

short CGame::runCycle(stCloneKeenPlus *pCKP)
{
	  int opt = MAINMNU_1PLAYER;
	  int retval;
	  int defaultopt = 0;

	  initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
	  initgame( &(pCKP->Control.levelcontrol) );

	  g_pLogFile->ftextOut("Game starting...<br>");

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
	        initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
	        loadinggame = 0;
	        playgame_levelmanager(pCKP);
	        break;
	      case MAINMNU_2PLAYER:
	        defaultopt = 0;
	        current_demo = 1;
	        numplayers = 2;
	        initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
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
	           initgamefirsttime(pCKP, EGAGraphics->getNumSprites());
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
	    	  break;

	      case MAINMNU_ORDERING_INFO:
	    	  COrderingInfo *OrderingInfo;
	    	  OrderingInfo = new COrderingInfo(pCKP->Control.levelcontrol.episode,
	    			  pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory);
	    	  OrderingInfo->Render(pCKP);
	    	  delete OrderingInfo;
	    	  break;

	      case MAINMNU_TIMEOUT:
	      case MAINMNU_DEMO:
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

bool CGame::loadResources(unsigned short Episode, const std::string& DataDirectory)
{
	m_Episode = Episode;
	m_DataDirectory = DataDirectory;

	if( m_DataDirectory.size() > 0 && m_DataDirectory[m_DataDirectory.size()-1] != '/' )
		m_DataDirectory += "/";

	// Decode the entire graphics for the game (EGALATCH, EGASPRIT)
    EGAGraphics = new CEGAGraphics(Episode, DataDirectory); // Path is relative to the data dir
    if(!EGAGraphics) return 1;

    EGAGraphics->loadData();

    // Get the EXE of the game and decompress it if needed.
    CExeFile *ExeFile = new CExeFile(Episode, DataDirectory);
    ExeFile->readData();
    int version = ExeFile->getEXEVersion();
	g_pLogFile->ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", Episode, version/100,version%100);
	if(version == 134) g_pLogFile->ftextOut("This version of the game is not supported!<br>");

    // Patch the EXE-File-Data directly in the memory.
	CPatcher *Patcher = new CPatcher(Episode, ExeFile->getEXEVersion(), ExeFile->getData(), DataDirectory);
	Patcher->patchMemory();
	delete Patcher;

    // Load tile attributes.
	if(TileLoader) delete TileLoader;
	
	if(ExeFile->getData() == NULL) {
		g_pLogFile->textOut(RED, "CGame::loadResources: Could not load data out of EXE<br>");
		delete ExeFile;
		return false;
	}
	
	TileLoader = new CTileLoader(Episode, ExeFile->getEXEVersion(), ExeFile->getData());
	if(!TileLoader->load()) {
		g_pLogFile->textOut(RED, "CGame::loadResources: Could not load data with TileLoader<br>");
		delete ExeFile;
		return false;
	}
	
    // load the strings. TODO: After that this one will replace loadstrings
    //m_Messages = new CMessages();    delete ExeFile;
    //m_Messages->readData(char *buf, int episode, int version);
	loadstrings("strings.dat");

    delete ExeFile;

	// Load the sound data
	bool ok = g_pSound->loadSoundData(m_Episode, DataDirectory);
	if( !ok ) return false;
	return true;
}

void CGame::freeResources(void)
{
	if(EGAGraphics) { delete EGAGraphics; EGAGraphics = NULL; }
}

void CGame::preallocateCKP(stCloneKeenPlus *pCKP)
{
	// This function prepares the CKP Structure so that the it is allocated in the memory.
	pCKP->numGames = 0;
	pCKP->Resources.GameSelected = 0;

	pCKP->GameData = NULL;
	pCKP->GameData = new stGameData[1];

	pCKP->Control.levelcontrol.demomode = DEMO_NODEMO;
	current_demo = 1;

	pCKP->Joystick = NULL;

	acceleratemode = 0;
	g_pVideoDriver->showFPS(false);

	player[0].x = player[0].y = 0;
}
