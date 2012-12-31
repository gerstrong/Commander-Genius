/*
 * CVarPlatform.h
 *
 *  Created on: Sep 6, 2012
 *      Author: gerstrong
 */

#ifndef CVARPLATFORM_H_
#define CVARPLATFORM_H_

#include "CPlatform.h"
#include "CVec.h"

namespace galaxy {


class CVarPlatform : public CPlatform
{
public:
	CVarPlatform(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y,
			const direction_t horidir, 
			const direction_t vertdir, 
			const int actionOffset);

	void process();

private:
	void detectNextTarget(const VectorD2<int> &oldTarget);
    
	bool targetmode;
	VectorD2<int> target;

};

};

#endif /* CVARPLATFORM_H_ */

