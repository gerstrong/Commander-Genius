/*
 * CGameControl.cpp
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#include "CGameControl.h"
#include "fileio/CExeFile.h"
#include "fileio/CPatcher.h"
#include "fileio.h"
#include "CLogFile.h"
#include "sdl/sound/CSound.h"
#include "graphics/effects/CColorMerge.h"

#include "engine/vorticon/CEGAGraphicsVort.h"
#include "engine/vorticon/CPassiveVort.h"
#include "engine/vorticon/playgame/CPlayGameVorticon.h"

#include "engine/galaxy/CEGAGraphicsGalaxy.h"
#include "engine/galaxy/CPassive.h"
#include "engine/galaxy/CPlayGameGalaxy.h"

#include "arguments.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CGameControl::CGameControl() :
mp_GameLauncher(NULL),
mp_PassiveMode(NULL),
mp_PlayGame(NULL)
{
	m_mode = GAMELAUNCHER;
	m_Episode = 0;
	m_ChosenGame = 0;
	m_Numplayers = 0;
	m_endgame = false;

	m_EGAGraphics = NULL;
	m_startLevel = 0;
}

////
// Initialization Routine
/////*/*/*
bool CGameControl::init(int argc, char *argv[])
{
	bool ok;
	std::string argument;
	argument = getArgument( argc, argv, "-game" );

	ok = init();

	if (!ok) return false;

	if(argument != "")
	{
		int chosengame;
		chosengame = atoi(argument.c_str()+strlen("-game"))-1;

		if(chosengame >= 0 && chosengame < mp_GameLauncher->m_numGames)
		{
			mp_GameLauncher->setChosenGame(chosengame);

			// Now check, if a level was also passed as parameter
			argument = getArgument( argc, argv, "-level" );
			m_startLevel = atoi(argument.c_str()+strlen("-level"));
		}
	}

	// Check if finale cutscenes must be shown
	if(getBooleanArgument( argc, argv, "-finale" ))
	{
		int finalegame;
		argument = getArgument( argc, argv, "-finale" );
		finalegame = atoi(argument.c_str()+strlen("-finale"))-1;
		mp_GameLauncher->setChosenGame(finalegame);
		m_startLevel = WM_MAP_NUM;
		m_show_finale = true;
	}
	else m_show_finale = false;

	return ok;
}

bool CGameControl::init(char mode)
{
	m_mode = mode;
	if(m_mode == GAMELAUNCHER)
	{
		// Load the graphics for menu and background.
		mp_GameLauncher = new CGameLauncher();
        if(!mp_GameLauncher->init())
        {
            g_pLogFile->textOut(RED,"The game cannot start, because you are missing game data files.<br>");
            return false;
        }
        // Resources for the main menu
		if(!loadMenuResources())	return false;

		return true;
	}
	else if(m_mode == PASSIVE)
	{
		// Create mp_PassiveMode object used for the screens while Player is not playing
		if(m_Episode >= 4)
			mp_PassiveMode = new galaxy::CPassiveGalaxy( m_ExeFile, m_SavedGame, mp_option );
		else
			mp_PassiveMode = new vorticon::CPassiveVort( m_ExeFile, m_SavedGame, mp_option );

		if( m_endgame == true )
		{
			m_endgame = false;
			// TODO: Overload this function for galaxy
			if( mp_PassiveMode->init(mp_PassiveMode->TITLE) ) return true;
		}
		else
		{
			if( mp_PassiveMode->init() ) return true;
		}
	}
	else if(m_mode == PLAYGAME)
	{
		bool ok = true;

		if(m_Episode >= 4)
			mp_PlayGame = new galaxy::CPlayGameGalaxy( m_ExeFile, m_startLevel,
													m_Numplayers, m_Difficulty,
													mp_option);
		else
		{
			if(m_startLevel == 0) m_startLevel = WORLD_MAP_LEVEL;
			mp_PlayGame = new CPlayGameVorticon(m_ExeFile, m_startLevel,
												m_Numplayers, m_Difficulty,
												mp_option, m_show_finale,
												m_SavedGame, m_TeleporterTable);
		}

		m_show_finale = false; // just show it once!!

		if(m_SavedGame.getCommand() == CSavedGame::LOAD)
			ok &= mp_PlayGame->loadGameState();
		else
		{
			// Create the special merge effect (Fadeout)
			CColorMerge *pColorMergeFX = new CColorMerge(8);

			ok &= mp_PlayGame->init();

			g_pGfxEngine->pushEffectPtr(pColorMergeFX);
		}
		return ok;
	}
	return false;
}

///
// Here the Resources for the menu are loaded. The file used here must exist in the directory
///
bool CGameControl::loadMenuResources()
{
	// Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
	if(m_EGAGraphics) delete m_EGAGraphics; // except for the first start of a game this always happens
	m_EGAGraphics = new CEGAGraphics(0, "."); // It has to be the local data path where the interpreter is
	if(!m_EGAGraphics) return false;
	m_EGAGraphics->loadData();
	return true;
}


///
// This is used for loading all the resources of the game the use has chosen.
// It loads graphics, sound and text into the memory
///
bool CGameControl::loadResources(Uint8 flags)
{
	int version;
	unsigned char *p_exedata;
	unsigned char *p_exeheader;

	m_SavedGame.setGameDirectory(m_DataDirectory);
	m_SavedGame.setEpisode(m_Episode);

	// TODO: not very readable. Check if there is a function for handling that.
	if( m_DataDirectory.size() > 0 && m_DataDirectory[m_DataDirectory.size()-1] != '/' )
		m_DataDirectory += "/";

    version = m_ExeFile.getEXEVersion();
	p_exedata = m_ExeFile.getRawData();
	p_exeheader = m_ExeFile.getHeaderData();

	g_pLogFile->ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", m_Episode, version/100, version%100);
	if( m_Episode == 1 && version == 134) g_pLogFile->ftextOut("This version of the game is not supported!<br>");

	if(p_exeheader == NULL) {
		g_pLogFile->textOut(RED, "CGameControl::loadResources: Could not load data from the EXE File<br>");
		return false;
	}

	// Patch the EXE-File-Data directly in the memory.
	CPatcher Patcher(m_ExeFile);
	Patcher.patchMemory();

	if( m_Episode == 1 || m_Episode == 2 || m_Episode == 3 ) // Vorticon resources
	{
		CTeleporter Teleporter(m_TeleporterTable, m_Episode);
		Teleporter.createTeleporterTable(p_exedata);

		if( (flags & LOADGFX) == LOADGFX )
		{
			// First, retrieve the Tile properties so the tilemap gets properly formatted
			// Important especially for masks
			// Load tile attributes.
			CTileLoader TileLoader( m_Episode, version, p_exedata );
			if( !TileLoader.load() )
			{
				g_pLogFile->textOut(RED, "CGameControl::loadResources: Could not load data for the tiles<br>");
				return false;
			}

			// Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
			if(m_EGAGraphics) delete m_EGAGraphics; // except for the first start of a game this always happens
			m_EGAGraphics = new vorticon::CEGAGraphicsVort(m_Episode, m_DataDirectory);
			if(!m_EGAGraphics) return false;

			m_EGAGraphics->loadData( version, p_exedata );
		}

		if( (flags & LOADSTR) == LOADSTR )
		{
			// load the strings.
			CMessages Messages(p_exedata, m_Episode, version);
			Messages.extractGlobalStrings();
		}

		if( (flags & LOADSND) == LOADSND )
		{
			// Load the sound data
			g_pSound->setGameData(m_ExeFile);
			g_pSound->loadSoundData(m_ExeFile);
		}
		return true;
	}
	else if( m_Episode == 4 || m_Episode == 5 || m_Episode == 6 ) // Galaxy resources
	{
		// TODO: Lots of coding
		if( (flags & LOADGFX) == LOADGFX )
		{
			// Decode the entire graphics for the game (Only EGAGRAPH.CK?)
			SAFE_DELETE(m_EGAGraphics);

			m_EGAGraphics = new galaxy::CEGAGraphicsGalaxy(m_ExeFile); // Path is relative to the data dir
			if(!m_EGAGraphics) return false;

			m_EGAGraphics->loadData();
		}

		if( (flags & LOADSTR) == LOADSTR )
		{
			// load the strings.
		}

		if( (flags & LOADSND) == LOADSND )
		{
			// Load the sound data
		}
	}
	return true;
}


////
// Process Routine
////
// This function is run every time, the Timer says so, through.
void CGameControl::process()
{
	//// First we must know in which mode we are. There are three:
	// The first menu of the game
	if(m_mode == GAMELAUNCHER)
	{
		// Launch the code of the Startmenu here! The one for choosing the games
		mp_GameLauncher->process();
		m_ChosenGame = mp_GameLauncher->getChosengame();

		if( mp_GameLauncher->waschosen() )
		{
			//// Game has been chosen. Launch it!
			// Get the path were to Launch the game
			m_DataDirectory = mp_GameLauncher->getDirectory( m_ChosenGame );

			// We have to check which Episode will be used
			m_Episode = mp_GameLauncher->getEpisode( m_ChosenGame );

			if( m_Episode > 0 ) // The game has to have a valid episode!
			{
			    // Get the EXE-Data of the game and load it into the memory.
			    if(!m_ExeFile.readData(m_Episode, m_DataDirectory))
			    {
					mp_GameLauncher->letchooseagain();
					delete mp_PassiveMode;
					mp_PassiveMode = NULL;
			    }
			    else
			    {
			    	// Load the Resources
			    	if( loadResources() )
			    	{
			    		// Now look if there are any old savegames that need to be converted
			    		CSavedGame savedgames;
			    		savedgames.setGameDirectory(m_DataDirectory);
			    		savedgames.setEpisode(m_Episode);
			    		savedgames.convertAllOldFormats();

			    		if(m_startLevel == 0) // Starts normally
			    		{
			    			if(init(PASSIVE)) cleanup(GAMELAUNCHER);
			    			else
			    			{
			    				mp_GameLauncher->letchooseagain();
			    				delete mp_PassiveMode;
			    				mp_PassiveMode = NULL;
			    			}
			    		}
			    		else // This happens, when a level was passed as argument when launching CG
			    		{
			    			if(init(PLAYGAME)) cleanup(GAMELAUNCHER);
			    			else
			    			{
			    				mp_GameLauncher->letchooseagain();
			    				delete mp_PlayGame;	mp_PlayGame = NULL;
			    			}
			    		}
			    	}
			    }
			}
			else
			{
				mp_GameLauncher->letchooseagain();
				g_pLogFile->textOut(RED,"No Suitable game was detected in this path! Please check its contents!\n");
			}
		}
		else if(mp_GameLauncher->getQuit())
		{
			// User chose exit. So quit...
			m_mode = SHUTDOWN;
		}
	}
	// Intro, Title screen, and demo mode are performed by the passive class CPassive
	else if(m_mode == PASSIVE)
	{
		if(mp_PlayGame != NULL)
		{
			if( mp_PlayGame->getEndGame() )
			{
				mp_PassiveMode->m_mode = PLAYGAME;
			}
		}
		mp_PassiveMode->process();

		// check here what the player chose from the menu over the passive mode.
		// NOTE: Demo is not part of playgame anymore!!
		if(mp_PassiveMode->getchooseGame())
		{
			// TODO: Some of game resources are still not cleaned up here!
			init( GAMELAUNCHER );
			return;
		}

		if(mp_PassiveMode->mustStartGame())
		{
			m_Episode = mp_PassiveMode->getEpisode();
			m_Numplayers = mp_PassiveMode->getNumPlayers();
			m_Difficulty = mp_PassiveMode->getDifficulty();
			m_DataDirectory = mp_PassiveMode->getGamePath();

			init( PLAYGAME );
			delete mp_PassiveMode;
			mp_PassiveMode=NULL;
			return;
		}

		// User wants to exit. Called from the PassiveMode
		if(mp_PassiveMode->getExitEvent())
			m_mode = SHUTDOWN;
	}
	// Here goes the PlayGame Engine
	else if(m_mode == PLAYGAME)
	{
		// The player is playing the game. It also includes scenes like ending
		mp_PlayGame->process();

		if( mp_PlayGame->getEndGame() )
		{
			m_startLevel = 0;
			m_endgame = true;
			init(PASSIVE);
			delete mp_PlayGame; mp_PlayGame = NULL;
		}
		else if( mp_PlayGame->getStartGame() )
		{ // Start another new game
			m_Numplayers = mp_PlayGame->getNumPlayers();
			m_Difficulty = mp_PlayGame->getDifficulty();

			delete mp_PlayGame; mp_PlayGame = NULL;
			init(PLAYGAME);
		}
		else if( mp_PlayGame->getExitEvent() )
			m_mode = SHUTDOWN;
	}
	// That should never happen!
	else
	{
		// Something went wrong here! send warning and load startmenu
		m_mode = GAMELAUNCHER;
	}

}

void CGameControl::cleanup(char mode)
{
	// Check, whatever is active and clean it up!
	if(mode == GAMELAUNCHER && mp_GameLauncher)
	{
		mp_GameLauncher->cleanup();
		delete mp_GameLauncher; mp_GameLauncher = NULL;
	}
	else if(mode == PASSIVE && mp_PassiveMode)
	{
		mp_PassiveMode->cleanup();
		delete mp_PassiveMode; mp_PassiveMode = NULL;
	}
	else if(mode == PLAYGAME && mp_PlayGame)
	{
		mp_PlayGame->cleanup();
		delete mp_PlayGame;	mp_PlayGame = NULL;
	}
}

void CGameControl::cleanupAll()
{
	// Check, whatever is active and clean it up!
	if(mp_GameLauncher)
	{
		mp_GameLauncher->cleanup();
		delete mp_GameLauncher; mp_GameLauncher = NULL;
	}

	if(mp_PassiveMode)
	{
		mp_PassiveMode->cleanup();
		delete mp_PassiveMode; mp_PassiveMode = NULL;
	}

	if(mp_PlayGame)
	{
		mp_PlayGame->cleanup();
		delete mp_PlayGame;	mp_PlayGame = NULL;
	}
}


CGameControl::~CGameControl() {
	cleanupAll();
	if(m_EGAGraphics) delete m_EGAGraphics;
}

