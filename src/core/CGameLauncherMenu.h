/*
 * CGameLauncherMenu.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#ifndef CGAMELAUNCHERMENU_H_
#define CGAMELAUNCHERMENU_H_

#include "mode/CGameMode.h"
#include "common/CGameLauncher.h"
#include "core/CBaseEngine.h"
#include "common/Menu/CProfilesMenu.h"
#include "engine/CEGAGraphics.h"
#include "SmartPointer.h"

enum load_states{
LOADGFX=0x01,
LOADSTR=0x02,
LOADSND=0x04,
LOADALL=0xFF
};

class CGameLauncherMenu : public CBaseEngine
{
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

// Events
// This event switches to the GameLauncher
struct GMSwitchToGameLauncher : CEvent
{
	const int m_ChosenGame;
	const int m_StartLevel;
	GMSwitchToGameLauncher(	const int ChosenGame=-1, const int StartLevel=-1 ) :
		m_ChosenGame(ChosenGame),
		m_StartLevel(StartLevel){}
};


#endif /* CGAMELAUNCHERMENU_H_ */
