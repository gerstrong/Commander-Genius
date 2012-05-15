/*
 * CSelectionMenu.h
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 *
 *  Based on the work of Pizza2004
 *  This is template for normal selection menus. It can tell what item the user selected
 */

#ifndef CSELECTIONMENU_H_
#define CSELECTIONMENU_H_

#include "common/CBehaviorEngine.h"
#include "CBaseMenu.h"

struct SelectionEvent : CEvent
{
	SelectionEvent(const int option) :
		mSelection(option) {};

	int mSelection;
};

// Invoked when New Game -> Players selected
struct NewGamePlayersEvent : SelectionEvent
{
	NewGamePlayersEvent(const int players) :
		SelectionEvent(players) {};
};

// Invoked when New Game -> Players selected -> Difficulty selected
struct StartNewGameEvent : CEvent
{
	StartNewGameEvent(const Difficulty difficulty) :
		mDifficulty(difficulty) {};

	const Difficulty mDifficulty;
};


template <class _T>
class CPlayersSelection : public CBaseMenu
{

public:
	CPlayersSelection() :
		CBaseMenu( CRect<float>(0.25f, 0.35f, 0.5f, 0.3f) )
	{

		for( int i = 1 ; i <= MAX_PLAYERS ; i++ )
		{
			const std::string	playerStr = "Player " + itoa(i);
			mpButtonList.push_back( new CGUIButton( playerStr,
													new _T(i) ) );
			mpMenuDialog->addControl( mpButtonList.back() );
		}

	}

	std::list<CGUIButton*> mpButtonList;
};


class CDifficultySelection : public CBaseMenu
{

public:
	CDifficultySelection() :
		CBaseMenu( CRect<float>(0.25f, 0.35f, 0.5f, 0.3f) )
	{

		mpMenuDialog->addControl( new CGUIButton( "Easy", new StartNewGameEvent(EASY) ) );
		mpMenuDialog->addControl( new CGUIButton( "Normal", new StartNewGameEvent(NORMAL) ) );
		mpMenuDialog->addControl( new CGUIButton( "Hard", new StartNewGameEvent(HARD) ) );

	}
};


/*class CSelectionMenu : public CBaseMenu
{
public:
	CSelectionMenu(const Uint8 dlg_theme) :
		CBaseMenu(dlg_theme, CRect<float>(0.10f, 0.10f , 0.10f, 0.10f ) ),
		mSelected(output_selection)
	{

		mpMenuDialog->setBackground( CGUIDialog::VORTICON );

	}

	int fetchSelOptionButtonID()
	{

		std::list<CGUIButton*>::iterator button = mpButtonList.begin();
		for( int i=0 ; button != mpButtonList.end() ; button++, i++ )
		{
			if( (*button)->Up() )
				return i;
		}

		return -1;

	}


	virtual void init() = 0;

	virtual void release() = 0;

private:
	int mSelected;
	std::list<CGUIButton*> mpButtonList;
};*/




#endif /* CSELECTIONMENU_H_ */
