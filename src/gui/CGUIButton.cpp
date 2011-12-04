/*
 * CGUIButton.cpp
 *
 *  Created on: 26.11.2011
 *      Author: gerstrong
 */

#include "CGUIButton.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/CInput.h"
#include "sdl/input/InputEvents.h"
#include "sdl/CVideoDriver.h"
#include "common/CBehaviorEngine.h"
#include "core/CGameMode.h"

CGUIButton::CGUIButton(const std::string& text, const SmartPointer<CEvent> ev) :
mHovered(false),
mButtonDown(false),
mButtonUp(false),
mText(text),
mEvent(ev)
{}


void CGUIButton::processLogic()
{
	// Here we check if the mouse-cursor/Touch entry clicked on our Button
	if( MouseMoveEvent *mouseevent = g_pInput->m_EventList.occurredEvent<MouseMoveEvent>() )
	{
		CVec MousePos = mouseevent->Pos;

		if( mRect.HasPoint(MousePos) )
		{
			if(mouseevent->Type == MOUSEEVENT_MOVED)
			{
				mHovered = true;
				g_pInput->m_EventList.pop_Event();
			}
			else if(mouseevent->Type == MOUSEEVENT_BUTTONDOWN)
			{
				mButtonDown = true;
				g_pInput->m_EventList.pop_Event();
			}
			else if(mouseevent->Type == MOUSEEVENT_BUTTONUP)
			{
				mButtonUp = true;
				g_pBehaviorEngine->m_EventList.add(mEvent);
				g_pInput->m_EventList.pop_Event();
			}
		}
		else
		{
			mHovered = false;
			mButtonDown = false;
			mButtonUp = false;
		}
	}
}

void CGUIButton::processRender(const CRect<float> &RectDispCoordFloat)
{
	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	if( mButtonUp )
	{
		SDL_FillRect( blitsfc, &lRect, 0x000000FF );
	}
	else if( mButtonDown )
	{
		SDL_FillRect( blitsfc, &lRect, 0x0000FF00 );
	}
	else if( mHovered )
	{
		SDL_FillRect( blitsfc, &lRect, 0x00FF0000 );
	}

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(0);

	Font.drawFontCentered( blitsfc, mText, lRect.x, lRect.w, lRect.y, false );
}
