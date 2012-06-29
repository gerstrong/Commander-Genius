/*
 * CFlag.h
 *
 *  Created on: 30.07.2010
 *      Author: gerstrong
 *
 *  This class represents the flag sign object
 *  which is shown and performed when Keen finished a Level
 */

#ifndef CFLAG_H_
#define CFLAG_H_

#include "engine/galaxy/CGalaxySpriteObject.h"
#include "CVec.h"

namespace galaxy  {

class CFlag : public CGalaxySpriteObject
{
public:
	static const unsigned int FOE_ID = 500;

	CFlag(CMap *pmap, const VectorD2<Uint32> &Location,
			const VectorD2<Uint32> &Destination);

	// The flag is flying when player throws it.
	// It's waving all the time being stuck on the pole.
	void process();
	void processFlying();
	void processWaving();

private:
	// Where the flag/sign will pop in
	VectorD2<Uint32> m_location;
	VectorD2<Uint32> m_destination;
	Uint16 m_baseframe;
	void (CFlag::*processState)();

};

}

#endif /* CFLAG_H_ */
