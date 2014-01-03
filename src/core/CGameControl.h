/*
 * GsAppState.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 *
 *  Auxiliary Class for CGame. It only tells the Game-Engine what to do.
 */

#ifndef GsAppState_H_
#define GsAppState_H_

#include <lib/base/GsAppState.h>
#include <string>
#include <memory>

class CGState : public GsAppState
{
public:

    CGState(bool &firsttime);
	
	bool mustShutdown(){ return (mpEngine.get()==nullptr); }
	
protected:

    std::unique_ptr<GsEngine> mpEngine;

	bool &m_firsttime;
	int m_startGame_no;
	int m_startLevel;
};

#endif /* GsAppState_H_ */
