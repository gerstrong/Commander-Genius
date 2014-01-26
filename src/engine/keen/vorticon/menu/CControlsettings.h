/*
 * CControlsettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CCONTROLSETTINGS_H_
#define CCONTROLSETTINGS_H_

#include "VorticonMenu.h"

#include <base/utils/StringUtils.h>
#include "widgets/Button.h"
#include "widgets/Switch.h"
#include <vector>
#include <map>

#include "engine/CEvent.h"
#include "CSelectionMenu.h"

namespace vorticon
{

// Main Class for Controller Config
class CControlsettings : public VorticonMenu
{
public:
	CControlsettings(const int selectedPlayer);

	void init();

	void release();

private:

	int mSelectedPlayer;

    Switch *mpTwoButtonSwitch;
    Switch *mpAnalogSwitch;
    Switch *mpSuperPogoSwitch;
    Switch *mpImpPogoSwitch;
    Switch *mpAutoGunSwitch;
};


// Movement config Part
class CControlSettingsMovement : public VorticonMenu
{
public:
	CControlSettingsMovement(const int selectedPlayer);

	void init();
    void ponder();
	void release();

private:

	int mSelectedPlayer;

    std::vector<Button*> mpButtonList;
	std::map<InputCommands, std::string> mCommandName;
	
	bool mapping;
};



// Buttons config Part
class CControlSettingsButtons : public VorticonMenu
{
public:
	CControlSettingsButtons(const int selectedPlayer);

	void init();
    void ponder();
	void release();

private:

	int mSelectedPlayer;

    std::vector<Button*> mpButtonList;
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
