/**
 * GsApp.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef GSAPP_H_
#define GSAPP_H_

#include <base/GsEvent.h>
#include <base/GsEngine.h>
#include <base/Singleton.h>

#include <memory>


#define gApp GsApp::get()

// Forward declaration
class GsApp;

// App State has an event sink that is registered thought the constructor and teared down through the deconstrcutor
class GsAppEventSink : public GsEventSink
{

public:
    GsAppEventSink(GsApp* pApp) :
        mpApp(pApp) {}

    void pumpEvent(const std::shared_ptr<CEvent> &ev) override;

private:
    GsApp* mpApp;

};

class GsApp : public GsSingleton<GsApp>
{
public:
    GsApp();
    ~GsApp();

    bool init(int argc, char *argv[]);
    void deinit();

    bool loadDrivers();
    void unloadDrivers();

    void setEngine(GsEngine *engPtr);

    void runMainCycleNonThreaded();

    void runMainCycle();

    void runMainCycleEmscripten();
    void cleanup();

    void pollEvents();

    void ponder(const float deltaT);

    void render();

    bool mustShutdown(){ return (mpCurEngine==nullptr); }

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr);

    void setName(const std::string &appName);
    std::string getName();

    /**
     * @brief ignoreLogicCounter    Some engine parts coming from legacy code have the issues,
     *                              that they like to hang for certain section (v.g. text printing)
     *                              and require a quick rendering. If that happens you should call this function
     *                              so the App engine does not think there was a intensive computational operation going on.
     *                              The ignore Logic Counter breaks only once per call.
     *                              NOTE: Does not work when vsync is enabled
     */
    void ignoreLogicCounter();

private:

    std::unique_ptr<GsEngine> mpCurEngine;
    std::unique_ptr<GsAppEventSink> mpSink;

    std::string mAppName;

    bool mIgnoreLogicCounterOnce = false;
};

#include <base/GsEventContainer.h>

// It's a simple quit event which will force CG to close the App
struct GMQuit : CEvent {};

#endif /* GSAPP_H */
