/*
 * CMenuController.h
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong
 */

#include <base/GsEvent.h>
#include <base/Singleton.h>
#include <memory>

#include "GsBaseMenu.h"

#ifndef __GSMENUCONTROLLER_H__
#define __GSMENUCONTROLLER_H__


#define gMenuController CMenuController::get()


/**
 * @brief drawMenuInGameButton  Draws a sandwich that is visible while in gameplay
 * @param buttonRect    where and how large to draw that menu...
 */
void drawMenuInGameButton(const SDL_Rect &buttonRect);

/**
 * @brief checkSandwichMenuClicked  Return true if sandwich menu was clicked
 */
bool checkSandwichMenuClicked(GsRect<float> &rRect);

/**
 * Events
 */


struct OpenMenuEvent : CEvent
{
    OpenMenuEvent(CBaseMenu* menuDlgPtr) : mMenuDialogPointer(menuDlgPtr) {}

	std::shared_ptr<CBaseMenu> mMenuDialogPointer;
};

struct CloseMenuEvent : CEvent
{};

struct CloseAllMenusEvent : CEvent
{};

/**
 * Class Declaration
 */


class CMenuController : public GsSingleton<CMenuController>
{

public:
	CMenuController() : mOpenedGamePlay(false),
                          mLocked(false),
                          mHidden(false) {}

    void clearMenuStack();

    void pumpEvent(const CEvent *evPtr);

    void ponder(const float deltaT);

    void render();

	bool active()
	{	return	!mMenuStack.empty();	}

    void lock(const bool value)
    {	mLocked = value;	}

    bool isLocked()
    {   return mLocked; }

    void hide(const bool value)
    {	mHidden = value;	}

    bool empty()
    {   return mMenuStack.empty();    }

	bool mOpenedGamePlay;

    void updateGraphics();

private:

	void popBackMenu();
	
	std::list< std::shared_ptr<CBaseMenu> > mMenuStack;

    bool mLocked;
    bool mHidden;
};

#endif /* __GSMENUCONTROLLER_H__ */
