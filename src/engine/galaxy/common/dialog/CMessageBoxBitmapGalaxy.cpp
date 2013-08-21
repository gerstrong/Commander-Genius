/*
 * CMessageBoxBitmapGalaxy.cpp
 *
 *  Created on: 04.05.2011
 *      Author: gerstrong
 */

#include "CMessageBoxBitmapGalaxy.h"
#include "sdl/CVideoDriver.h"
#include "sdl/extensions.h"

const int FONT_ID = 0;

CMessageBoxBitmapGalaxy::CMessageBoxBitmapGalaxy( const std::string& Text,
														 const CBitmap &BitmapRef,
														 const direction_t alignment ) :
CMessageBoxGalaxy(Text),
mBitmap(BitmapRef),
mAlignment(alignment)
{

	// Looking if the Bitmap is too big for the Message box. In that case enlarge it!
	if( (mBitmap.getHeight()+26) > mMBRect.h )
	{
		mMBRect.h = mBitmap.getHeight()+26;
	}

	mMBRect.w += (mBitmap.getWidth()+32);

	mMBRect.x = (320-mMBRect.w)/2;
	mMBRect.y = (200-mMBRect.h)/2;

	mpMBSurface.reset(CG_CreateRGBSurface( mMBRect ), &SDL_FreeSurface);
//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else
    mpMBSurface.reset(g_pVideoDriver->convertThroughBlitSfc( mpMBSurface.get() ), &SDL_FreeSurface);
//#endif
}

void CMessageBoxBitmapGalaxy::init()
{
	initGalaxyFrame();

	SDL_Rect rect = mMBRect;

	rect.x = 16;

	// Move text to the right if bitmap is on the left side
	if( mAlignment == LEFT )
		rect.x += mBitmap.getWidth();

	rect.w -= 16;
	rect.h -= 8;
	rect.y = (rect.h-mTextHeight)/2;

	initText(rect);

	const Uint16 bmpX = ( mAlignment == LEFT ) ? 10 : mMBRect.w-(mBitmap.getWidth()+32);


	mBitmap._draw( mpMBSurface.get(), bmpX, 10 );
}
