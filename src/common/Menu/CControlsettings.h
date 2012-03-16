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
#include <vector>


class CControlsettings : public CBaseMenu
{
public:
	CControlsettings(const Uint8 dlgTheme, const int selectedPlayer);

	void init();

	void release();

private:

	int mSelectedPlayer;
	bool mWaitingForInput;

	std::vector<CGUIButton*> mpButtonList;
	CGUISwitch *mpTwoButtonSwitch;
};

// Create the Control-Menu for the indicated Player
struct OpenControlMenuEvent : CEvent
{
	OpenControlMenuEvent(const Uint8 dlgTheme, const int option) :
		mDlgTheme(dlgTheme), mNumPlayers(option) {};

	Uint8 mDlgTheme;
	int mNumPlayers;
	std::list<CGUIButton*> mpButtonList;
};

class CPlayerControl : public CBaseMenu
{

public:
	CPlayerControl(const Uint8 dlgTheme) :
		CBaseMenu( dlgTheme, CRect<float>(0.25f, 0.35f, 0.5f, 0.3f) )
	{
		mpMenuDialog->setBackground( CGUIDialog::VORTICON );

		for( int i = 1 ; i <= MAX_PLAYERS ; i++ )
		{
			const std::string	playerStr = "Player " + itoa(i);
			mpButtonList.push_back( new CGUIButton( playerStr,
													new OpenControlMenuEvent(dlgTheme, i),
													CGUIButton::VORTICON ) );
			mpMenuDialog->addControl( mpButtonList.back() );
		}

	}

	std::list<CGUIButton*> mpButtonList;
};



#endif /* CCONTROLSETTINGS_H_ */
