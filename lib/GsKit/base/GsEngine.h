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

    /**
     * @brief start Usually this is started before anything else but still after the construction.
     */
    virtual void start() = 0;

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     * @param evPtr
     */
    virtual void pumpEvent(const CEvent *evPtr) = 0;

    /**
     * @brief ponder    Logic cycle run usually at 120 LPS
     * @param deltaT    how much time of logic to do
     */
    virtual void ponder(const float deltaT) = 0;

    /**
     * @brief render Everything that needs to be rendered representing the current state of the object
     */
    virtual void render() = 0;


};


struct SwitchEngineEvent : CEvent
{
    SwitchEngineEvent(GsEngine *ptr) :
        mpEnginePtr( std::unique_ptr<GsEngine>(ptr) ) {}

    std::unique_ptr<GsEngine> mpEnginePtr;
};


#endif /* __GSENGINE_H_ */
