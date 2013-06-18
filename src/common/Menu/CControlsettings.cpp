/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "StringUtils.h"
#include "CSelectionMenu.h"
#include "CControlsettings.h"
#include "common/CBehaviorEngine.h"

#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"


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

	void setButtonPtr(CGUIButton* button)
	{
		mpButton = button;
	}

	void operator()()
	{
		g_pInput->setupNewEvent(mSelPlayer-1, mCommand);

		const std::string buf = mCommandName;		
		mpButton->setText(buf + "=Reading=");
	}

	int mSelPlayer;
	InputCommands mCommand;
	const std::string mCommandName;
	CGUIButton* mpButton;
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

	void operator()()
	{
		g_pInput->resetControls(mSelPlayer);
	}

	int mSelPlayer;
};



CControlsettings::CControlsettings( const int selectedPlayer ) :
CBaseMenu( CRect<float>(0.1f, 0.25f, 0.8f, 0.5f) ),
mSelectedPlayer(selectedPlayer)
{
	CGUIButton *button;

	button = new CGUIButton( "Movement", new OpenMovementControlMenuEvent(mSelectedPlayer) );
	mpMenuDialog->addControl( button );

	button = new CGUIButton( "Buttons", new OpenButtonsControlMenuEvent(mSelectedPlayer) );
	mpMenuDialog->addControl( button );

	mpTwoButtonSwitch = new CGUISwitch( "Two Button Fire" );
	mpTwoButtonSwitch->enable(g_pInput->getTwoButtonFiring(mSelectedPlayer-1));

	mpAnalogSwitch = new CGUISwitch( "Analog Movement" );
	mpAnalogSwitch->enable(g_pInput->isAnalog(mSelectedPlayer-1));

	mpSuperPogoSwitch = new CGUISwitch( "Super Pogo" );
	mpSuperPogoSwitch->enable(g_pInput->SuperPogo(mSelectedPlayer-1));

	mpImpPogoSwitch = new CGUISwitch( "Impossible Pogo" );
	mpImpPogoSwitch->enable(g_pInput->ImpossiblePogo(mSelectedPlayer-1));

	mpAutoGunSwitch = new CGUISwitch( "Auto Gun" );
	mpAutoGunSwitch->enable(g_pInput->AutoGun(mSelectedPlayer-1));

	mpMenuDialog->addControl( mpTwoButtonSwitch );
	mpMenuDialog->addControl( mpAnalogSwitch );
	mpMenuDialog->addControl( mpSuperPogoSwitch );
	mpMenuDialog->addControl( mpImpPogoSwitch );
	mpMenuDialog->addControl( mpAutoGunSwitch );
	mpMenuDialog->addControl( new CGUIButton( "Reset Controls", new ResetInputEvent(mSelectedPlayer-1) ) );
    
    setMenuLabel("KEYBMENULABEL");

}

void CControlsettings::init()
{}



void CControlsettings::release()
{
	g_pInput->setTwoButtonFiring(mSelectedPlayer-1, mpTwoButtonSwitch->isEnabled() );
	g_pInput->enableAnalog(mSelectedPlayer-1, mpAnalogSwitch->isEnabled() );
	g_pInput->setSuperPogo(mSelectedPlayer-1, mpSuperPogoSwitch->isEnabled() );
	g_pInput->setImpossiblePogo(mSelectedPlayer-1, mpImpPogoSwitch->isEnabled() );
	g_pInput->setAutoGun(mSelectedPlayer-1, mpAutoGunSwitch->isEnabled() );
	g_pInput->saveControlconfig();
}



// Movements Parts of the Control Settings
CControlSettingsMovement::CControlSettingsMovement(const int selectedPlayer) :
CBaseMenu( CRect<float>(0.01f, (1.0f-((MAX_COMMANDS/2.0f)+2)*0.06f)*0.5f, 0.98f,(MAX_COMMANDS/2.0f+2)*0.06f) ),
mSelectedPlayer(selectedPlayer)
{}


void CControlSettingsMovement::init()
{
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
		const std::string buf2 = g_pInput->getEventShortName( it->first, mSelectedPlayer-1 );

		ReadInputEvent *rie = new ReadInputEvent(mSelectedPlayer, it->first, it->second);
		CGUIButton	*guiButton = new CGUIButton( buf+buf2, rie );
		rie->setButtonPtr(guiButton);

		mpButtonList.push_back( guiButton );
		mpMenuDialog->addControl( guiButton );
	}

	setMenuLabel("MOVEMENULABEL");
}

void CControlSettingsMovement::process()
{
    if( !mapping )
    {
	if(g_pInput->MappingInput()) // mapping changed!
	    mapping = true;
    }
    else
    {
	if( !g_pInput->MappingInput() )
	{
	    // mapping changed!
	    mapping = false;
	 
	    CGUIButton *button = dynamic_cast<CGUIButton*>(mpMenuDialog->CurrentControl());
	    if(button)
	    {
		int pos; unsigned char input;
		std::string evName = g_pInput->getNewMappedEvent(pos, input);		
		InputCommands com = static_cast<InputCommands>(pos);		
		button->setText(mCommandName[com] + evName);
	    }
	}
    }	
    CBaseMenu::ponder();
}

void CControlSettingsMovement::release()
{
	if(!mCommandName.empty())
		mCommandName.clear();
	
	g_pInput->saveControlconfig();
}



// Movements Parts of the Control Settings
CControlSettingsButtons::CControlSettingsButtons(const int selectedPlayer) :
CBaseMenu( CRect<float>(0.01f, (1.0f-(MAX_COMMANDS/2.0f+2)*0.06f)*0.5f, 0.98f,(MAX_COMMANDS/2.0f+2)*0.06f) ),
mSelectedPlayer(selectedPlayer),
mapping(false)
{}


void CControlSettingsButtons::init()
{
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
		const std::string buf2 = g_pInput->getEventShortName( it->first, mSelectedPlayer-1 );

		ReadInputEvent *rie = new ReadInputEvent(mSelectedPlayer, it->first, it->second);
		CGUIButton	*guiButton = new CGUIButton( buf+buf2, rie );
		rie->setButtonPtr(guiButton);


		mpButtonList.push_back( guiButton );
		mpMenuDialog->addControl( guiButton );

	}

	setMenuLabel("BUTTONMENULABEL");
}

void CControlSettingsButtons::process()
{
    if( !mapping )
    {
	if(g_pInput->MappingInput()) // mapping changed!
	    mapping = true;	
    }
    else
    {
	if( !g_pInput->MappingInput() )
	{
	    // mapping changed!
	    mapping = false;
	 
	    CGUIButton *button = dynamic_cast<CGUIButton*>(mpMenuDialog->CurrentControl());
	    if(button)
	    {
		int pos; unsigned char input;
		std::string evName = g_pInput->getNewMappedEvent(pos, input);		
		InputCommands com = static_cast<InputCommands>(pos);		
		button->setText(mCommandName[com] + evName);
	    }
	}
    }
    
    CBaseMenu::ponder();
}

void CControlSettingsButtons::release()
{
	if(!mCommandName.empty())
		mCommandName.clear();
	
	g_pInput->saveControlconfig();
}

