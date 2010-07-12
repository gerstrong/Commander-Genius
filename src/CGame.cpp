/*
 * CGame.cpp
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 *  This Game-engine stripes down the main function
 *  and provides more dynamic control over the game
 */

#include <stdlib.h>

#include "CGame.h"
#include "CLogFile.h"
#include "sdl/CSettings.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"

CGame::CGame() {
}

//////////////////////////////////
// Initialize Game Engine here! //
//////////////////////////////////

/**
 * \brief  This is the function where CG beings
 *
 * \param	argc	number of arguments
 * \param	argv	pointer to  char arrays where
 * 					where the passed arguments are stored
 * 					in the process
 * \return			If the function could setup
 * 					the game, it will return true, else it
 * 					will be false.
 *
 */
bool CGame::init(int argc, char *argv[])
{
	CSettings Settings(m_option);
	m_Engine.mp_option = m_option;
	
	// Check if there are settings on the PC, otherwise use defaults.
	if(!Settings.loadDrvCfg())
	{
		g_pLogFile->textOut(PURPLE,"First time message: CKP didn't find the driver config file. However, it generated some default values and will save them now.<br>");
		Settings.saveDrvCfg();
	}

	if(!Settings.loadGameCfg())	Settings.loadDefaultGameCfg();

	// Setup the Hardware using the settings we have loaded
	g_pLogFile->textOut(GREEN,"Loading hardware settings...<br>");
	if(!loadCKPDrivers())
	{
		g_pLogFile->textOut(RED,"The game cannot start, because you do not meet the hardware requirements.<br>");
		return false;
	}
	
	// Initialize the way the launcher is started
	if(!m_Engine.init(argc, argv))	return false;
	
	return true;
}

/**
 * \brief  			This function will try to load the hardware
 * 					resources, which are the video driver and the sound.
 * 					Input stuff is only loaded, when CG needs it, what mostly
 * 					happens, when it has started.
 *
 * \return			If the function could load
 * 					the drivers, it will return true, else it
 * 					will be false.
 */
// Load the driver needed to start the game
bool CGame::loadCKPDrivers()
{
	// initialize/activate all drivers
	g_pLogFile->ftextOut("Starting graphics driver...<br>");
	
	// The graphics are very important, if the other subsystems fail, warn but continue
	if (!g_pVideoDriver->start()) return false;
	
	g_pLogFile->ftextOut("Starting sound driver...<br>");
	g_pSound->init();
	
	// g_pInput is started automatically when needed!

	return true;
}

/////////////////////////////
// Process Game Engine here! //
/////////////////////////////
/**
 * \brief  			This is the main run cycle of the game,
 * 					no matter what happens in the game logic or
 * 					which engine is chosen, it always get to this point
 * 					Mainly timer and logic processes are performed here.
 */
void CGame::run()
{
	do
	{
        // Perform game logic
        if (g_pTimer->TimeToLogic()) {
            // Poll Inputs
            g_pInput->pollEvents();
            // Process Game Control
            m_Engine.process();
        }
		
        // Render the Screen
        if (g_pTimer->TimeToRender()) {
        	// Pass all the surfaces to one
        	g_pVideoDriver->collectSurfaces();
        	// Apply graphical effects if any
        	g_pGfxEngine->process();
			// Now you really render the screen
        	// When enabled, it also will apply Filters
            g_pVideoDriver->updateScreen();
        }
		
        // delay time remaining in current loop
        g_pTimer->TimeToDelay();

	} while(!m_Engine.mustShutdown() && !g_pInput->getExitEvent());
}

///////////////////////////////
// Cleanup Game Engine here! //
///////////////////////////////
/**
 * \brief  			This function cleans up all the used engines and the singleton
 * 					classes that were used during the program. This can happen at the end of the program
 * 					or when an engine may be changed.
 */
void CGame::cleanup()
{
	m_Engine.cleanupAll();
	
    g_pInput->Del();
    g_pSound->Del();
    g_pVideoDriver->Del();
    g_pTimer->Del();
}

CGame::~CGame() {
}

