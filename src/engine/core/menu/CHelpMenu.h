/*
 * CHelpMenu.h
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#ifndef CHELPMENU_H_
#define CHELPMENU_H_

#include "GameMenu.h"

struct StartHelpEv : CEvent
{
    enum class Variant
    {
        THEGAME,
        STORY,
        ORDERING,
        ABOUT_ID,
        ABOUT_CG,
        CREDITS,
        PREVIEWS
    } mType;

    StartHelpEv(const Variant value) : mType(value) {}
};


class CHelpMenu : public GameMenu
{
public:
    CHelpMenu(const Style style);
};

#endif /* CHELPMENU_H_ */
