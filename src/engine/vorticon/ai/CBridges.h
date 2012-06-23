/*
 * CBridges.h
 *
 *  Created on: 10.07.2010
 *      Author: gerstrong
 */

#ifndef CBRIDGES_H_
#define CBRIDGES_H_

#include "engine/vorticon/CVorticonSpriteObject.h"


class CBridges : public CVorticonSpriteObject
{
public:
	CBridges(CMap *pmap, Uint32 x, Uint32 y, Uint32 platx, Uint32 platy);
	void process();
	void extend();
	void retract();
	virtual ~CBridges();

private:
	enum{
		EXTEND,
		RETRACT
	}m_state;
	int timer;

	Uint32 m_platx;
	Uint32 m_platy;

	direction_t m_dir;
	static unsigned int m_bgtile;
};

#endif /* CBRIDGES_H_ */
