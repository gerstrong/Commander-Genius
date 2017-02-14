/*
 * CMessageBoxBitmapGalaxy.cpp
 *
 *  Created on: 04.05.2011
 *      Author: gerstrong
 */

#include "CMessageBoxBitmapGalaxy.h"
#include <base/video/CVideoDriver.h>
#include "sdl/extensions.h"
#include "dialog/CMessageBox.h"

namespace galaxy
{

const int FONT_ID = 0;

CMessageBoxBitmapGalaxy::CMessageBoxBitmapGalaxy(const std::string& Text,
                                                  const GsBitmap &BitmapRef,
                                                  const direction_t alignment,
                                                  CEvent *closeEv) :
CMessageBoxGalaxy(Text, closeEv),
mBitmap(BitmapRef),
mAlignment(alignment)
{
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

	// Looking if the Bitmap is too big for the Message box. In that case enlarge it!
	if( (mBitmap.height()+26) > mMBRect.h )
	{
		mMBRect.h = mBitmap.height()+26;
	}

	mMBRect.w += (mBitmap.width()+32);

    mMBRect.x = (gameRes.w-mMBRect.w)/2;
    mMBRect.y = (gameRes.h-mMBRect.h)/2;

    mMBSurface.createRGBSurface(mMBRect);
    mMBSurface.makeBlitCompatible();
}

void CMessageBoxBitmapGalaxy::init()
{
	initGalaxyFrame();

	SDL_Rect rect = mMBRect;

	rect.x = 16;

	// Move text to the right if bitmap is on the left side
	if( mAlignment == LEFT )
		rect.x += mBitmap.width();

	rect.w -= 16;
	rect.h -= 8;
	rect.y = (rect.h-mTextHeight)/2;

	initText(rect);

	const Uint16 bmpX = ( mAlignment == LEFT ) ? 10 : mMBRect.w-(mBitmap.width()+32);

    mBitmap._draw( bmpX, 10, mMBSurface.getSDLSurface() );
}


void showMsgWithBmp( const std::string &text,
                     const std::string &bmp,
                     const direction_t alignment,
                     CEvent *closeEv)
{
    CMessageBoxBitmapGalaxy *pMsgBoxBmp =
                new CMessageBoxBitmapGalaxy( text,
                                             *gGraphics.getBitmapFromStr(bmp),
                                             alignment, closeEv ) ;

    pMsgBoxBmp->init();

    gEventManager.add( new EventSendDialog( dynamic_cast<CMessageBoxGalaxy*>(pMsgBoxBmp) ) );
}

void showMsgWithBmp( const std::string &text,
                     const unsigned int bmpID,
                     const direction_t alignment,
                     CEvent *closeEv)
{
    CMessageBoxBitmapGalaxy *pMsgBoxBmp =
                new CMessageBoxBitmapGalaxy( text,
                                             gGraphics.getBitmapFromId(bmpID),
                                             alignment, closeEv ) ;

    pMsgBoxBmp->init();

    gEventManager.add( new EventSendDialog( dynamic_cast<CMessageBoxGalaxy*>(pMsgBoxBmp) ) );
}


}
