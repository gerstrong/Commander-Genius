/*
 * CPlayGameNew.cpp
 *
 *  Created on: 26.05.2010
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "graphics/GsGraphics.h"

CPlayGame::CPlayGame(CExeFile &ExeFile, char level) :
m_endgame(false),
m_ExeFile(ExeFile),
m_Episode(ExeFile.getEpisode()),
m_Level(level),
m_startgame(false),
m_exitgame(false),
m_gameover(false),
m_restartVideo(false),
mp_option(g_pBehaviorEngine->m_option)
{
    m_NumSprites = gGraphics.getNumSprites(0);
	m_Gamepath = ExeFile.getDataDirectory();
	m_alldead = false;
	m_hideobjects = false;
	m_checkpoint_x = m_checkpoint_y = 0;
	m_checkpointset = false;
	mCamLead = 0;
}

void CPlayGame::loadGame()
{
    // This is for the new xml based savegame format since version 1.6.0
    if(loadXMLGameState())
        return;

    // This is for legacy savegame state formats
    loadGameState();
}

// Getters
bool CPlayGame::getEndGame() { return m_endgame; }
bool CPlayGame::getStartGame() { return m_startgame; }
bool CPlayGame::getExitEvent() { return m_exitgame; }
char CPlayGame::getEpisode() { return m_Episode; }
