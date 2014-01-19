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
#include "Button.h"

namespace galaxy
{

template <class _T>
class CPlayersSelection : public CBaseMenu
{

public:
    CPlayersSelection(bool numFirst = false) :
        CBaseMenu( GsRect<float>(0.25f, 0.35f, 0.5f, 0.3f) )
    {
        GsRect<float> rect(0.30f, 0.282f, 0.5f, 0.5f);
        mpMenuDialog->setRect(rect);
        mpReturnButton = new GalaxyButton( "<<", new CloseMenuEvent() );
        mpMenuDialog->addControl( mpReturnButton, GsRect<float>(-0.1f, 0.1f, 0.03f/rect.w, 0.03f/rect.h) );
        mpReturnButton->setDown(true);

        std::string	playerStr;
        for( unsigned int i = 1 ; i <= MAX_PLAYERS ; i++ )
        {
            if(numFirst)
                playerStr = itoa(i) + " Player";
            else
                playerStr = "Player " + itoa(i);

            GalaxyButton *button = new GalaxyButton( playerStr, new _T(i) );

            mpButtonList.push_back( button );
            mpMenuDialog->addControl( mpButtonList.back() );
        }
    }

    std::list<GalaxyButton*> mpButtonList;
};


class CDifficultySelection : public CBaseMenu
{

public:
    CDifficultySelection() :
        CBaseMenu( GsRect<float>(0.25f, 0.2f, 0.5f, 0.5f) )
    {
        GsRect<float> rect(0.30f, 0.282f, 0.5f, 0.5f);
        mpMenuDialog->setRect(rect);
        mpReturnButton = new GalaxyButton( "<<", new CloseMenuEvent() );
        mpMenuDialog->addControl( mpReturnButton, GsRect<float>(-0.1f, 0.1f, 0.03f/rect.w, 0.03f/rect.h) );
        mpReturnButton->setDown(true);

        mpMenuDialog->addControl( new GalaxyButton( "Easy", new StartNewGameEvent(EASY) ) );
        mpMenuDialog->addControl( new GalaxyButton( "Normal", new StartNewGameEvent(NORMAL) ) );
        mpMenuDialog->addControl( new GalaxyButton( "Hard", new StartNewGameEvent(HARD) ) );
        mpMenuDialog->addControl( new GalaxyButton( "Expert", new StartNewGameEvent(EXPERT) ) );
        mpMenuDialog->addControl( new GalaxyButton( "Ninja", new StartNewGameEvent(NINJA) ) );
        mpMenuDialog->addControl( new GalaxyButton( "Elite", new StartNewGameEvent(ELITE) ) );
        setMenuLabel("NEWGAMELABEL");
    }
};




}


#endif /* CGALAXYSELECTIONMENU_H_ */
