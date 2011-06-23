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

#include "common/CObject.h"
#include "CVec.h"

namespace galaxy {

class CMiragia : public CObject {
public:
	CMiragia(CMap *pmap, const VectorD2<Uint32> &Location);
	void process();

private:
	Uint8 m_step;
	enum {FADE_IN, FADE_OUT} m_fade;
	Uint8 m_waittime;
};

}

#endif /* CMIRAGIA_H_ */
