/*
 * CPlatformMoveAway.h
 *
 *  Created on: 11.09.2012
 *      Author: gerstrong
 *
 *  This is the code of the dropping platforms.
 *  In Galaxy this platform falls but can return. If it falls too fast it won't return.
 */

#ifndef CPLATFORMMOVEAWAY_H_
#define CPLATFORMMOVEAWAY_H_

#include "CPlatform.h"

namespace galaxy
{

class CPlatformMoveAway : public CPlatform
{
public:
    CPlatformMoveAway(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y,
            const direction_t vertdir, const direction_t hordir, const int actionOffset, const int sprVar);

	bool isNearby(CSpriteObject &theObject);

	void processMoveAway();

	void processMoveBack();

	void processStay();

	/**
	 * \brief Processes the platforms AI. It's just simple moving around of course
	 */
	void process();

private:
    const GsVec2D<Uint32> m_Origin;
	void (CPlatformMoveAway::*mpActionProc)();
	int mSpeed;
};

}

#endif /* CPLATFORMDROP_H_ */
