/*
 * CGamePlayMode.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#ifndef CGAMEPLAYMODE_H_
#define CGAMEPLAYMODE_H_

#include "CGameMode.h"
#include "engine/playgame/CPlayGame.h"
#include "fileio/CSaveGameController.h"
#include "SmartPointer.h"
#include <string>

class CGamePlayMode : public CGameMode {
public:
	CGamePlayMode( const int Episode, const int Numplayers,
			const int Difficulty, const std::string& DataDirectory, CSaveGameController& SavedGame, bool& show_finale,
			const int startLevel = -1);

	void init();
	void process();

private:
	int m_startLevel;
	bool &m_show_finale;
	int m_Episode;
	Uint8 m_Numplayers;
	Uint8 m_Difficulty;
	std::string m_DataDirectory;
	CSaveGameController m_SavedGame;
	SmartPointer<CPlayGame> mp_PlayGame;
};

#endif /* CGAMEPLAYMODE_H_ */
