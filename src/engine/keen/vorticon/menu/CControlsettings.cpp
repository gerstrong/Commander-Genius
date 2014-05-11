/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include <base/utils/StringUtils.h>
#include "CSelectionMenu.h"
#include "CControlsettings.h"
#include "engine/core/CBehaviorEngine.h"

#include <base/CInput.h>

namespace vorticon
{

/**
 * \brief This sets the default settings for a classic gameplay
 */
class ReadInputEvent : public InvokeFunctorEvent
{
public:

	ReadInputEvent( const int selPlayer,
			const InputCommands command,
			const std::string &commandName ) :
		mSelPlayer(selPlayer),
		mCommand(command),
		mCommandName(commandName),
		mpButton(NULL)
		{}

    void setButtonPtr(Button* button)
	{
		mpButton = button;
	}

    void operator()() const
	{
		gInput.setupNewEvent(mSelPlayer-1, mCommand);

		const std::string buf = mCommandName;		
		mpButton->setText(buf + "=Reading=");
	}

	int mSelPlayer;
	InputCommands mCommand;
	const std::string mCommandName;
    Button* mpButton;
};


/**
 * \brief This sets the default settings for a classic gameplay
 */
class ResetInputEvent : public InvokeFunctorEvent
{
public:

	ResetInputEvent( const int selPlayer ) :
		mSelPlayer(selPlayer)
		{}

    void operator()() const
	{
		gInput.resetControls(mSelPlayer);
	}

	int mSelPlayer;
};



CControlsettings::CControlsettings( const int selectedPlayer ) :
VorticonMenu( GsRect<float>(0.1f, 0.25f, 0.8f, 0.5f) ),
mSelectedPlayer(selectedPlayer)
{
    GsButton *button;

    button = new Button( "Movement", new OpenMovementControlMenuEvent(mSelectedPlayer) );
    mpMenuDialog->addControl( button );

    button = new Button( "Buttons", new OpenButtonsControlMenuEvent(mSelectedPlayer) );
    mpMenuDialog->addControl( button );

    mpTwoButtonSwitch = new Switch( "Two Button Fire" );
    mpTwoButtonSwitch->enable(gInput.getTwoButtonFiring(mSelectedPlayer-1));

    mpAnalogSwitch = new Switch( "Analog Movement" );
    mpAnalogSwitch->enable(gInput.isAnalog(mSelectedPlayer-1));

    mpSuperPogoSwitch = new Switch( "Super Pogo" );
    mpSuperPogoSwitch->enable(gInput.SuperPogo(mSelectedPlayer-1));

    mpImpPogoSwitch = new Switch( "Impossible Pogo" );
    mpImpPogoSwitch->enable(gInput.ImpossiblePogo(mSelectedPlayer-1));

    mpAutoGunSwitch = new Switch( "Auto Gun" );
    mpAutoGunSwitch->enable(gInput.AutoGun(mSelectedPlayer-1));

    mpMenuDialog->addControl( mpTwoButtonSwitch );
    mpMenuDialog->addControl( mpAnalogSwitch );
    mpMenuDialog->addControl( mpSuperPogoSwitch );
    mpMenuDialog->addControl( mpImpPogoSwitch );
    mpMenuDialog->addControl( mpAutoGunSwitch );
    mpMenuDialog->addControl( new Button( "Reset Controls", new ResetInputEvent(mSelectedPlayer-1) ) );
}


void CControlsettings::release()
{
	gInput.setTwoButtonFiring(mSelectedPlayer-1, mpTwoButtonSwitch->isEnabled() );
	gInput.enableAnalog(mSelectedPlayer-1, mpAnalogSwitch->isEnabled() );
	gInput.setSuperPogo(mSelectedPlayer-1, mpSuperPogoSwitch->isEnabled() );
	gInput.setImpossiblePogo(mSelectedPlayer-1, mpImpPogoSwitch->isEnabled() );
	gInput.setAutoGun(mSelectedPlayer-1, mpAutoGunSwitch->isEnabled() );
	gInput.saveControlconfig();
}


CControlSettingsBase::CControlSettingsBase(const int selectedPlayer) :
VorticonMenu( GsRect<float>(0.01f, (1.0f-((MAX_COMMANDS/2.0f)+2)*0.06f)*0.5f, 0.98f,(MAX_COMMANDS/2.0f+2)*0.06f) ),
mSelectedPlayer(selectedPlayer),
mapping(false)
{}


void CControlSettingsBase::ponder(const float deltaT)
{
    if( !mapping && gInput.MappingInput() )  // mapping changed!
    {
        mapping = true;
    }
    else if( !gInput.MappingInput() )
    {
        if(mapping) // mapping changed!
        {
            mapping = false;

            int pos; unsigned char input;
            std::string evName = gInput.getNewMappedEvent(pos, input);
            InputCommands com = static_cast<InputCommands>(pos);

            if(pos >= MID_COMMANDS_OFFSETS)
                pos -= MID_COMMANDS_OFFSETS;

            mpButtonList[pos]->setText(mCommandName[com] + evName);
        }

        CBaseMenu::ponder(0);
    }
}

void CControlSettingsBase::release()
{
    if(!mCommandName.empty())
        mCommandName.clear();

    gInput.saveControlconfig();
}



// Movements Parts of the Control Settings
void CControlSettingsMovement::init()
{
    mapping = false;
	mCommandName[IC_LEFT]		= "Left:   ";
	mCommandName[IC_RIGHT]		= "Right:  ";
	mCommandName[IC_UP]		= "Up:     ";
	mCommandName[IC_DOWN]		= "Down:   ";
	mCommandName[IC_UPPERLEFT]	= "U-left: ";
	mCommandName[IC_UPPERRIGHT] 	= "U-right:";
	mCommandName[IC_LOWERLEFT] 	= "D-left: ";
	mCommandName[IC_LOWERRIGHT]	= "D-right:";

	if(!mpButtonList.empty())
		mpButtonList.clear();

	std::map<InputCommands, std::string>::iterator it = mCommandName.begin();
	for ( ; it != mCommandName.end(); it++ )
	{
		const std::string buf = it->second;
		const std::string buf2 = gInput.getEventShortName( it->first, mSelectedPlayer-1 );

		ReadInputEvent *rie = new ReadInputEvent(mSelectedPlayer, it->first, it->second);
        Button	*guiButton = new Button( buf+buf2, rie );
		rie->setButtonPtr(guiButton);

		mpButtonList.push_back( guiButton );
		mpMenuDialog->addControl( guiButton );
	}
}


// Movements Parts of the Control Settings
void CControlSettingsButtons::init()
{
    mapping = false;
	mCommandName[IC_JUMP] 		= "Jump:   ";
	mCommandName[IC_POGO] 		= "Pogo:   ";
	mCommandName[IC_FIRE]		= "Fire:   ";
	mCommandName[IC_STATUS] 	= "Status: ";
	mCommandName[IC_CAMLEAD] 	= "Camlead:";
	mCommandName[IC_HELP] 		= "Help:   ";
	mCommandName[IC_BACK] 		= "Back:   ";

	if(!mpButtonList.empty())
		mpButtonList.clear();

	std::map<InputCommands, std::string>::iterator it = mCommandName.begin();
	for ( ; it != mCommandName.end(); it++ )
	{
		const std::string buf = it->second;
		const std::string buf2 = gInput.getEventShortName( it->first, mSelectedPlayer-1 );

		ReadInputEvent *rie = new ReadInputEvent(mSelectedPlayer, it->first, it->second);
        Button	*guiButton = new Button( buf+buf2, rie );
		rie->setButtonPtr(guiButton);


		mpButtonList.push_back( guiButton );
		mpMenuDialog->addControl( guiButton );

	}
}


}

