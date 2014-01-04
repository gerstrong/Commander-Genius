#ifndef __GALAXYENGINE_H__
#define __GALAXYENGINE_H__

#include "engine/keen/KeenEngine.h"

#include "engine/galaxy/res/CEGAGraphicsGalaxy.h"

namespace galaxy
{


class GalaxyEngine : public KeenEngine
{
public:

    const int mEp;

    GalaxyEngine(const bool openedGamePlay,
                 const int ep,
                 const std::string &dataPath) :
        KeenEngine(openedGamePlay, ep, dataPath),
        mEp(ep) {}

    bool loadResources( const Uint8 flags );

private:

    //std::unique_ptr<CEGAGraphicsGalaxy> mpEGAGraphics;
};

}

// Events
struct StartGalaxyEngine : public SwitchEngineEvent
{
    StartGalaxyEngine(const bool openedGamePlay,
                      const int ep,
                      const std::string &dataPath) :
        SwitchEngineEvent( new galaxy::GalaxyEngine(openedGamePlay, ep, dataPath) )
        { }
};


#endif /* __GALAXYENGINE_H__ */
