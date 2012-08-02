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
	CPassiveGalaxy();

	bool init(char mode);
	void process();
	void processIntro();
	void processTitle();

	void (CPassiveGalaxy::*processMode)();

	CBitmap m_BackgroundBitmap;
	bool m_restartVideo;

	SmartPointer<SDL_Surface> mpTextSfc;
};

}

#endif /* CPASSIVE_GALAXY_H_ */
