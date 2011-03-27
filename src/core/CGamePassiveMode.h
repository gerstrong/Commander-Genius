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


class CGamePassiveMode : public CGameMode {
public:
	CGamePassiveMode(Uint8& episode, Uint8& Numplayers,
					Uint8& Difficulty, std::string& DataDirectory);

	bool init();
	void process();

private:
	CPassive *mp_PassiveMode;

};

#endif /* CGAMEPASSIVEMODE_H_ */
