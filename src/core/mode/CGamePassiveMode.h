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
//#include "engine/infoscenes/CInfoScene.h"
#include "common/CMap.h"
#include <memory>

class CGamePassiveMode : public CGameMode
{
public:
	CGamePassiveMode();

	void init();
    void ponder(const float deltaT);
    void render();

private:
	std::unique_ptr<CPassive> mpPassive;

	CSaveGameController m_SavedGame;
	int m_Difficulty;

};

#endif /* CGAMEPASSIVEMODE_H_ */
