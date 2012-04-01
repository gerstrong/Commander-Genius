/*
 * CBaseEngine.h
 *
 *  Created on: 28.03.2012
 *      Author: gerstrong
 *
 *  Base for the engine. Some commons functions are performed here!
 */

#ifndef CBASEENGINE_H_
#define CBASEENGINE_H_

#include "mode/CGameMode.h"

class CBaseEngine
{

public:

	virtual void init() = 0;
	virtual void process() = 0;
};

#endif /* CBASEENGINE_H_ */
