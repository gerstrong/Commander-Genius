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

#include <base/GsEvent.h>
#include <vector>
#include <memory>

class GsEngine
{

public:
    virtual ~GsEngine() {}

    virtual void start() = 0;
    virtual void pumpEvent(const CEvent *evPtr) = 0;
    virtual void ponder(const float deltaT) = 0;
    virtual void render() = 0;
private:


};


struct SwitchEngineEvent : CEvent
{
    SwitchEngineEvent(GsEngine *ptr) :
        mpEnginePtr( std::unique_ptr<GsEngine>(ptr) ) {}

    std::unique_ptr<GsEngine> mpEnginePtr;
};


#endif /* __GSENGINE_H_ */
