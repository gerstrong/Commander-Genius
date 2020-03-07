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


CGUIInputText::CGUIInputText(const std::string& text,
                             const int fontID) :
GsButton(text,
         nullptr,
         fontID)
{}

CGUIInputText::CGUIInputText(const std::string& text,
                             const GsRect<float> &rect,
                             const int fontID) :
GsButton(text,
         rect,
         nullptr,
         fontID)
{}



bool CGUIInputText::sendEvent(const InputCommand command)
{
    if(!isEnabled())
        return false;

    if(command == IC_STATUS || command == IC_JUMP)
    {
        mTyping = !mTyping;
        GsButton::sendEvent(command);
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

        auto appText = getText();

		if(gInput.getPressedIsTypingKey())
		{
			std::string c = gInput.getPressedTypingKey();

            appText.append(c);
            GsButton::setText(appText);
		}

        if(gInput.getPulsedKey(KBCKSPCE, 5) && (appText.length() > 0))
		{
            appText.erase(appText.length()-1);
            GsButton::setText(appText);
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
    auto displayRect = getRect();

    displayRect.transform(RectDispCoordFloat);
    auto lRect = displayRect.SDLRect();

    drawNoStyle(lRect);

    if(!mTyping)
    {
        mTextWidget.processRender(displayRect);
        return;
    }

    if(mTypeTick%MAX_TICK == 0)
    {
        mTick = !mTick;

        const auto oldText = getText();
        auto text = oldText;

/*
        mOrigText;
        mTextWithCursor;
*/

        if(mTick)
        {
            text = text + "|";
        }
        else
        {

        }

        setText(text);

        mTextWidget.processRender(displayRect);
        setText(oldText);
    }
    else
    {
        mTextWidget.processRender(displayRect);
    }

    mTypeTick++;
}

void CGUIInputText::processRender(const GsRect<float> &backRect,
                             const GsRect<float> &frontRect)
{
    // Transform this object display coordinates
    auto objBackRect = backRect.transformed(getRect());
    auto objFrontRect = objBackRect.clipped(frontRect);

    drawNoStyle( objFrontRect.SDLRect() );

    mTextWidget.processRender(objBackRect,
                              objFrontRect);
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
