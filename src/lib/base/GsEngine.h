/*
 * CBaseEngine.h
 *
 *  Created on: 28.03.2012
 *      Author: gerstrong
 *
 *  Base for the engine. Some commons functions are performed here!
 */

#ifndef __GSENGINE_H_
#define __GSENGINE_H_

class GsEngine
{

public:
    virtual ~GsEngine() {}

    virtual void ponder(const float deltaT) = 0;
    virtual void render() = 0;

    virtual void pumpEvent(const CEvent *evPtr) = 0;

    //virtual void init() = 0;
    virtual void start() = 0;
};

#endif /* __GSENGINE_H_ */
