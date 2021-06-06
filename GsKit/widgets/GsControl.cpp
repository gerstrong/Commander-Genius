/*
 * GsControl.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include <base/PointDevice.h>

#include "GsControl.h"

const int BLEND_SPEED = 16;

int GsControl::mTwirliconID;


GsControl::GsControl(const GsRect<float> &rect) :
GsWidget(rect)
{}

GsControl::GsControl() :
GsControl(GsRect<float>(0.0f, 0.0f, 0.0f, 0.0f))
{
    mTwirliconID = 10;
}

GsControl::~GsControl()
{}

void GsControl::processPointingState(const GsRect<float> &rect)
{
    GsPointingState &pointingState = gPointDevice.mPointingState;

    const bool hasPoint = rect.HasPoint(pointingState.mPos);
    const bool bDown = (pointingState.mActionButton>0);


    mReleased = false;

    if(!bDown && mPressed)
    {
        mPressed = false;

        if(hasPoint)
        {
            mReleased = true;
        }
    }

#ifndef DISABLE_HOVER
    if(!bDown || mPressed)
    {
        mHovered = hasPoint;        
    }

    if(mHovered && bDown)
    {
        mPressed = true;
    }
#else
    if(bDown)
    {
        if(hasPoint)
        {
            mPressed = true;
        }
    }
#endif
}


void GsControl::processPointingStateRel(const GsRect<float> &parentRect)
{
    const auto absRect = parentRect.transformed(getRect());
    processPointingState(absRect);
}



void GsControl::drawTwirl( const SDL_Rect& lRect )
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

	// Now lets draw the text of the list control
    GsFontLegacy &Font = gGraphics.getFontLegacy(0);

	if( gTimer.HasTimeElapsed(100) )
	{
		mTwirliconID++;

        if(mTwirliconID == 15)
            mTwirliconID = 9;
    }

    if( mPressed )
	{
        Font.drawCharacter( blit.getSDLSurface(), mTwirliconID, lRect.x+12, lRect.y );
	}
    else if( mSelected )
	{
        Font.drawCharacter( blit.getSDLSurface(), mTwirliconID, lRect.x+8, lRect.y );
	}

    // Now lets draw the text of the button
#ifndef DISABLE_HOVER
    if(mHovered)
    {
        Font.drawCharacter( blit.getSDLSurface(), 127, lRect.x+16, lRect.y );
    }
#endif

}



void GsControl::drawBlinker( const SDL_Rect& lRect )
{
	SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

	if( gTimer.HasTimeElapsed(500) )
		mTwirliconID = !mTwirliconID;

#ifndef DISABLE_HOVER
    const bool blink = (mHovered || mSelected) && mTwirliconID;
#else
    const bool blink = mSelected && mTwirliconID;
#endif

    int tile;

	if(!mEnabled)
	{
        tile = blink ? 97 : 96;
	}
	else
	{
        tile = blink ? 93 : 92;
	}

    GsTilemap &Tilemap = gGraphics.getTileMap(2);
    Tilemap.drawTile(blitsfc, lRect.x, lRect.y, tile);
}
