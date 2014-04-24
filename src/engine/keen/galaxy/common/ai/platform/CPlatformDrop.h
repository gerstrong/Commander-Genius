/*
 * CPlatformDrop.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 *
 *  This is the code of the dropping platforms.
 *  In Galaxy this platform falls but can return. If it falls too fast it won't return.
 */

#ifndef CPLATFORMDROP_H_
#define CPLATFORMDROP_H_

#include "CPlatform.h"

namespace galaxy
{

class CPlatformDrop : public CPlatform
{
public:
    CPlatformDrop(CMap *pmap, const Uint16 foeID,
              const Uint32 x, const Uint32 y, const int actionOff, const int sprVar);

    // Most accurate platform drop movement, hopefully
    void procPlatdrop();

	/**
	 * \brief Processes the platforms AI. It's just simple moving around of course
	 */
	void process();

private:
	int m_delay_for_drop;
	int m_drop_speed;

    // Times the cycle has to run through until Speed is raised
    int dropSpeedAcc;

    // Speed at what the Platform might return to the original position
    int hoverSpeed;

	const Vector2D<Uint32> m_Origin;

    bool mAllowReturn;
};

}

#endif /* CPLATFORMDROP_H_ */
