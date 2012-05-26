/*
 * CControlsettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CCONTROLSETTINGS_H_
#define CCONTROLSETTINGS_H_

#include "CBaseMenu.h"
#include "StringUtils.h"
#include "gui/CGUIButton.h"
#include "gui/CGUISwitch.h"
#include "engine/CEvent.h"
#include "CSelectionMenu.h"
#include <vector>
#include <map>


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
	void release();

private:

	int mSelectedPlayer;

	std::vector<CGUIButton*> mpButtonList;
	std::map<InputCommands, std::string> mCommandName;
};



// Buttons config Part
class CControlSettingsButtons : public CBaseMenu
{
public:
	CControlSettingsButtons(const int selectedPlayer);

	void init();
	void release();

private:

	int mSelectedPlayer;

	std::vector<CGUIButton*> mpButtonList;
	std::map<InputCommands, std::string> mCommandName;
};

// Create the Control-Menu for the indicated Player



struct OpenControlMenuEvent : SelectionEvent
{
	OpenControlMenuEvent(const int players) :
		SelectionEvent(players) {};
};


struct OpenButtonsControlMenuEvent : OpenControlMenuEvent
{
	OpenButtonsControlMenuEvent(const int option) :
		OpenControlMenuEvent(option) {};
};

struct OpenMovementControlMenuEvent : OpenControlMenuEvent
{
	OpenMovementControlMenuEvent(const int option) :
		OpenControlMenuEvent(option) {};

};


#endif /* CCONTROLSETTINGS_H_ */
