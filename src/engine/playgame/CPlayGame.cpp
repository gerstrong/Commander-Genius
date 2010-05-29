/*
 * CPlayGameNew.cpp
 *
 *  Created on: 26.05.2010
 *      Author: gerstrong
 */

#include "CPlayGame.h"

CPlayGame::CPlayGame() {
	// TODO Auto-generated constructor stub

}


// Getters
bool CPlayGame::isFinished() { return m_finished; }
bool CPlayGame::getEndGame() { return m_endgame; }
bool CPlayGame::getStartGame() { return m_startgame; }
bool CPlayGame::getExitEvent() { return m_exitgame; }
char CPlayGame::getEpisode() { return m_Episode; }
char CPlayGame::getNumPlayers() { return m_NumPlayers; }
char CPlayGame::getDifficulty() { return m_Difficulty; }


CPlayGame::~CPlayGame() {
	// TODO Auto-generated destructor stub
}
