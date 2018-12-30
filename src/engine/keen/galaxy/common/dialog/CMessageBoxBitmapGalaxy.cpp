/*
 * CMessageBoxBitmapGalaxy.cpp
 *
 *  Created on: 04.05.2011
 *      Author: gerstrong
 */

#include "CMessageBoxBitmapGalaxy.h"
#include <base/video/CVideoDriver.h>
#include <base/GsEventContainer.h>

#include "dialog/CMessageBox.h"

namespace galaxy
{

//constexpr int FONT_ID = 0;

CMessageBoxBitmapGalaxy::CMessageBoxBitmapGalaxy(int sprVar,
                                                 const std::string& Text,
                                                 const GsBitmap &BitmapRef,
                                                 const direction_t alignment,
                                                 const bool animation,
                                                 CEvent *closeEv) :
CMessageBoxGalaxy(sprVar, Text, closeEv),
mBitmap(BitmapRef),
mAlignment(alignment)
{
	// Looking if the Bitmap is too big for the Message box. In that case enlarge it!
	if( (mBitmap.height()+26) > mMBRect.h )
	{
		mMBRect.h = mBitmap.height()+26;
	}

	mMBRect.w += (mBitmap.width()+32);

    mMBSurface.createRGBSurface(mMBRect);
    mMBSurface.makeBlitCompatible();

    if(animation)
    {
        // After 240 time frames the dialog closes automatically
        mShowtime = 240;

        mMBAnimatedSurface.createRGBSurface(mMBRect);
        mMBAnimatedSurface.makeBlitCompatible();
    }
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

    const int scaling = gVideoDriver.getOptimalScaling();
    mMBRect.w *= scaling;
    mMBRect.h *= scaling;
    mMBSurface.scaleTo(mMBRect, filterOptionType(scaling) );

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    mMBRect.x = (gameRes.w-mMBRect.w)/2;
    mMBRect.y = (gameRes.h-mMBRect.h)/2;    
}

void CMessageBoxBitmapGalaxy::ponder()
{
    CMessageBoxGalaxy::ponder();

    if(!mMBAnimatedSurface.empty() && mAnimFrame < 6)
    {
        mAnimtimer++;

        if(mAnimtimer >= 15)
        {
            mAnimtimer = 0;

            std::string bmpStr = "KEENTHUMBSUPLOADING";
            bmpStr += itoa(mAnimFrame);

            const GsBitmap &bitmapRef = *gGraphics.getBitmapFromStr(mSprVar, bmpStr);

            const Uint16 bmpX = 10+mBitmap.width()-bitmapRef.width();
            const Uint16 bmpY = 10+mBitmap.height()-bitmapRef.height();
            bitmapRef._draw( bmpX, bmpY, mMBSurface.getSDLSurface() );

            mAnimFrame++;
        }
    }

    if(mShowtime > 0)
    {
        mShowtime--;

        if(mShowtime == 0)
        {
            std::shared_ptr<CEvent> ev(std::move(mCloseEv));
            gEventManager.add( ev );
            mMustClose = true;
        }
    }
}

void CMessageBoxBitmapGalaxy::render()
{
    CMessageBoxGalaxy::render();

    if(!mMBAnimatedSurface.empty())
    {
        BlitSurface(mMBAnimatedSurface.getSDLSurface(), nullptr,
                    gVideoDriver.getBlitSurface(), &mMBRect);
    }
}


void showMsgWithBmp(const int sprVar,
                    const std::string &text,
                    const std::string &bmp,
                    const direction_t alignment,
                    const bool animation,
                    CEvent *closeEv)
{
    CMessageBoxBitmapGalaxy *pMsgBoxBmp =
                new CMessageBoxBitmapGalaxy( sprVar, text,
                                             *gGraphics.getBitmapFromStr(sprVar, bmp),
                                             alignment, animation, closeEv ) ;

    pMsgBoxBmp->init();

    gEventManager.add( new EventSendDialog( dynamic_cast<CMessageBoxGalaxy*>(pMsgBoxBmp) ) );
}

void showMsgWithBmp( const int sprVar,
                     const std::string &text,
                     const unsigned int bmpID,
                     const direction_t alignment,
                     const bool animation,
                     CEvent *closeEv)
{
    CMessageBoxBitmapGalaxy *pMsgBoxBmp =
                new CMessageBoxBitmapGalaxy( sprVar, text,
                                             gGraphics.getBitmapFromId(sprVar, bmpID),
                                             alignment, animation, closeEv ) ;

    pMsgBoxBmp->init();

    gEventManager.add( new EventSendDialog( dynamic_cast<CMessageBoxGalaxy*>(pMsgBoxBmp) ) );
}


}
