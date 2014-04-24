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

//#include "hardware/Configurator.h"
#include "engine/core/CBehaviorEngine.h"

#include "../KeenEngine.h"

#include <widgets/GsBaseMenu.h>
#include <base/utils/StringUtils.h>

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


#endif /* CSELECTIONMENU_H_ */
