/*
 * CGameMain.h
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "CBaseEngine.h"
#include "engine/CEvent.h"
#include <memory>

#ifndef CGAMEMAIN_H_
#define CGAMEMAIN_H_

class CGameMain : public CBaseEngine
{
public:
	CGameMain(bool &openedGamePlay) : mOpenedGamePlay(openedGamePlay) {};

protected:

	void switchToGamePlayMode();

	void init();
	void process();

private:

	std::unique_ptr<CInfoScene> mpInfoScene;
	std::unique_ptr<CGameMode> mpGameMode;

	//bool mShowFinale; // This should be an event
	bool &mOpenedGamePlay;
};


// Events
struct StartMainGameEvent : public CEvent {};

#endif /* CGAMEMAIN_H_ */
