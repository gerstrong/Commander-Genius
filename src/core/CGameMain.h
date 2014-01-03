/*
 * CGameMain.h
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

//#include "GsEngine.h"
#include "engine/CEvent.h"
#include "engine/infoscenes/CInfoScene.h"
#include "core/mode/CGameMode.h"
#include <base/GsEngine.h>
#include <memory>

#ifndef CGAMEMAIN_H_
#define CGAMEMAIN_H_

class CGameMain : public GsEngine
{
public:
    CGameMain(bool &openedGamePlay) : mOpenedGamePlay(openedGamePlay) {}

protected:

	void switchToGamePlayMode();

    void start() {}
    void pumpEvent(const CEvent *evPtr);
    void ponder(const float deltaT);
    void render();

private:

	std::shared_ptr<CInfoScene> mpInfoScene;
	std::unique_ptr<CGameMode> mpGameMode;

	bool &mOpenedGamePlay;
};


// Events
struct StartMainGameEvent : public CEvent {};

#endif /* CGAMEMAIN_H_ */
