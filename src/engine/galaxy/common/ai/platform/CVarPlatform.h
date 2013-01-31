/*
 * CVarPlatform.h
 *
 *  Created on: Sep 6, 2012
 *      Author: gerstrong
 */

#ifndef CVARPLATFORM_H_
#define CVARPLATFORM_H_

#include "CPlatform.h"
#include <engine/galaxy/common/ai/CMoveTarget.h>
#include "CVec.h"

namespace galaxy {


class CVarPlatform : public CPlatform, public CMoveTarget
{
public:
	CVarPlatform(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y,
			const direction_t horidir, 
			const direction_t vertdir, 
			const int actionOffset);

	void process();
		
	void deserialize(CSaveGameController &savedGame) 
	{
	    savedGame.decodeData(target.x);
	    savedGame.decodeData(target.y);
	}

	void serialize(CSaveGameController &savedGame) 
	{
    	    savedGame.encodeData(target.x);
	    savedGame.encodeData(target.y);
	}
	
};

};

#endif /* CVARPLATFORM_H_ */

