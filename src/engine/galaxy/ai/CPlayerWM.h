/*
 * CPlayer.h
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#ifndef CPLAYERWM_H_
#define CPLAYERWM_H_

#include "../../../common/CObject.h"

namespace galaxy {

class CPlayerWM : public CObject {
public:
	CPlayerWM(CMap *pmap, Uint32 x, Uint32 y);
	virtual ~CPlayerWM();
};

}

#endif /* CPLAYERWM_H_ */
