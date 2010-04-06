/*
 * CPassive.h
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVE_GALAXY_H_
#define CPASSIVE_GALAXY_H_

#include "../CPassive.h"

namespace galaxy
{

class CPassiveGalaxy : public CPassive
{
public:
	CPassiveGalaxy(char Episode, std::string DataDirectory,
			CSavedGame &SavedGame, stOption *p_Option);

	bool init(char mode = INTRO);
	void process();
	void processIntro();
	void processTitle();
	void processMenu();
	void cleanup();

	void (CPassiveGalaxy::*processMode)();

	virtual ~CPassiveGalaxy();
};

}

#endif /* CPASSIVE_GALAXY_H_ */
