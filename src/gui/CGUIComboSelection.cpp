/*
 * CGUIComboSelection.cpp
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#include "CGUIComboSelection.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/CInput.h"
#include "sdl/input/InputEvents.h"
#include "sdl/CVideoDriver.h"
#include "common/CBehaviorEngine.h"
#include "core/mode/CGameMode.h"
#include "sdl/CTimer.h"
#include "Utils.h"


CGUIComboSelection::CGUIComboSelection( const std::string& text,
										     const std::list<std::string>& optionsList ) :
mText(text),
mOptionsList( optionsList ),
mOLCurrent( mOptionsList.begin() ),
drawButton(&CGUIComboSelection::drawNoStyle)
{

	if(g_pBehaviorEngine->getEngine() == ENGINE_VORTICON)
	{
		mFontID = 1;
		drawButton = &CGUIComboSelection::drawVorticonStyle;
	}
	else if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
		mFontID = 1;
		setupButtonSurface();
		drawButton = &CGUIComboSelection::drawGalaxyStyle;
	}
}

void CGUIComboSelection::setupButtonSurface()
{
	CFont &Font = g_pGfxEngine->getFont(mFontID);
	SDL_PixelFormat *format = g_pVideoDriver->getBlitSurface()->format;

	mpTextDarkSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 38, 134, 38));
	mpTextLightSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 84, 234, 84));
	mpTextDisabledSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 123, 150, 123));
}

const std::string& CGUIComboSelection::getSelection()
{
	return *mOLCurrent;
}

void CGUIComboSelection::setSelection( const std::string& selectionText )
{

	std::list<std::string>::const_iterator ptr = mOLCurrent;

	do
	{

		if( *mOLCurrent == selectionText )
			return;

		// Cycle through the Optionslist and find the entry
		mOLCurrent++;
		if( mOLCurrent == mOptionsList.end() )
			mOLCurrent =  mOptionsList.begin();

	} while( ptr != mOLCurrent );

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
	{
		mOptionsList.push_back( strArray[i] );
	}

	mOLCurrent = mOptionsList.begin();

}





void CGUIComboSelection::processLogic()
{
	if(!mEnabled)
		return;

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
				mHovered = true;
				mButtonDown = false;

				// Cycle through the Optionslist
				mOLCurrent++;
				if( mOLCurrent == mOptionsList.end() )
					mOLCurrent =  mOptionsList.begin();

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


void CGUIComboSelection::drawVorticonStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
		return;

	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
	Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );
	const std::string text = (*mOLCurrent);
	Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );

	drawTwirl(lRect);

}


void CGUIComboSelection::drawGalaxyStyle(SDL_Rect& lRect)
{
	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	if(!mEnabled)
	{
		SDL_BlitSurface(mpTextDisabledSfc.get(), NULL, blitsfc, &lRect);
	}
	else
	{
		if(mHovered)
		{
			SDL_BlitSurface(mpTextLightSfc.get(), NULL, blitsfc, &lRect);
		}
		else // Button is not hovered
		{
			SDL_BlitSurface(mpTextDarkSfc.get(), NULL, blitsfc, &lRect);
		}
	}

	drawBlinker(lRect);
}


void CGUIComboSelection::drawNoStyle(SDL_Rect& lRect)
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

	Font.drawFontCentered( blitsfc, mText, lRect.x, lRect.w, lRect.y, lRect.h,false );

}


void CGUIComboSelection::processRender(const CRect<float> &RectDispCoordFloat)
{

	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	(this->*drawButton)(lRect);

}
