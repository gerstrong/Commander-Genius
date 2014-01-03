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

class CBaseEngine
{

public:
	virtual ~CBaseEngine() {}

    virtual void ponder(const float deltaT) = 0;
    virtual void render() = 0;

    virtual void pumpEvent(const CEvent *evPtr) = 0;

    //virtual void init() = 0;
    virtual void start() = 0;
};

#endif /* CBASEENGINE_H_ */
