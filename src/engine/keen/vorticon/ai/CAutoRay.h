/*
 * CAutoRay.h
 *
 *  Created on: 04.07.2010
 *      Author: gerstrong
 */

#ifndef CAUTORAY_H_
#define CAUTORAY_H_

#include "../CVorticonSpriteObject.h"

#define GUNFIRE_TIMER 256


class CAutoRay : public CVorticonSpriteObject
{
public:
	enum stRayAlignment{ VERTICAL, HORIZONTAL };
	CAutoRay(std::shared_ptr<CMap> pMap, Uint32 x, Uint32 y, stRayAlignment type);
	bool isNearby(CSpriteObject &theObject);
	void process();


private:
	stRayAlignment m_type;
	bool silent;

    int mGunTimer;
};

#endif /* CAUTORAY_H_ */
