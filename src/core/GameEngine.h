/*
 * GameEngine.h
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "engine/CEvent.h"
#include "engine/infoscenes/CInfoScene.h"
#include "core/mode/CGameMode.h"
#include <base/GsEngine.h>
#include <memory>

#ifndef __GAMEENGINE_H_
#define __GAMEENGINE_H_

class GameEngine : public GsEngine
{
public:
    GameEngine(const bool openedGamePlay) :
        mOpenedGamePlay(openedGamePlay) {}

protected:

	void switchToGamePlayMode();

    void start() {}
    void pumpEvent(const CEvent *evPtr);
    void ponder(const float deltaT);
    void render();

private:

	std::shared_ptr<CInfoScene> mpInfoScene;
	std::unique_ptr<CGameMode> mpGameMode;

    bool mOpenedGamePlay;
};


// Events
struct StartMainGameEvent : public SwitchEngineEvent
{
    StartMainGameEvent(const bool openedGamePlay) :
        SwitchEngineEvent( new GameEngine(openedGamePlay) )
        { }
};

#endif /* __GAMEENGINE_H_ */
