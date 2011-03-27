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
#include "fileio/CSavedGame.h"
#include "SmartPointer.h"
#include <string>

class CGamePlayMode : public CGameMode {
public:
	CGamePlayMode( const int Episode, const int Numplayers,
			const int Difficulty, const std::string& DataDirectory, CSavedGame& SavedGame);

	void init();
	void process();

private:
	int m_startLevel;
	bool m_show_finale;
	int m_Episode;
	Uint8 m_Numplayers;
	Uint8 m_Difficulty;
	std::string m_DataDirectory;
	CSavedGame m_SavedGame;
	SmartPointer<CPlayGame> mp_PlayGame;
};

#endif /* CGAMEPLAYMODE_H_ */
