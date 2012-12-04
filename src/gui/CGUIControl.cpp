/*
 * CGUIControl.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include "CGUIControl.h"
#include "sdl/CVideoDriver.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/CInput.h"
#include "sdl/input/InputEvents.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CTimer.h"


int CGUIControl::mTwirliconID;

CGUIControl::CGUIControl() :
mEnabled(true),
mFontID(0),
mHovered(false),
mButtonDown(false),
mButtonUp(false)
{
	CRect<float> NewRect(0.0f, 0.0f, 0.0f, 0.0f);
	mRect = NewRect;
	mTwirliconID = 10;
}



void CGUIControl::drawTwirl( const SDL_Rect& lRect )
{

	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	if( g_pTimer->HasTimeElapsed(100) )
	{
		mTwirliconID++;

		if(mTwirliconID == 15)
			mTwirliconID = 9;
	}

	if( mButtonDown )
	{
		Font.drawCharacter( blitsfc, mTwirliconID, lRect.x+12, lRect.y );
	}
	else if( mHovered )
	{
		Font.drawCharacter( blitsfc, mTwirliconID, lRect.x+8, lRect.y );
	}

}



void CGUIControl::drawBlinker( const SDL_Rect& lRect )
{
	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	if( g_pTimer->HasTimeElapsed(500) )
		mTwirliconID = !mTwirliconID;


	int tile;

	if(!mEnabled)
	{
		tile = (mHovered && mTwirliconID) ? 97 : 96;
	}
	else
	{
		tile = (mTwirliconID && mHovered) ? 93 : 92;
	}

	CTilemap &Tilemap = g_pGfxEngine->getTileMap(2);
	Tilemap.drawTile(blitsfc, lRect.x, lRect.y, tile);
}
