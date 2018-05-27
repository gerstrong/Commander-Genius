/*
 * GameEngine.h
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "mode/CGameMode.h"
#include "CResourceLoader.h"

#include <base/GsEngine.h>
#include <graphics/GsSurface.h>
#include <memory>

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

class GameEngine : public GsEngine
{
public:
    GameEngine(const bool openedGamePlay,
               const std::string &datapath) :
        mOpenedGamePlay(openedGamePlay),
        mDataPath(datapath)
    {}

    virtual bool start() override = 0;

    void ponder(const float deltaT) override;

    void render() override;

    void renderFPSDisplay();

    virtual void pumpEvent(const CEvent *evPtr) override;


protected:

    CResourceLoaderBackground mEngineLoader;	

    std::unique_ptr<CGameMode> mpGameMode;

    bool mOpenedGamePlay;

    const std::string mDataPath;

private:

    GsSurface mFPSSurface;

};


#endif /* GAMEENGINE_H */
