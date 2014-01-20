/*
 * CSelectionMenu.h
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 *
 *  Based on the work of Pizza2004
 *  This is template for normal selection menus. It can tell what item the user selected
 */

#ifndef CVORITCONSELECTIONMENU_H_
#define CVORITCONSELECTIONMENU_H_


#include "../../menu/CSelectionMenu.h"
#include "VorticonMenu.h"

namespace vorticon
{

template <class _T>
class CPlayersSelection : public VorticonMenu
{

public:
    CPlayersSelection(bool numFirst = false) :
        VorticonMenu( GsRect<float>(0.25f, 0.35f, 0.5f, 0.3f) )
    {
            std::string	playerStr;
            for( unsigned int i = 1 ; i <= MAX_PLAYERS ; i++ )
            {
                if(numFirst)
                    playerStr = itoa(i) + " Player";
                else
                    playerStr = "Player " + itoa(i);

                GsButton *button = new GsButton( playerStr, new _T(i) );

                mpButtonList.push_back( button );
                mpMenuDialog->addControl( mpButtonList.back() );
            }
    }

    std::list<GsButton*> mpButtonList;
};


class CDifficultySelection : public VorticonMenu
{

public:
    CDifficultySelection() :
        VorticonMenu( GsRect<float>(0.25f, 0.2f, 0.5f, 0.5f) )
    {
        mpMenuDialog->addControl( new GsButton( "Easy", new StartNewGameEvent(EASY) ) );
        mpMenuDialog->addControl( new GsButton( "Normal", new StartNewGameEvent(NORMAL) ) );
        mpMenuDialog->addControl( new GsButton( "Hard", new StartNewGameEvent(HARD) ) );
        mpMenuDialog->addControl( new GsButton( "Expert", new StartNewGameEvent(EXPERT) ) );
        mpMenuDialog->addControl( new GsButton( "Ninja", new StartNewGameEvent(NINJA) ) );
        mpMenuDialog->addControl( new GsButton( "Elite", new StartNewGameEvent(ELITE) ) );
        setMenuLabel("NEWGAMELABEL");
    }
};

};



#endif /* CVORITCONSELECTIONMENU_H_ */
