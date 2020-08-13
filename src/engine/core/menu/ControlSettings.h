/*
 * CControlsettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CCONTROLSETTINGS_H_
#define CCONTROLSETTINGS_H_

#include <widgets/GsBaseMenu.h>

#include "engine/core/menu/GameMenu.h"

#include "SelectionMenu.h"
#include <base/utils/StringUtils.h>
#include <base/GsEvent.h>
#include "widgets/Button.h"
#include "widgets/Switch.h"
#include <vector>
#include <map>


#include "SelectionMenu.h"


// Main Class for Controller Config
class CControlsettings : public GameMenu
{
public:
    CControlsettings(const int selectedPlayer,
                     const Style style);

    void refresh();

	void release();

private:

    int mSelectedPlayer = 0;

    std::shared_ptr<Switch> mpTwoButtonSwitch;
    std::shared_ptr<Switch> mpAnalogSwitch;
    std::shared_ptr<Switch> mpSuperPogoSwitch;
    std::shared_ptr<Switch> mpImpPogoSwitch;
    std::shared_ptr<Switch> mpAutoGunSwitch;
};


class CControlSettingsBase : public GameMenu
{
public:
    CControlSettingsBase(const int selectedPlayer,
                         const Style style);

    virtual void ponder(const float deltaT);
    virtual void release();

protected:

    void addBottomText();


    int mSelectedPlayer;

    std::vector< std::shared_ptr<GameButton> > mpButtonList;
    std::map<InpCmd, std::string> mCommandName;

    bool mapping;
};


// Movement config Part
class CControlSettingsMovement : public CControlSettingsBase
{
public:
    CControlSettingsMovement(const int selectedPlayer,
                             const Style style) :
        CControlSettingsBase(selectedPlayer, style) {}

    void refresh() override;
};


// Movement config Part
class CControlSettingsMoveDiag : public CControlSettingsBase
{
public:
    CControlSettingsMoveDiag(const int selectedPlayer,
                             const Style style) :
        CControlSettingsBase(selectedPlayer, style) {}

    void refresh() override;
};



// Game play action config Part
class CControlSettingsGameplayActions : public CControlSettingsBase
{
public:
    CControlSettingsGameplayActions(const int selectedPlayer,
                            const Style &style) :
        CControlSettingsBase(selectedPlayer, style) {}

    void refresh() override;
};

// Misc config Part
class CControlSettingsMisc : public CControlSettingsBase
{
public:
    CControlSettingsMisc(const int selectedPlayer,
                            const Style &style) :
        CControlSettingsBase(selectedPlayer, style) {}

    void refresh() override;
};


// Create the Control-Menu for the indicated Player



struct OpenControlMenuEvent : SelectionEvent
{
	OpenControlMenuEvent(const int players) :
        SelectionEvent(players) {}
};


#endif /* CCONTROLSETTINGS_H_ */
