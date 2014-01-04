#ifndef __VORTICONENGINE_H__
#define __VORTICONENGINE_H__

#include "engine/keen/KeenEngine.h"

#include "engine/vorticon/CEGAGraphicsVort.h"

namespace vorticon
{


class VorticonEngine : public KeenEngine
{
public:
    const int mEp;

    VorticonEngine(const bool openedGamePlay,
                   const int ep,
                   const std::string &dataPath) :
        KeenEngine(openedGamePlay, ep, dataPath),
        mEp(ep) {}

    bool loadResources( const Uint8 flags );

private:

    //std::unique_ptr<CEGAGraphicsVort> mpEGAGraphics;
};

}

// Events
struct StartVorticonEngine : public SwitchEngineEvent
{
    StartVorticonEngine(const bool openedGamePlay,
                        const int ep,
                        const std::string &dataPath) :
        SwitchEngineEvent( new vorticon::VorticonEngine(openedGamePlay, ep, dataPath) )
        { }
};


#endif /* __VORTICONENGINE_H__ */
