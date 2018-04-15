/*
 * CSelectionMenu.h
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 *
 *  Based on the work of Pizza2004
 *  This is template for normal selection menus. It can tell what item the user selected
 */

#ifndef CGALAXYSELECTIONMENU_H_
#define CGALAXYSELECTIONMENU_H_

#include <widgets/GsMenuController.h>

#include "../../menu/CSelectionMenu.h"
#include "widgets/Button.h"
/*
namespace galaxy
{

template <class _T>
class CPlayersSelection : public GalaxyMenu
{

public:
    CPlayersSelection(bool numFirst = false) :
        GalaxyMenu( GsRect<float>(0.25f, 0.35f, 0.5f, 0.3f) )
    {
        std::string	playerStr;
        for( unsigned int i = 1 ; i <= MAX_PLAYERS ; i++ )
        {
            if(numFirst)
                playerStr = itoa(i) + " Player";
            else
                playerStr = "Player " + itoa(i);

            GameButton *button = new GameButton( playerStr, new _T(i) );

            mpButtonList.push_back( button );
            mpMenuDialog->addControl( mpButtonList.back() );
        }
    }

    std::list<GameButton*> mpButtonList;
};


class CDifficultySelection : public GalaxyMenu
{

public:
    CDifficultySelection() :
        GalaxyMenu( GsRect<float>(0.25f, 0.2f, 0.5f, 0.5f) )
    {
        mpMenuDialog->addControl( new GameButton( "EASY", new StartNewGameEvent(EASY) ) );
        mpMenuDialog->addControl( new GameButton( "NORMAL", new StartNewGameEvent(NORMAL) ) );
        mpMenuDialog->addControl( new GameButton( "HARD", new StartNewGameEvent(HARD) ) );
        mpMenuDialog->addControl( new GameButton( "EXPERT", new StartNewGameEvent(EXPERT) ) );
        mpMenuDialog->addControl( new GameButton( "NINJA", new StartNewGameEvent(NINJA) ) );
        mpMenuDialog->addControl( new GameButton( "ELITE", new StartNewGameEvent(ELITE) ) );
        setMenuLabel("NEWGAMELABEL");

        mpMenuDialog->setSelection(1);
    }
};


class CPlayerSpriteVarSelection : public GalaxyMenu
{
public:
    CPlayerSpriteVarSelection() :
        GalaxyMenu( GsRect<float>(0.25f, 0.2f, 0.5f, 0.5f) )
    {
        mpMenuDialog->addControl( new GameButton( "Sprite 1", new SelectPlayerSpriteVarEvent(0)) );
        mpMenuDialog->addControl( new GameButton( "Sprite 2", new SelectPlayerSpriteVarEvent(1)) );
        mpMenuDialog->addControl( new GameButton( "Sprite 3", new SelectPlayerSpriteVarEvent(2)) );
        mpMenuDialog->addControl( new GameButton( "Sprite 4", new SelectPlayerSpriteVarEvent(3)) );
        setMenuLabel("NEWGAMELABEL");
    }
};




}

*/
#endif /* CGALAXYSELECTIONMENU_H_ */
