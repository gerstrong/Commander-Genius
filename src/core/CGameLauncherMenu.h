/*
 * CGameLauncherMenu.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#ifndef CGAMELAUNCHERMENU_H_
#define CGAMELAUNCHERMENU_H_

#include "CGameMode.h"
#include "common/CGameLauncher.h"
#include "common/Menu/CProfilesMenu.h"
#include "engine/CEGAGraphics.h"
#include "gui/CGUIDialog.h"
#include "SmartPointer.h"

enum load_states{
LOADGFX=0x01,
LOADSTR=0x02,
LOADSND=0x04,
LOADALL=0xFF
};

class CGameLauncherMenu : public CGameMode {
public:
	CGameLauncherMenu(bool& first_time, const int start_game_no = -1, const int start_level = -1);

	bool loadMenuResources();
	bool loadResources( const std::string& DataDirectory, const int& Episode, const Uint8 flags = LOADALL);
	void init();
	void process();

private:
	SmartPointer<CGameLauncher> mp_GameLauncher;
	bool &m_firsttime;
	SmartPointer<CProfilesMenu> mp_FirstTimeMenu;
	int m_start_game_no;
	int m_start_level;

	SmartPointer<CEGAGraphics> mp_EGAGraphics;
};

#endif /* CGAMELAUNCHERMENU_H_ */
