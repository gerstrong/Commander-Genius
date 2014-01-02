/*
 * GsApp.cpp
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 *  This Game-engine stripes down the main function
 *  and provides more dynamic control over the game engine itself
 * 
 *  It also manages the load of drivers and main game cycle
 */

#include "GsApp.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "sdl/sound/CSound.h"
#include <lib/base/GsTimer.h>
#include "common/CSettings.h"
#include <lib/base/GsLogging.h>
#include "common/Menu/CMenuController.h"

#include "graphics/CGfxEngine.h"

GsApp::GsApp() :
m_firsttime(false),
mAppState(m_firsttime)
{}


///////////////////////////////
// Cleanup Game Engine here! //
///////////////////////////////
/**
 * \brief  			This function cleans up all the used engines and the singleton
 * 					classes that were used during the program.
 * 					This can happen at the end of the program
 * 					or when an engine may be changed.
 */
GsApp::~GsApp()
{
	gpMenuController->emptyMenuStack();
	g_pInput->Del();
	g_pSound->destroy();
	g_pVideoDriver->Del();
	g_pTimer->Del();
	g_pLogFile->Del();
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
 */
bool GsApp::init(int argc, char *argv[])
{
	// Check if there are settings on the PC, otherwise use defaults.
	if(!g_pSettings->loadDrvCfg())
	{
		m_firsttime = true;
		g_pLogFile->textOut(RED,"First time message: CG didn't find the driver config file. ");
		g_pLogFile->textOut(RED,"However, it generated some default values and will save them now.\n");
		g_pSettings->saveDrvCfg();
	}

	if(!g_pSettings->loadGameOptions())
	{
	    g_pSettings->loadDefaultGameCfg();
	}

	// Setup the Hardware using the settings we have loaded
	g_pLogFile->textOut(GREEN,"Loading hardware settings...<br>");
	if(!loadCKPDrivers())
	{
		g_pLogFile->textOut(RED,"The game cannot start, because you do not meet the hardware requirements.<br>");
		return false;
	}
	
	// Initialize the way the launcher is started
    if(!mAppState.init(argc, argv))	return false;
	
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
bool GsApp::loadCKPDrivers()
{
	// Init graphics
	if (!g_pVideoDriver->start()) return false;
	
	// Init the sound
	g_pSound->init();
	
	return true;
}

/////////////////////////////
// Process Game Engine here! //
/////////////////////////////
/**
 * \brief  	This is the main run cycle of the game,
 * 		no matter what happens in the game logic or
 * 		which engine is chosen, it always get to this point
 * 		Mainly timer and logic processes are performed here.
 */
void GsApp::run()
{
    float acc = 0.0f;
    float start = 0.0f;
    float elapsed = 0.0f;
    float total_elapsed = 0.0f;
    float curr = 0.0f;
    int counter = 0;

    while(1)
    {
        const float logicLatency = g_pTimer->LogicLatency();
        const float renderLatency = g_pTimer->RenderLatency();

        curr = timerTicks();

        if(g_pTimer->resetLogicSingal())
            start = curr;

        elapsed = curr - start;        

        start = timerTicks();

        // GameState previous;
        // GameState current;

        /*if ( elapsed > 0.25 )
             elapsed = 0.25;	  // note: max frame time to avoid spiral of death
        */

        acc += elapsed;

        // Perform the game cycle
        while( acc > logicLatency )
        {
            // Poll Inputs
            g_pInput->pollEvents();                        

            // Process App Events
            g_pBehaviorEngine->EventList().processSinks();

            // Ponder Game Control
            mAppState.ponder();

            /*
              previousState = currentState;
              integrate( currentState, logicLatency );
            */

            // Apply graphical effects if any. It does not render, it only prepares for the rendering task.
            g_pGfxEngine->ponder();

            acc -= logicLatency;
        }

        // Now we render the whole GameControl Object to the blit surface
        mAppState.render(0);

        // Apply graphical effects if any.
        g_pGfxEngine->render();

        // Pass all the surfaces to one. Some special surfaces are used and are collected here
        g_pVideoDriver->collectSurfaces();

        // Now you really render the screen
        // When enabled, it also will apply Filters
        g_pVideoDriver->updateDisplay();

        /*
         const double alpha = acc / logicLatency;

         State state = currentState*alpha + previousState * ( 1.0 - alpha );

         render( state );
        */

        elapsed = timerTicks() - start;
        total_elapsed += elapsed;

        if( mAppState.mustShutdown() )
            break;

        int waitTime = renderLatency - elapsed;

        // wait time remaining in current loop
        if( waitTime > 0 )
            timerDelay(waitTime);

        total_elapsed += static_cast<float>(waitTime);

        // This will refresh the fps display, so it stays readable and calculates an average value.
        counter++;
        if(counter >= 100)
        {
            counter = 0;
            g_pTimer->setTimeforLastLoop(total_elapsed/100.0f);
            total_elapsed = 0.0f;
        }
    }
}

