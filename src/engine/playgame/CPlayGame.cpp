/*
 * CPlayGameNew.cpp
 *
 *  Created on: 26.05.2010
 *      Author: gerstrong
 */

#include "CPlayGame.h"

CPlayGame::CPlayGame(CExeFile &ExeFile, char level,
		 char numplayers, char difficulty) :
m_NumPlayers(numplayers),
m_endgame(false),
m_ExeFile(ExeFile),
m_Episode(ExeFile.getEpisode()),
m_Level(level),
m_startgame(false),
m_exitgame(false),
m_Difficulty(difficulty)
{}


// Getters
bool CPlayGame::getEndGame() { return m_endgame; }
bool CPlayGame::getStartGame() { return m_startgame; }
bool CPlayGame::getExitEvent() { return m_exitgame; }
char CPlayGame::getEpisode() { return m_Episode; }
char CPlayGame::getNumPlayers() { return m_NumPlayers; }
char CPlayGame::getDifficulty() { return m_Difficulty; }


CPlayGame::~CPlayGame() {
	// TODO Auto-generated destructor stub
}
