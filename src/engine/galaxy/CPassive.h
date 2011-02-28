/*
 * CPassive.h
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVE_GALAXY_H_
#define CPASSIVE_GALAXY_H_

#include "../CPassive.h"
#include "CMenuGalaxy.h"

namespace galaxy
{

class CPassiveGalaxy : public CPassive
{
public:
	CPassiveGalaxy(CExeFile &Exefile, CSavedGame &SavedGame,
			Uint8& difficulty);

	bool init(char mode);
	void process();
	void processIntro();
	void processTitle();
	void processMenu();
	void cleanup();

	void (CPassiveGalaxy::*processMode)();

	virtual ~CPassiveGalaxy();

	CBitmap m_BackgroundBitmap;
	CMenuGalaxy *mp_Menu;
	bool m_restartVideo;
};

}

#endif /* CPASSIVE_GALAXY_H_ */
