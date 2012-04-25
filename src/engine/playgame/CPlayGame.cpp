/*
 * CPlayGameNew.cpp
 *
 *  Created on: 26.05.2010
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "graphics/CGfxEngine.h"

CPlayGame::CPlayGame(CExeFile &ExeFile, char level,
		 char numplayers, Uint8& difficulty) :
m_NumPlayers(numplayers),
m_endgame(false),
m_ExeFile(ExeFile),
m_Episode(ExeFile.getEpisode()),
m_Level(level),
m_startgame(false),
m_exitgame(false),
m_Difficulty(difficulty),
m_gameover(false),
m_restartVideo(false),
mp_option(g_pBehaviorEngine->m_option)
{
	m_NumSprites = g_pGfxEngine->getNumSprites();
	m_Gamepath = ExeFile.getDataDirectory();
	m_alldead = false;
	m_hideobjects = false;
	m_checkpoint_x = m_checkpoint_y = 0;
	m_checkpointset = false;
}


// Getters
bool CPlayGame::getEndGame() { return m_endgame; }
bool CPlayGame::getStartGame() { return m_startgame; }
bool CPlayGame::getExitEvent() { return m_exitgame; }
char CPlayGame::getEpisode() { return m_Episode; }
char CPlayGame::getNumPlayers() { return m_NumPlayers; }
char CPlayGame::getDifficulty() { return m_Difficulty; }
