/*
 * CMenuController.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong
 */

#include "GsMenuController.h"
#include <base/CInput.h>
#include <base/PointDevice.h>
#include <base/video/CVideoDriver.h>
#include <base/audio/music/CMusicPlayer.h>
#include <cassert>

void drawMenuInGameButton(const SDL_Rect &buttonRect)
{
    SDL_Surface *dst = gVideoDriver.getBlitSurface();


    SDL_Rect tempRect = buttonRect;

    SDL_FillRect(dst, &tempRect, SDL_MapRGB(dst->format, 255, 0, 0) );

    SDL_Rect line = buttonRect;
    line.w -= 4;    line.h  = 2; line.x += 2;
    line.y +=2;

    // This draws the classical sandwich icon
    for(;line.y<buttonRect.y+buttonRect.h-2;line.y+=5)
    {
        SDL_FillRect(dst, &line, SDL_MapRGB(dst->format, 255, 255, 255) );
    }
}


bool checkSandwichMenuClicked(GsRect<float> &rRect)
{
    GsPointingState &pointingState = gPointDevice.mPointingState;

    const bool hasPoint = rRect.HasPoint(pointingState.mPos);
    const bool bDown = (pointingState.mActionButton > 0);

    return (bDown && hasPoint);
}


void CMenuController::clearMenuStack()
{
    while(!mMenuStack.empty())
    {
        popBackMenu();
        mBackroundDrawFcn = nullptr;
    }

}


void CMenuController::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    if( const OpenMenuEvent* openMenu = dynamic_cast<const OpenMenuEvent*>(evPtr.get()) )
    {
        CBaseMenu &menu = *openMenu->mMenuDialogPointer.get();
        menu.refresh();

        // Select the second element. The first one (0) is the close button.
        menu.select(1);

        if( !mMenuStack.empty() )
        {
            menu.setProperty( CBaseMenu::CANGOBACK );
        }

        mMenuStack.push_back( openMenu->mMenuDialogPointer );
    }
    else if( auto *cme = dynamic_cast<const CloseMenuEvent*>(evPtr.get()) )
    {
        gInput.flushAll();
        popBackMenu();

        if(cme->mReplayMusic)
        {
            gEventManager.add(new EventReloadMusic);
        }
    }
    else if( dynamic_cast<const CloseAllMenusEvent*>(evPtr.get()) )
    {
        clearMenuStack();
    }
}


void CMenuController::ponder(const float deltaT)
{
    // Process Menu if open
    if( !mMenuStack.empty() )
    {
        mMenuStack.back()->ponder(deltaT);
    }
}

void CMenuController::popBackMenu()
{
    // This should not be called when menu stack is empty
    assert(!mMenuStack.empty());

    mMenuStack.back()->release();
    mMenuStack.pop_back();

    if(mMenuStack.empty())
    {
        if(mExecAfterClose)
            mExecAfterClose();
    }

}


void CMenuController::render()
{
    // Render the Menu if open
    if( !mMenuStack.empty() && !mHidden )
    {
        mMenuStack.back()->render();
    }
}


void CMenuController::updateGraphics()
{
    for( auto &menu : mMenuStack)
    {
        menu->updateGraphics();
    }
}
