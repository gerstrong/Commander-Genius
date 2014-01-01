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
#include "core/LogicDrawSplitter.h"
#include "engine/CEvent.h"

class CBaseEngine : public GameState
{

public:
	virtual ~CBaseEngine() {}

    virtual void pumpEvent(const CEvent *evPtr) = 0;

	virtual void init() = 0;
};

#endif /* CBASEENGINE_H_ */
