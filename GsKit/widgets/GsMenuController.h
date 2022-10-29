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

#ifndef GSMENUCONTROLLER_H
#define GSMENUCONTROLLER_H


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
    OpenMenuEvent(CBaseMenu* menuDlgPtr) :
        mMenuDialogPointer(menuDlgPtr) {}

    std::shared_ptr<CBaseMenu> mMenuDialogPointer;
};

struct CloseMenuEvent : CEvent
{
    /**
     * @brief CloseMenuEvent
     * @param replayMusic   If menu are closed so it goes back to the title
     *                      or gameplay whatever is open, trigger a ReplayMusic
     *                      Event
     */
    CloseMenuEvent(const bool replayMusic) :
        mReplayMusic(replayMusic) {}

    const bool mReplayMusic;
};


struct CloseAllMenusEvent : CEvent
{};

/**
 * @brief Sent when all the menus are closed
 */
struct EventAllMenusClosed : CEvent
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

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr);

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

    std::function<void(GsWeakSurface&)> mBackroundDrawFcn;
    std::function<void(GsWeakSurface&, const GsRect<int>)> mDrawTwirlFcn;
    std::function<void()> mExecAfterClose;

    bool mEnableTwirl = false;

    void updateGraphics();

private:

    void popBackMenu();

    std::list< std::shared_ptr<CBaseMenu> > mMenuStack;

    bool mLocked;
    bool mHidden;
};

#endif /* GSMENUCONTROLLER_H */
