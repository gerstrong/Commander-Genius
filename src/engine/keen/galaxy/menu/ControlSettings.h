/*
 * CControlsettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CCONTROLSETTINGS_H_
#define CCONTROLSETTINGS_H_

#include <widgets/GsBaseMenu.h>

#include <base/utils/StringUtils.h>
#include <widgets/GsButton.h>
#include <widgets/GsSwitch.h>
#include <vector>
#include <map>

#include "engine/CEvent.h"
#include "SelectionMenu.h"

namespace galaxy
{

// Main Class for Controller Config
class CControlsettings : public CBaseMenu
{
public:
	CControlsettings(const int selectedPlayer);

	void init();

	void release();

private:

	int mSelectedPlayer;

	CGUISwitch *mpTwoButtonSwitch;
	CGUISwitch *mpAnalogSwitch;
	CGUISwitch *mpSuperPogoSwitch;
	CGUISwitch *mpImpPogoSwitch;
	CGUISwitch *mpAutoGunSwitch;
};


// Movement config Part
class CControlSettingsMovement : public CBaseMenu
{
public:
	CControlSettingsMovement(const int selectedPlayer);

	void init();
    void ponder();
	void release();

private:

	int mSelectedPlayer;

	std::vector<GsButton*> mpButtonList;
	std::map<InputCommands, std::string> mCommandName;
	
	bool mapping;
};



// Buttons config Part
class CControlSettingsButtons : public CBaseMenu
{
public:
	CControlSettingsButtons(const int selectedPlayer);

	void init();
    void ponder();
	void release();

private:

	int mSelectedPlayer;

	std::vector<GsButton*> mpButtonList;
	std::map<InputCommands, std::string> mCommandName;
	
	bool mapping;
};

// Create the Control-Menu for the indicated Player



struct OpenControlMenuEvent : SelectionEvent
{
	OpenControlMenuEvent(const int players) :
        SelectionEvent(players) {}
};


struct OpenButtonsControlMenuEvent : OpenControlMenuEvent
{
	OpenButtonsControlMenuEvent(const int option) :
        OpenControlMenuEvent(option) {}
};

struct OpenMovementControlMenuEvent : OpenControlMenuEvent
{
	OpenMovementControlMenuEvent(const int option) :
        OpenControlMenuEvent(option) {}

};

}

#endif /* CCONTROLSETTINGS_H_ */
