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
#include "widgets/Button.h"

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

                Button *button = new Button( playerStr, new _T(i) );

                mpButtonList.push_back( button );
                mpMenuDialog->addControl( mpButtonList.back() );
            }
    }

    std::list<Button*> mpButtonList;
};


class CDifficultySelection : public VorticonMenu
{

public:
    CDifficultySelection() :
        VorticonMenu( GsRect<float>(0.25f, 0.2f, 0.5f, 0.5f) )
    {
        mpMenuDialog->addControl( new Button( "Easy", new StartNewGameEvent(EASY) ) );
        mpMenuDialog->addControl( new Button( "Normal", new StartNewGameEvent(NORMAL) ) );
        mpMenuDialog->addControl( new Button( "Hard", new StartNewGameEvent(HARD) ) );
        mpMenuDialog->addControl( new Button( "Expert", new StartNewGameEvent(EXPERT) ) );
        mpMenuDialog->addControl( new Button( "Ninja", new StartNewGameEvent(NINJA) ) );
        mpMenuDialog->addControl( new Button( "Elite", new StartNewGameEvent(ELITE) ) );
    }
};

};



#endif /* CVORITCONSELECTIONMENU_H_ */
