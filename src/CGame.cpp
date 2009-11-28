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
#include "common/CMenu.h"
#include "CLogFile.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"

CGame::CGame() {
}

//////////////////////////////////
// Initialize Game Engine here! //
//////////////////////////////////
bool CGame::init(int argc, char *argv[])
{
	CSettings Settings(m_option);
	m_GameControl.mp_option = m_option;
	
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
	if(!m_GameControl.init(argc, argv))	return false;
	
	return true;
}

// Load the driver needed to start the game
bool CGame::loadCKPDrivers()
{
	// initialize/activate all drivers
	g_pLogFile->ftextOut("Starting graphics driver...<br>");
	
	// The graphics are very important, if the other subsystems fail, warn but continue
	if (!g_pVideoDriver->start()) return false;
	
	g_pLogFile->ftextOut("Starting sound driver...<br>");
	g_pSound->init();
	
	g_pLogFile->ftextOut("Starting the input driver...<br>");
	g_pInput->resetControls();
	
	return true;
}


/////////////////////////////
// Process Game Engine here! //
/////////////////////////////
void CGame::run()
{
	do
	{
        // Perform game logic
        if (g_pTimer->TimeToLogic()) {
            // Poll Inputs
            g_pInput->pollEvents();
            // Process Game Control
            m_GameControl.process();
        }
		
        // Render the Screen
        if (g_pTimer->TimeToRender()) {
            g_pVideoDriver->update_screen();
        }
		
        // delay time remaining in current loop
        g_pTimer->TimeToDelay();
	} while(!m_GameControl.mustShutdown());
}

///////////////////////////////
// Cleanup Game Engine here! //
///////////////////////////////
void CGame::cleanup()
{
	m_GameControl.cleanup();
	
    g_pInput->Del();
    g_pSound->Del();
    g_pVideoDriver->Del();
    g_pTimer->Del();
}

CGame::~CGame() {
}

