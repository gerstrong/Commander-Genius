/*
 * CAutoRay.h
 *
 *  Created on: 04.07.2010
 *      Author: gerstrong
 */

#ifndef CAUTORAY_H_
#define CAUTORAY_H_

#include "../../../common/CObject.h"

#define GUNFIRE_TIMER 128


class CAutoRay : public CObject
{
public:
	enum stRayAlignment{ VERTICAL, HORIZONTAL };
	CAutoRay(CMap *pmap, Uint32 x, Uint32 y,
			std::vector<CObject*> &Objvect, stRayAlignment type);
	void process();


private:
	std::vector<CObject*> &m_Objvect;
	stRayAlignment m_type;
};

#endif /* CAUTORAY_H_ */
