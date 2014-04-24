/*
 * CGUIInputText.cpp
 *
 *  Created on: 25.03.2012
 *      Author: gerstrong
 */

#include "GsInputText.h"
#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>

//#include "sdl/extensions.h"
//#include "engine/core/CBehaviorEngine.h"
//#include "core/mode/CGameMode.h"

#ifdef ANDROID
#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
#include <SDL_screenkeyboard.h>
#endif
#endif


const int MAX_TICK = 8; // Units in a logical loop


CGUIInputText::CGUIInputText(const std::string& text , const int fontID) :
mText(text),
mTyping(false),
mTypeTick(0),
mTick(false)
{
    mFontID = fontID;
}


bool CGUIInputText::sendEvent(const InputCommands command)
{
    if(command == IC_STATUS || command == IC_JUMP)
    {
        mTyping = !mTyping;
        return true;
    }
	return false;
}


void CGUIInputText::processLogic()
{
	if(!mEnabled)
		return;

	// process the typing here!
	if(mTyping)
	{

		if(gInput.getPressedIsTypingKey())
		{
			std::string c = gInput.getPressedTypingKey();

			mText.append(c);
		}

		if(gInput.getPulsedKey(KBCKSPCE, 5) && (mText.length() > 0))
		{
			mText.erase(mText.length()-1);
		}
	}
	else
	{
		mPressed = false;
		mReleased = false;
	}

    processPointingState();

    // If Input Text control was clicked
    if(mReleased)
    {
        mTyping = !mTyping;
    }
}



void CGUIInputText::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

    if(!mEnabled)
        return;

    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    if( mReleased )
    {
        drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00CFCFCF );
    }
    else if( mPressed )
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
    GsFont &Font = gGraphics.getFont(mFontID);

    Font.drawFontCentered( blitsfc, getInputString(), lRect.x, lRect.w, lRect.y, lRect.h,false );
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
#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
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
#endif

	mTyping = value;
}
