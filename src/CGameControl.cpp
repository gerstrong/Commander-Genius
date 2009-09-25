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

CGameControl::CGameControl() {
	m_mode = GAMELAUNCHER;
	m_Episode = 0;
	m_ChosenGame = 0;

	m_TileLoader = NULL;
    m_EGAGraphics = NULL;
    m_Messages = NULL;
}

CGameControl::~CGameControl() {
	if(m_EGAGraphics) delete m_EGAGraphics;
	if(m_Messages) delete m_Messages;
	if(m_TileLoader) delete m_TileLoader; // Is this one really needed here?
}

////
// Initialization Routine
////
bool CGameControl::init()
{
	// Resources for the main menu.
	if(!loadResources(1, "games"))
		return false;

	// Load the graphics for menu and background.
	mp_GameLauncher = new CGameLauncher();
	return mp_GameLauncher->init();
}

bool CGameControl::loadResources(unsigned short Episode, const std::string& DataDirectory)
{
	m_Episode = Episode;
	m_DataDirectory = DataDirectory;

	if( m_DataDirectory.size() > 0 && m_DataDirectory[m_DataDirectory.size()-1] != '/' )
		m_DataDirectory += "/";

    // Get the EXE of the game and decompress it if needed.
    CExeFile *ExeFile = new CExeFile(Episode, DataDirectory);
    if(!ExeFile->readData()) return false;
    int version = ExeFile->getEXEVersion();

	g_pLogFile->ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", Episode, version/100,version%100);
	if(version == 134) g_pLogFile->ftextOut("This version of the game is not supported!<br>");

    // Load tile attributes.
	if(m_TileLoader) delete m_TileLoader;

	if(ExeFile->getData() == NULL) {
		g_pLogFile->textOut(RED, "CGame::loadResources: Could not load data from the EXE File<br>");
		delete ExeFile;
		return false;
	}

	m_TileLoader = new CTileLoader(Episode, ExeFile->getEXEVersion(), ExeFile->getData());

	// Patch the EXE-File-Data directly in the memory.
	CPatcher *Patcher = new CPatcher(Episode, ExeFile->getEXEVersion(), ExeFile->getData(), DataDirectory);
	Patcher->patchMemory();
	delete Patcher;

	if(!m_TileLoader->load()) {
		g_pLogFile->textOut(RED, "CGame::loadResources: Could not load data for the tiles<br>");
		delete ExeFile;
		return false;
	}

	// Decode the entire graphics for the game (EGALATCH, EGASPRIT)
	if(m_EGAGraphics) delete m_EGAGraphics;
    m_EGAGraphics = new CEGAGraphics(Episode, DataDirectory); // Path is relative to the data dir
    if(!m_EGAGraphics) return false;

    m_EGAGraphics->loadData();

    // load the strings. TODO: After that this one will replace loadstrings
    //m_Messages = new CMessages();    delete ExeFile;
    //m_Messages->readData(Episode, version, DataDirectory);
	loadstrings();

	delete ExeFile;

	// Load the sound data
	bool ok = g_pSound->loadSoundData(m_Episode, DataDirectory);
	if( !ok ) return false;
	return true;
}


////
// Process Routine
////
// This function is run every time, the Timer says so, through.
void CGameControl::process()
{
	// First we must know in which mode we are. There are three:

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
			m_DataDirectory = "games/" + mp_GameLauncher->getDirectory( m_ChosenGame );

			// We have to check which Episode is used
			m_Episode = mp_GameLauncher->retrievetEpisode( m_ChosenGame );

			if( m_Episode > 0 ) // If not, don't nothing, because the episode is not valid!
			{
				// Load the Resources
				loadResources( m_Episode, m_DataDirectory );

				// Unload the GameLauncher
				delete mp_GameLauncher;

				// Create mp_PassiveMode object used for the screens while Player is not playing
				mp_PassiveMode = new CPassive( m_Episode, m_DataDirectory );
				if( mp_PassiveMode->init() )	m_mode = PASSIVE;
				else
				{
					mp_GameLauncher->letchooseagain();
					delete mp_PassiveMode;
				}
			}
			else
			{
				mp_GameLauncher->letchooseagain();
				g_pLogFile->textOut(RED,"This Path is erroneous! No Suitable game was detected! Please check it!\n");
			}
		}
		else if(mp_GameLauncher->getQuit())
		{
			// User chose exit. So quit...
			m_mode = SHUTDOWN;
		}
	}
	// Intro, Title screen, and demo mode are performed by the passive Class
	else if(m_mode == PASSIVE)
	{
		// Demo also means intro and titlescreen. They are switched through

		mp_PassiveMode->process();

		// User wants to exit. Called from the PassiveMode
		if(mp_PassiveMode->getExitEvent())
			m_mode = SHUTDOWN;
	}
	// Here goes the Playgame Engine
	else if(m_mode == PLAYGAME)
	{
		// The player is playing the game. It also includes scenes like ending
	}
	// That should never happen!
	else
	{
		// Something went wrong here! send warning and load startmenu
		m_mode = GAMELAUNCHER;
	}

}

void CGameControl::cleanup()
{
	if(m_mode == GAMELAUNCHER)
	{
		// Launch the code of the Startmenu here! The one for choosing the games
		mp_GameLauncher->cleanup();
		delete mp_GameLauncher;
	}
	else if(m_mode == PASSIVE)
	{
		mp_PassiveMode->cleanup();
		delete mp_PassiveMode;
	}
}

