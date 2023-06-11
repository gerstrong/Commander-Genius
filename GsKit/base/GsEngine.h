/*
 * CBaseEngine.h
 *
 *  Created on: 28.03.2012
 *      Author: gerstrong
 *
 *  Base for the engine.
 *  It handles the rendering and logic tasks without
 *  using additional threads.
 *
 */

#ifndef GSENGINE_H
#define GSENGINE_H

#include <base/GsEvent.h>
#include <vector>
#include <memory>
#include <string_view>

class GsEngine
{

public:

    /**
     * @brief setupNativeRes
     * @param strView View to the string describing the section
     *        of currently engine (aka name of the engine)
     * @return true is all went fine, otherwise false
     */
    bool setupNativeRes(const std::string_view &strView);

    /**
     * @brief ~GsEngine virtual destructor for GsEngine
     */
    virtual ~GsEngine();

    /**
     * @brief start Usually this is started before anything else but still after the construction.
     */
    virtual bool start() = 0;

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     * @param evPtr
     */
    virtual void pumpEvent(const std::shared_ptr<CEvent> &evPtr) = 0;

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


#endif /* GSENGINE_H */
