/*
 * CSelectionMenu.h
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 *
 *  Based on the work of Pizza2004
 *  This is template for normal selection menus. It can tell what item the user selected
 */

#ifndef SELECTIONMENU_H_INCLUDED
#define SELECTIONMENU_H_INCLUDED

#include <widgets/GsMenuController.h>

#include "widgets/Button.h"
#include "widgets/ComboSelection.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/menu/GameMenu.h"
#include "fileio/KeenFiles.h"
#include "fileio/ResourceMgmt.h"
#include "engine/keen/KeenEngine.h"


#include <base/utils/StringUtils.h>
#include <base/utils/FindFile.h>


struct SelectionEvent : CEvent
{
    SelectionEvent(const int option) :
        mSelection(option) {}

    int mSelection;
};

// Invoked when New Game -> Players selected
struct NewGamePlayersEvent : SelectionEvent
{
    NewGamePlayersEvent(const int players) :
        SelectionEvent(players) {}
};



template <class _T>
class CPlayersSelection : public GameMenu
{

public:
    CPlayersSelection(bool numFirst, 
                      const GsControl::Style style) :
        GameMenu( GsRect<float>(0.25f, 0.35f, 0.5f, 0.3f), 
                  style )
    {
        std::string	playerStr;
        for( unsigned int i = 1 ; i <= MAX_PLAYERS ; i++ )
        {
            if(numFirst)
                playerStr = itoa(i) + " Player";
            else
                playerStr = "Player " + itoa(i);

            GameButton *button = new GameButton( playerStr, new _T(i), style );

            mpButtonList.push_back( button );
            mpMenuDialog->addControl( mpButtonList.back() );
        }
    }

    std::list<GameButton*> mpButtonList;
};


class CDifficultySelection : public GameMenu
{

public:
    CDifficultySelection(const GsControl::Style style) :
        GameMenu( GsRect<float>(0.25f, 0.2f, 0.5f, 0.5f), style )
    {
        mpMenuDialog->addControl( new GameButton( "EASY", new StartNewGameEvent(EASY), style ) );
        mpMenuDialog->addControl( new GameButton( "NORMAL", new StartNewGameEvent(NORMAL), style ) );
        mpMenuDialog->addControl( new GameButton( "HARD", new StartNewGameEvent(HARD), style ) );
        mpMenuDialog->addControl( new GameButton( "EXPERT", new StartNewGameEvent(EXPERT), style ) );
        mpMenuDialog->addControl( new GameButton( "NINJA", new StartNewGameEvent(NINJA), style ) );
        mpMenuDialog->addControl( new GameButton( "ELITE", new StartNewGameEvent(ELITE), style ) );

        setMenuLabel("NEWGAMELABEL");

        mpMenuDialog->setSelection(1);
    }
};


class CPlayerSpriteVarSelection : public GameMenu
{
public:
    CPlayerSpriteVarSelection(const GsControl::Style style);

    void changeEvent();

    // Processes the stuff that the menus have in common
    void ponder(const float deltaT) override;

private:

    CGUIBitmap *mBmpBox;

    std::vector< std::shared_ptr<GsBitmap> > mpPlayerBmpVec;

    std::map<std::string, int> mIdMap;

    int mCurIdx = 0;

    ComboSelection *mpComboSelection = nullptr;

    GameButton *mpGameButton = nullptr;
    GameButton *mpChangeButton = nullptr;

};



#endif /* SELECTIONMENU_H_INCLUDED */
