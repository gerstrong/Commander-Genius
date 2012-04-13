/*
 * CGameMain.h
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "CBaseEngine.h"
#include "engine/CEvent.h"

#ifndef CGAMEMAIN_H_
#define CGAMEMAIN_H_

class CGameMain : public CBaseEngine
{
public:
	CGameMain() {};

	bool isPlayingGame()
	{		return (dynamic_cast<GMSwitchToPlayGameMode*>(mpGameMode.get())!=NULL);	}

protected:

	void init();
	void process();

private:

	SmartPointer<CInfoScene> mpInfoScene;
	SmartPointer<CGameMode>	mpGameMode;

	//bool mShowFinale; // This should be an event
};


// Events
struct StartMainGameEvent : public CEvent {};

#endif /* CGAMEMAIN_H_ */
