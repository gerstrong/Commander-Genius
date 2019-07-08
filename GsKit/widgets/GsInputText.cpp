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

//
//#include "engine/core/CBehaviorEngine.h"
//#include "core/mode/CGameMode.h"

#ifdef ANDROID
#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
#include <SDL_screenkeyboard.h>
#endif
#endif


const int MAX_TICK = 8; // Units in a logical loop


CGUIInputText::CGUIInputText(const std::string& text ,
                             const int fontID) :
GsButton(text,
         nullptr,
         fontID),
mText(text)
{
    //mFontID = fontID;
}

CGUIInputText::CGUIInputText(const std::string& text,
                             const GsRect<float> &rect,
                             const int fontID) :
GsButton(text,
         rect,
         nullptr,
         fontID),
mText(text)
{
    //mFontID = fontID;
}



bool CGUIInputText::sendEvent(const InputCommand command)
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
