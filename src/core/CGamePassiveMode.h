/*
 * CGamePassiveMode.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#ifndef CGAMEPASSIVEMODE_H_
#define CGAMEPASSIVEMODE_H_

#include "CGameMode.h"

class CGamePassiveMode : public CGameMode {
public:
	CGamePassiveMode(Uint8& episode, Uint8& Numplayers,
					Uint8& Difficulty, std::string& DataDirectory);

	bool init();
	void process();
};

#endif /* CGAMEPASSIVEMODE_H_ */
