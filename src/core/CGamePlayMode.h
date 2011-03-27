/*
 * CGamePlayMode.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#ifndef CGAMEPLAYMODE_H_
#define CGAMEPLAYMODE_H_

#include "CGameMode.h"

class CGamePlayMode : public CGameMode {
public:
	CGamePlayMode(bool& show_finale, Uint8& episode,
			Uint8& Numplayers, Uint8& Difficulty,
			std::string& DataDirectory);
	virtual ~CGamePlayMode();

	bool init();
	void process();

private:
	int m_startLevel;
	bool& m_show_finale;
};

#endif /* CGAMEPLAYMODE_H_ */
