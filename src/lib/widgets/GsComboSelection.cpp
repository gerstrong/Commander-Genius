/*
 * CGUIComboSelection.cpp
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#include "GsComboSelection.h"
#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <lib/base/GsTimer.h>

//#include "common/CBehaviorEngine.h"
//#include "core/mode/CGameMode.h"
//#include "Utils.h"


CGUIComboSelection::CGUIComboSelection( const std::string& text,
										     const std::list<std::string>& optionsList ) :
mText(text),
mOptionsList( optionsList ),
mOLCurrent( mOptionsList.begin() ),
drawButton(&CGUIComboSelection::drawNoStyle)
{

/*	if(g_pBehaviorEngine->getEngine() == ENGINE_VORTICON)
	{
		mFontID = 1;
		drawButton = &CGUIComboSelection::drawVorticonStyle;
	}
	else if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
		mFontID = 1;
		drawButton = &CGUIComboSelection::drawGalaxyStyle;
    }*/
}

void CGUIComboSelection::setupButtonSurface(const std::string &optionText)
{
	GsFont &Font = gGraphics.getFont(mFontID);

	const std::string showText = "  " + mText + ": " + optionText;
    Font.createTextSurface(mTextDarkSfc, showText, 38, 134, 38 );
    Font.createTextSurface(mTextLightSfc, showText, 84, 234, 84 );
    Font.createTextSurface(mTextDisabledSfc, showText, 123, 150, 123 );
}

const std::string& CGUIComboSelection::getSelection()
{
	return *mOLCurrent;
}


void CGUIComboSelection::cycleOption()
{
	// Cycle through the Optionslist
	mOLCurrent++;
	if( mOLCurrent == mOptionsList.end() )
		mOLCurrent =  mOptionsList.begin();

    /*if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
		setupButtonSurface(*mOLCurrent);
    }*/
}



bool CGUIComboSelection::sendEvent(const InputCommands command)
{
	if(command == IC_STATUS || command == IC_JUMP)
	{
		cycleOption();
		return true;
	}
	return false;
}


void CGUIComboSelection::setSelection( const std::string& selectionText )
{

	std::list<std::string>::const_iterator ptr = mOLCurrent;

    /*do
	{
		if( *mOLCurrent == selectionText )
		{
			if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
			{
				setupButtonSurface(selectionText);
			}

			return;
		}

		cycleOption();

    } while( ptr != mOLCurrent );*/

	// Getting at this point means, that this option never existed
	mOptionsList.push_back( selectionText );
	mOLCurrent = mOptionsList.end();
	mOLCurrent--;
}





void CGUIComboSelection::setList(const char **strArray, const int numElem)
{
	if(!mOptionsList.empty())
		mOptionsList.clear();


	for( int i=0 ; i<numElem ; i++ )
		mOptionsList.push_back( strArray[i] );

	mOLCurrent = mOptionsList.begin();

}



void CGUIComboSelection::processLogic()
{
	if(!mEnabled)
		return;

	// Here we check if the mouse-cursor/Touch entry clicked on our Button
	if( PointingDevEvent *mouseevent = gInput.m_EventList.occurredEvent<PointingDevEvent>() )
	{
		CVec MousePos = mouseevent->Pos;

		if( mRect.HasPoint(MousePos) )
		{
			if(mouseevent->Type == PDE_MOVED)
			{
				mHovered = true;
				gInput.m_EventList.pop_Event();
			}
			else if(mouseevent->Type == PDE_BUTTONDOWN)
			{
                mPressed = true;
				gInput.m_EventList.pop_Event();
			}
			else if(mouseevent->Type == PDE_BUTTONUP)
			{
                mReleased = true;
				mHovered = true;
                mPressed = false;

				cycleOption();

				gInput.m_EventList.pop_Event();
			}
		}
		else
		{
			mHovered = false;
            mPressed = false;
            mReleased = false;
		}
	}
}


void CGUIComboSelection::drawVorticonStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
		return;

	SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

	// Now lets draw the text of the list control
	GsFont &Font = gGraphics.getFont(mFontID);

	Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
	Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );
	const std::string text = (*mOLCurrent);
	Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );

	drawTwirl(lRect);

}


void CGUIComboSelection::drawGalaxyStyle(SDL_Rect& lRect)
{
    GsWeakSurface sfc(gVideoDriver.getBlitSurface());

	if(!mEnabled)
	{        
        mTextDisabledSfc.blitTo(sfc, lRect);
	}
	else
	{
		if(mHovered)
		{
            mTextLightSfc.blitTo(sfc, lRect);
		}
		else // Button is not hovered
		{
            mTextDarkSfc.blitTo(sfc, lRect);
		}
	}

	drawBlinker(lRect);
}


void CGUIComboSelection::drawNoStyle(SDL_Rect& lRect)
{
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

	Font.drawFontCentered( blitsfc, mText, lRect.x, lRect.w, lRect.y, lRect.h,false );

}


void CGUIComboSelection::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	(this->*drawButton)(lRect);

}
