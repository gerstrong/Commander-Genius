/*
 * CGameControl.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 *
 *  Auxiliary Class for CGame. It only tells the Game-Engine what to do.
 */

#ifndef CGAMECONTROL_H_
#define CGAMECONTROL_H_

#include "common/CGameLauncher.h"
#include "common/options.h"
#include "common/Menu/CMenuController.h"
#include "core/CBaseEngine.h"
#include "fileio/CSaveGameController.h"
#include "engine/CEGAGraphics.h"
#include "engine/CMessages.h"
#include "engine/CPassive.h"
#include "engine/playgame/CPlayGame.h"
#include "core/CGameControl.h"
#include "core/mode/CGameMode.h"
#include "SmartPointer.h"

#include <string>

class CGameControl
{
public:

	CGameControl(bool &firsttime);
	
	bool init(int argc, char *argv[]);

	void process();
	
	// getters and setters
	bool mustShutdown(){ return (mpEngine.get()==NULL); }
	
protected:

	SmartPointer<CBaseEngine> mpEngine;

	bool &m_firsttime;
	int m_startGame_no;
	int m_startLevel;

	CMenuController mMenuController;
};

#endif /* CGAMECONTROL_H_ */
