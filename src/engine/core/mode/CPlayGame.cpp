/*
 * CPlayGameNew.cpp
 *
 *  Created on: 26.05.2010
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "graphics/GsGraphics.h"

#include <widgets/GsMenuController.h>
#include <fileio/KeenFiles.h>

// TODO: I think this should go into the keen directory, maybe, because many elements are keen related, not all though.

CPlayGame::CPlayGame(char level) :
m_endgame(false),
m_Episode(gKeenFiles.exeFile.getEpisode()),
m_Level(level),
m_startgame(false),
m_exitgame(false),
m_gameover(false),
m_restartVideo(false),
mp_option(gBehaviorEngine.m_option)
{
    m_NumSprites = gGraphics.getNumSprites(0);
    m_Gamepath = gKeenFiles.gameDir;
	m_alldead = false;
	m_hideobjects = false;
	m_checkpoint_x = m_checkpoint_y = 0;
	m_checkpointset = false;
	mCamLead = 0;
}

void CPlayGame::pumpEvent(const CEvent *evPtr)
{
    if( dynamic_cast<const EventEndGamePlay*>(evPtr) )
    {
        // The last menu has been removed. Restore back the game status
        gBehaviorEngine.setPause(false);
        gMenuController.clearMenuStack();
        m_endgame = true;
    }
}

void CPlayGame::loadGame()
{
    // This is for the new xml based savegame format since version 1.6.0
    if(loadXMLGameState())
    {
        gLogging.textOut("XML based save state loaded successfully!");
        return;
    }

    // This is for legacy savegame state formats
    loadGameState();
}

// Getters
bool CPlayGame::getEndGame() { return m_endgame; }
bool CPlayGame::getStartGame() { return m_startgame; }
/*char CPlayGame::getEpisode() { return m_Episode; }*/
