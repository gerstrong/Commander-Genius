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



CControlsettings::CControlsettings( const Uint8 dlgTheme,
									const int selectedPlayer ) :
CBaseMenu( dlgTheme, CRect<float>(0.01f, (1.0f-(MAX_COMMANDS+2)*0.06f)*0.5f, 0.98f,(MAX_COMMANDS+2)*0.06f) ),
mSelectedPlayer(selectedPlayer)
{

	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

}

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



//void CControlsettings::setControlsText()
//{
	// Here it sets up the items after the player number was chosen
	// Then create the controls selection screen
	/*std::string buf, buf2;

	for(unsigned int i=0 ; i<MAX_COMMANDS ; i++)
	{
		buf2 = g_pInput->getEventName(IC_LEFT+i, m_chosenPlayer-1);
		buf = actionsnames[i] + buf2;
		mp_Dialog->setObjectText(i, buf);
	}
	
	buf = "Two Button Firing " + getSwitchString(g_pInput->getTwoButtonFiring(m_chosenPlayer-1));
	mp_Dialog->m_dlgobject.at(MAX_COMMANDS)->m_Option->m_FontMapID = 1;
	mp_Dialog->setObjectText(MAX_COMMANDS, buf);*/
//}

//void CControlsettings::processSpecific()
//{
	/*if(!m_suspended)
	{
		if(m_waiting_for_input) // This part only happens, when waiting for an input
			processWaitInput();
		else
			processSelection();
	}
	else
	{
		mp_SubMenu->processCommon();
		mp_SubMenu->processSpecific();
		mp_SubMenu->postProcess();

		if(mp_SubMenu->mustClose())
		{
			SAFE_DELETE(mp_SubMenu);
			m_suspended = false;
			if(m_chosenPlayer == 0)
			{
				m_mustclose = true;
				m_suspended = true;
			}
			else
				setControlsText();
		}
	}*/
//}


