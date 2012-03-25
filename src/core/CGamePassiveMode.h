/*
 * CGamePassiveMode.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#ifndef CGAMEPASSIVEMODE_H_
#define CGAMEPASSIVEMODE_H_

#include "CGameMode.h"
#include "engine/CPassive.h"
#include "SmartPointer.h"

class CGamePassiveMode : public CGameMode {
public:
	CGamePassiveMode(const std::string& DataDirectory, const int& Episode);

	void init();
	void process();

private:
	SmartPointer<CPassive> mp_Passive;

	const std::string m_DataDirectory;
	const int m_Episode;
	bool m_Endgame;
	CSaveGameController m_SavedGame;
	int m_Difficulty;
};

#endif /* CGAMEPASSIVEMODE_H_ */
