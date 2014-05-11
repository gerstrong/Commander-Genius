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

#include <base/GsEvent.h>
#include "CSelectionMenu.h"

namespace vorticon
{

// Main Class for Controller Config
class CControlsettings : public VorticonMenu
{
public:
	CControlsettings(const int selectedPlayer);

	void release();

private:

	int mSelectedPlayer;

    Switch *mpTwoButtonSwitch;
    Switch *mpAnalogSwitch;
    Switch *mpSuperPogoSwitch;
    Switch *mpImpPogoSwitch;
    Switch *mpAutoGunSwitch;
};


class CControlSettingsBase : public VorticonMenu
{
public:
    CControlSettingsBase(const int selectedPlayer);

    virtual void ponder(const float deltaT);
    void release();

protected:
    int mSelectedPlayer;

    std::vector<Button*> mpButtonList;
    std::map<InputCommands, std::string> mCommandName;

    bool mapping;
};




// Movement config Part
class CControlSettingsMovement : public CControlSettingsBase
{
public:
    CControlSettingsMovement(const int selectedPlayer) :
        CControlSettingsBase(selectedPlayer)
    {
        init();
    }

    void init();
};



// Buttons config Part
class CControlSettingsButtons : public CControlSettingsBase
{
public:
    CControlSettingsButtons(const int selectedPlayer) :
        CControlSettingsBase(selectedPlayer)
    {
        init();
    }

	void init();
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
