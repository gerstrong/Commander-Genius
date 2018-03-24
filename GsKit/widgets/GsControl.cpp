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

GsControl::GsControl(const Style style) :    
    mStyle(style) 
{
    GsRect<float> NewRect(0.0f, 0.0f, 0.0f, 0.0f);
	mRect = NewRect;
	mTwirliconID = 10;    
}

GsControl::GsControl() :
    GsControl(Style::UNSET) {}


void GsControl::processBlendEffects()
{
    if(mEnabled)
    {
        // For some nice special effects
        if(mHovered || mSelected)
        {
            int maxBlend = 224;

            if(mHovered && mSelected)
            {
                maxBlend = 255;
            }

            if(mLightRatio+BLEND_SPEED < maxBlend)
               mLightRatio += BLEND_SPEED;
            else
               mLightRatio = maxBlend;
        }
        else // Button is not hovered
        {
            if(mLightRatio-BLEND_SPEED > 0)
               mLightRatio -= BLEND_SPEED;
            else
               mLightRatio = 0;
        }
    }
}

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

    if(!bDown || mPressed)
    {
        mHovered = hasPoint;
    }

    if(mHovered && bDown)
    {
        mPressed = true;
    }
}


void GsControl::drawTwirl( const SDL_Rect& lRect )
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

	// Now lets draw the text of the list control
	GsFont &Font = gGraphics.getFont(mFontID);

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
    if(mHovered)
    {
        Font.drawCharacter( blit.getSDLSurface(), 127, lRect.x+16, lRect.y );
    }

}



void GsControl::drawBlinker( const SDL_Rect& lRect )
{
	SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

	if( gTimer.HasTimeElapsed(500) )
		mTwirliconID = !mTwirliconID;


	int tile;

    const bool blink = (mHovered || mSelected) && mTwirliconID;

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
