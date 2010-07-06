/*
 * CAutoRay.h
 *
 *  Created on: 04.07.2010
 *      Author: gerstrong
 */

#ifndef CAUTORAY_H_
#define CAUTORAY_H_

#include "../../../common/CObject.h"

#define GUNFIRE_TIMER 180


class CAutoRay : public CObject
{
public:
	enum stRayAlignment{ VERTICAL, HORIZONTAL };
	CAutoRay(CMap *p_map, std::vector<CObject*> &Objvect, stRayAlignment type);
	void process();


private:
	unsigned int timer;
	int m_gunfiretimer;
	std::vector<CObject*> &m_Objvect;
	stRayAlignment m_type;
};

#endif /* CAUTORAY_H_ */
