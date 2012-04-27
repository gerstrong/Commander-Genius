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
					const std::string &commandName,
					std::vector<CGUIButton*> &buttonList ) :
		mSelPlayer(selPlayer),
		mCommand(command),
		mCommandName(commandName),
		mButtonList(buttonList)
		{}

	void operator()()
	{
		while( !g_pInput->readNewEvent(mSelPlayer-1, mCommand) );

		const std::string buf = mCommandName;
		const std::string buf2 = g_pInput->getEventName(mCommand, mSelPlayer-1);
		mButtonList.at(mCommand)->setText(buf + buf2);
	}

	int mSelPlayer;
	InputCommands mCommand;
	const std::string mCommandName;
	std::vector<CGUIButton*> &mButtonList;
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
CBaseMenu( CRect<float>(0.01f, (1.0f-(MAX_COMMANDS+2)*0.06f)*0.5f, 0.98f,(MAX_COMMANDS+2)*0.06f) ),
mSelectedPlayer(selectedPlayer)
{

	mCommandName[IC_LEFT]		= "Left:        ";
	mCommandName[IC_RIGHT]		= "Right:       ";
	mCommandName[IC_UP]			= "Up:          ";
	mCommandName[IC_DOWN]		= "Down:        ";
	mCommandName[IC_UPPERLEFT]	= "Upper left:  ";
	mCommandName[IC_UPPERRIGHT] = "Upper right: ";
	mCommandName[IC_LOWERLEFT] 	= "Lower left:  ";
	mCommandName[IC_LOWERRIGHT]	= "Lower right: ";
	mCommandName[IC_JUMP] 		= "Jump:        ";
	mCommandName[IC_POGO] 		= "Pogo:        ";
	mCommandName[IC_FIRE]		= "Fire:        ";
	mCommandName[IC_STATUS] 	= "Status:      ";
	mCommandName[IC_HELP] 		= "Help:        ";
	mCommandName[IC_BACK] 		= "Back:        ";
}

void CControlsettings::init()
{

	if(!mpButtonList.empty())
		mpButtonList.clear();

	std::map<InputCommands, std::string>::iterator it = mCommandName.begin();
	for ( ; it != mCommandName.end(); it++ )
	{
		const std::string buf = it->second;
		const std::string buf2 = g_pInput->getEventName( it->first, mSelectedPlayer-1 );

		CGUIButton	*guiButton = new CGUIButton( buf+buf2,
	 	 	 	 	 	 	 	 	 	 	 	 new ReadInputEvent(mSelectedPlayer, it->first,
	 	 	 	 	 	 	 	 	 	 	 			 	 	 	 it->second, mpButtonList) );

		mpButtonList.push_back( guiButton );
		mpMenuDialog->addControl( guiButton );

	}

	mpTwoButtonSwitch = new CGUISwitch( "Two Button Firing",
										CGUISwitch::VORTICON );

	mpTwoButtonSwitch->enable(g_pInput->getTwoButtonFiring(mSelectedPlayer-1));

	mpMenuDialog->addControl( mpTwoButtonSwitch );
	mpMenuDialog->addControl( new CGUIButton( "Reset Controls",
	 	 	 	 	 	 	 	 	 	 	  new ResetInputEvent(mSelectedPlayer-1) ) );

	setMenuLabel("MOVEMENULABEL");
}



void CControlsettings::release()
{
	g_pInput->setTwoButtonFiring(mSelectedPlayer-1, mpTwoButtonSwitch->isEnabled() );
	g_pInput->saveControlconfig();
}
