/*
 * CGUIInputText.cpp
 *
 *  Created on: 25.03.2012
 *      Author: gerstrong
 */

#include "CGUIInputText.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/CInput.h"
#include "sdl/input/InputEvents.h"
#include "sdl/CVideoDriver.h"
#include "common/CBehaviorEngine.h"
#include "core/mode/CGameMode.h"
#include "sdl/CTimer.h"
#include "sdl/extensions.h"
#ifdef ANDROID
#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
#include <SDL_screenkeyboard.h>
#endif
#endif


const int MAX_TICK = 8; // Units in a logical loop


CGUIInputText::CGUIInputText( const std::string& text,
							  const Style	style ) :
mText(text),
mTyping(false),
mTypeTick(0),
mTick(false),
drawButton(&CGUIInputText::drawNoStyle)
{

	if(g_pBehaviorEngine->getEngine() == ENGINE_VORTICON)
	{
		mFontID = 1;
		drawButton = &CGUIInputText::drawVorticonStyle;
	}
	else if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
		mFontID = 1;
		drawButton = &CGUIInputText::drawGalaxyStyle;
	}

}


bool CGUIInputText::sendEvent(const InputCommands command)
{
	/*if(command == IC_STATUS)
	{
		mTyping = !mTyping;
	}*/
	return false;
}


void CGUIInputText::processLogic()
{
	if(!mEnabled)
		return;

	// process the typing here!
	if(mTyping)
	{

		if(g_pInput->getPressedIsTypingKey())
		{
			std::string c = g_pInput->getPressedTypingKey();

			mText.append(c);
		}

		if(g_pInput->getPulsedKey(KBCKSPCE, 5) && (mText.length() > 0))
		{
			mText.erase(mText.length()-1);
		}
	}
	else
	{
		mButtonDown = false;
		mButtonUp = false;
	}

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
				mTyping = !mTyping;
				g_pInput->m_EventList.pop_Event();
			}
			else if(mouseevent->Type == MOUSEEVENT_BUTTONUP)
			{
				mButtonUp = true;
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


void CGUIInputText::drawGalaxyStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
		return;

	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	SDL_PixelFormat *format = g_pVideoDriver->getBlitSurface()->format;


	const Uint32 oldcolor = Font.getFGColor();


	Uint32 newcolor;

	if(mHovered || mButtonDown)
		newcolor = SDL_MapRGB( format, 84, 234, 84);
	else
		newcolor = SDL_MapRGB( format, 38, 134, 38);

	Font.setupColor( newcolor );

	drawEmptyRect( blitsfc, &lRect, newcolor);

	Font.drawFont( blitsfc, getInputString(), lRect.x+24, lRect.y+2, false );

	Font.setupColor( oldcolor );
}

void CGUIInputText::drawVorticonStyle(SDL_Rect& lRect)
{

	if(!mEnabled)
		return;


	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	Font.drawFont( blitsfc, getInputString(), lRect.x+24, lRect.y, false );

	drawTwirl(lRect);
}

void CGUIInputText::drawNoStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
		return;

	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	if( mButtonUp )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00CFCFCF );
	}
	else if( mButtonDown )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00DFDFDF );
	}
	else if( mHovered )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00EFEFEF );
	}
	else
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00FFFFFF );
	}

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	Font.drawFontCentered( blitsfc, getInputString(), lRect.x, lRect.w, lRect.y, lRect.h,false );
}


void CGUIInputText::processRender(const CRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	(this->*drawButton)(lRect);
}


std::string CGUIInputText::getInputString()
{
	std::string text;
	text = mText;

	if(!mTyping)
		return text;

	if(mTick)
		text += "|";

	if(mTypeTick%MAX_TICK == 0)
		mTick = !mTick;

	mTypeTick++;

	return text;

}

void CGUIInputText::setTypeMode( const bool value )
{
#ifdef ANDROID
	if(!mTyping && value)
	{
		// Invoke Android native text edit field with on-screen keyboard
		char buf[256]; // it must be 256 for SDL_ANDROID_ToggleScreenKeyboardTextInput
		strncpy(buf, mText.c_str(), sizeof(buf));
		buf[sizeof(buf) - 1] = 0;
		SDL_ANDROID_ToggleScreenKeyboardTextInput(buf);
		mText.clear();		
	}
#endif

	mTyping = value;
}
