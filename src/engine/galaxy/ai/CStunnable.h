/*
 * CStunnable.h
 *
 *  Created on: 23.06.2011
 *      Author: gerstrong
 */

#ifndef CSTUNNABLE_H_
#define CSTUNNABLE_H_

#include "common/CObject.h"

class CStunnable: public CObject {
public:
	CStunnable(	const CMap *pmap,
			const Uint32 x,
			const Uint32 y,
			const object_t type );
};

#endif /* CSTUNNABLE_H_ */
