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
#include "fileio/CSavedGame.h"
#include "engine/CEGAGraphics.h"
#include "engine/CMessages.h"
#include "engine/CPassive.h"
#include "engine/playgame/CPlayGame.h"
#include "core/CGameControl.h"
#include "core/CGameMode.h"
#include "SmartPointer.h"

#include <string>

class CGameControl {
public:

	CGameControl(bool &firsttime);
	
	bool init(int argc, char *argv[]);

	void process();
	
	// getters and setters
	bool mustShutdown(){ return (mp_GameMode.get()==NULL); }
	
protected:

	SmartPointer<CGameMode> mp_GameMode;

	bool &m_firsttime;
	int m_startGame_no;
	int m_startLevel;
	bool m_show_finale;

	CMenuController mMenuController;
};

#endif /* CGAMECONTROL_H_ */
