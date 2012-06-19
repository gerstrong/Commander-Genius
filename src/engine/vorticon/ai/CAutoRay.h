/*
 * CAutoRay.h
 *
 *  Created on: 04.07.2010
 *      Author: gerstrong
 */

#ifndef CAUTORAY_H_
#define CAUTORAY_H_

#include "../../../common/CSpriteObject.h"

#define GUNFIRE_TIMER 128


class CAutoRay : public CSpriteObject
{
public:
	enum stRayAlignment{ VERTICAL, HORIZONTAL };
	CAutoRay(CMap *pmap, Uint32 x, Uint32 y,
			std::vector<CSpriteObject*> &Objvect, stRayAlignment type);
	void process();


private:
	std::vector<CSpriteObject*> &m_Objvect;
	stRayAlignment m_type;
};

#endif /* CAUTORAY_H_ */
