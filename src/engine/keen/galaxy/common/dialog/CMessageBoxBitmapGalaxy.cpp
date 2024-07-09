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

CMessageBoxBitmapGalaxy::CMessageBoxBitmapGalaxy(int sprVar,
                                                 const std::string& Text,
                                                 const GsBitmap &BitmapRef,
                                                 const direction_t alignment,
                                                 const bool animation,
                                                 CEvent *closeEv) :
CMessageBoxGalaxy(sprVar, Text, closeEv),
mMainBitmap(BitmapRef),
mAlignment(alignment)
{
	// Looking if the Bitmap is too big for the Message box. In that case enlarge it!
    if( (mMainBitmap.height()+26) > mMBRect.h )
	{
        mMBRect.h = mMainBitmap.height()+26;
	}

    mMBRect.w += (mMainBitmap.width()+32);

    mMBSurface.createRGBSurface(mMBRect);
    mMBSurface.makeBlitCompatible();

    if(animation)
    {
        // After 240 time frames the dialog closes automatically
        mShowtime = 240;
        mHasAnimation = true;
    }
}

void CMessageBoxBitmapGalaxy::init()
{
	initGalaxyFrame();

	SDL_Rect rect = mMBRect;

	rect.x = 16;

	// Move text to the right if bitmap is on the left side
	if( mAlignment == LEFT )
        rect.x += mMainBitmap.width();

    rect.w -= 16;
	rect.h -= 8;
	rect.y = (rect.h-mTextHeight)/2;

	initText(rect);

    const int scaling = gVideoDriver.getOptimalScaling();

    mMBRect.w *= scaling;
    mMBRect.h *= scaling;

    mMBSurface.scaleTo(mMBRect, VidFilter(scaling) );

    renderCurrentBitmap(0);

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    mMBRect.x = (gameRes.dim.x-mMBRect.w)/2;
    mMBRect.y = (gameRes.dim.y-mMBRect.h)/2;
}

void CMessageBoxBitmapGalaxy::renderCurrentBitmap(const int frametoRender)
{
    GsBitmap frame(mMainBitmap);

    if(frametoRender > 0)
    {
        std::string aniBmpStr = "KEENTHUMBSUPLOADING";
        aniBmpStr += itoa(frametoRender);

        const GsBitmap &aniBmpRef = *gGraphics.getBitmapFromStr(mSprVar, aniBmpStr);

        GsVec2D<Uint16> aniBmpPos =
            { frame.width()-aniBmpRef.width(),
             frame.height()-aniBmpRef.height() };

        // Creates the right animation of the frame to blit
        aniBmpRef._draw( aniBmpPos.x, aniBmpPos.y, frame.getSDLSurface() );
    }

    const int scaling = gVideoDriver.getOptimalScaling();

    {
        const Uint16 bmpX = ( mAlignment == LEFT ) ? 10 :
                (mMBSurface.width()/scaling)-(frame.width()+20);
        GsVec2D<Uint16> bmpPos = {bmpX, 10};

        const GsRect<Uint16> frameRect =
            {0, 0,
             static_cast<Uint16>(frame.width()*scaling),
             static_cast<Uint16>(frame.height()*scaling)};
        frame.scaleTo(frameRect);

        bmpPos *= scaling;

        frame._draw( bmpPos.x, bmpPos.y, mMBSurface.getSDLSurface() );
    }
}

void CMessageBoxBitmapGalaxy::ponder(const int deltaT)
{
    CMessageBoxGalaxy::ponder(deltaT);

    if(mHasAnimation && mAnimFrame < 6)
    {
        mAnimtimer++;

        if(mAnimtimer >= 15)
        {
            mAnimtimer = 0;

            renderCurrentBitmap(mAnimFrame);

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
}


void showModalMsgWithBmp(const int sprVar,
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

void showModalMsgWithBmp( const int sprVar,
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
