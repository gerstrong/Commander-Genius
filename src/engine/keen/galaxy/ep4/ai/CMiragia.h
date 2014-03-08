/*
 * CMiragia.h
 *
 *  Created on: 31.07.2010
 *      Author: gerstrong
 *
 *  This class process a object called Miragia
 *  It is a special Level on map in Keen 4 which
 *  appears and dissappears by time
 */

#ifndef CMIRAGIA_H_
#define CMIRAGIA_H_

#include "../../common/CGalaxySpriteObject.h"
#include "CVec.h"

namespace galaxy {

class CMiragia : public CGalaxySpriteObject
{
public:
	CMiragia(CMap *pmap, const Uint16 foeID, const VectorD2<Uint32> &Location);

	/**
	 * Main process cycle
	 */
	void process();

	/**
	 * What happens when an object touches me...
	 */
	void getTouchedBy(CSpriteObject &theObject);

private:
	Uint8 m_step;
	enum {FADE_IN, FADE_OUT} m_fade;
	Uint8 m_waittime;
	bool m_empty;
};

}

#endif /* CMIRAGIA_H_ */
