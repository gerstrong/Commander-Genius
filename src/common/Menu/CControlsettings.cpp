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

const std::string actionsnames[MAX_COMMANDS] =
{
		"Left:        ",
		"Right:       ",
		"Up:          ",
		"Down:        ",
		"Upper left:  ",
		"Upper right: ",
		"Lower left:  ",
		"Lower right: ",
		"Jump:        ",
		"Pogo:        ",
		"Fire:        ",
		"Status:      ",
		"Help:        ",
		"Quit:        "
};


/**
 * \brief This sets the default settings for a classic gameplay
 */
class ReadInputEvent : public InvokeFunctorEvent
{
public:

	ReadInputEvent( const int selPlayer,
					const int command,
					std::vector<CGUIButton*> &buttonList ) :
		mSelPlayer(selPlayer),
		mCommand(command),
		mButtonList(buttonList)
		{}

	void operator()()
	{
		while( !g_pInput->readNewEvent(mSelPlayer-1, mCommand) );

		const std::string buf = actionsnames[mCommand];
		const std::string buf2 = g_pInput->getEventName(mCommand, mSelPlayer-1);
		mButtonList.at(mCommand)->setText(buf + buf2);
	}

	int mSelPlayer;
	int mCommand;
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
{}

void CControlsettings::init()
{

	if(!mpButtonList.empty())
		mpButtonList.clear();

	for(unsigned int i=0 ; i<MAX_COMMANDS ; i++)
	{
		const std::string buf = actionsnames[i];
		const std::string buf2 = g_pInput->getEventName( i, mSelectedPlayer-1 );

		CGUIButton	*guiButton = new CGUIButton( buf+buf2,
	 	 	 	 	 	 	 	 	 	 	 	 new ReadInputEvent(mSelectedPlayer, i, mpButtonList),
	 	 	 	 	 	 	 	 	 	 	 	 CGUIButton::VORTICON );

		mpButtonList.push_back( guiButton );
		mpMenuDialog->addControl( guiButton );
	}

	mpTwoButtonSwitch = new CGUISwitch( "Two Button Firing",
										CGUISwitch::VORTICON );

	mpTwoButtonSwitch->enable(g_pInput->getTwoButtonFiring(mSelectedPlayer-1));

	mpMenuDialog->addControl( mpTwoButtonSwitch );
	mpMenuDialog->addControl( new CGUIButton( "Reset Controls",
	 	 	 	 	 	 	 	 	 	 	  new ResetInputEvent(mSelectedPlayer-1),
	 	 	 	 	 	 	 	 	 	 	  CGUIButton::VORTICON ) );

}



void CControlsettings::release()
{
	g_pInput->setTwoButtonFiring(mSelectedPlayer-1, mpTwoButtonSwitch->isEnabled() );
	g_pInput->saveControlconfig();
}
