/*
 * CMessageBoxGalaxy.cpp
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 *
 *  This class basically handles the message boxes seen 
 *  within the galaxy games. This box is shown whenever there is a Dialog between
 *  Keen and one of the oracles.
 */

#include "CMessageBoxGalaxy.h"
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include "graphics/GsGraphics.h"
#include "../../common/TrophyMsg.h"
#include "engine/BgMsg.h"

#include <base/utils/StringUtils.h>
#include <memory>

#include "dialog/CMessageBox.h"

namespace galaxy
{

const int FONT_ID = 0;

EventSendDialog::~EventSendDialog()
{
}

CMessageBoxGalaxy::CMessageBoxGalaxy(const int sprVar,
                                     const std::string& Text,
                                     CEvent *closeEv,
                                     const bool isModal,
                                     const CMessageBoxGalaxy::Alignment alignment,
                                     const int timeout) :
mMustClose(false),
mText(Text),
mCloseEv(closeEv),
mSprVar(sprVar),
mIsModal(isModal),
mTimeout(timeout)
{
	auto &Font = gGraphics.getFontLegacy(FONT_ID);

	mTextHeight = Font.getPixelTextHeight()*calcNumLines(mText);
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    // Create a surface for that
    if(alignment == Alignment::UPPERRIGHT)
    {
        mMBRect.w = Font.calcPixelTextWidth(mText)+16*2;
        mMBRect.h = Font.getPixelTextHeight()*(calcNumLines(mText)+2)+16;
        mMBRect.x = (gameRes.dim.x-mMBRect.w)+8;
        mMBRect.y = 0;
    }
    else
    {
        mMBRect.w = Font.calcPixelTextWidth(mText)+16*2;
        mMBRect.h = Font.getPixelTextHeight()*(calcNumLines(mText)+2)+16;
        mMBRect.x = (gameRes.dim.x-mMBRect.w)/2;
        mMBRect.y = (gameRes.dim.y-mMBRect.h)/2;
    }
}

CMessageBoxGalaxy::~CMessageBoxGalaxy()
{}

void CMessageBoxGalaxy::init()
{
    mMBSurface.createRGBSurface(mMBRect);
    mMBSurface.makeBlitCompatible();

	initGalaxyFrame();

	SDL_Rect rect = mMBRect;
    rect.x = 16;
    rect.y = 15;
    rect.w -= 16;
	rect.h -= 16;

	initText(rect);
}

bool CMessageBoxGalaxy::initWithBgBitmapInternal(const unsigned char *data,
                                                 const std::string &name,
                                                 const int size)
{
    mMBSurface.createRGBSurface(mMBRect);
    mMBSurface.makeBlitCompatible();

    const auto bmpFileLoaded = initBitmapFrameInternal(data,
                                                       name,
                                                       size);

    if(!bmpFileLoaded)
    {
        gLogging.ftextOut(FONTCOLORS::PURPLE,
                          "Could not load internal picture %s for msgbox<br>",
                          name.c_str());
    }

    SDL_Rect rect = mMBRect;
    rect.x = 72;
    rect.y = 64;
    rect.w *= 4;
    rect.h *= 4;

    initText(rect);

    {
        const auto ok = mMBTexture.loadFromSurface(mMBSurface,
                                                   gVideoDriver.Renderer());

        if(!ok) return false;

        int h,w;

        SDL_QueryTexture(mMBTexture.getPtr(),
                         nullptr, nullptr,
                         &w, &h);

        // I want this msgbox to be shown in the upper right corner as a texture
        mTextureRect.dim.x = 0.4f;
        mTextureRect.dim.y = (mTextureRect.dim.x*h)/w;
        mTextureRect.pos.x = 1.0f - mTextureRect.dim.x;
        mTextureRect.pos.y = 0.0f;

        mMBSurface.tryToDestroy();
    }

    return true;
}

bool CMessageBoxGalaxy::initWithBgBitmap(const std::string &filename)
{
    mMBSurface.createRGBSurface(mMBRect);
    mMBSurface.makeBlitCompatible();

    const auto bmpFileLoaded = initBitmapFrame(filename);

    if(!bmpFileLoaded)
    {
        gLogging.ftextOut(FONTCOLORS::PURPLE,
                         "Could not load picture file %s for msgbox<br>",
                         filename.c_str());
    }

    SDL_Rect rect = mMBRect;
    rect.x = 72;
    rect.y = 64;
    rect.w *= 4;
    rect.h *= 4;

    initText(rect);

    {
        const auto ok = mMBTexture.loadFromSurface(mMBSurface,
                                                   gVideoDriver.Renderer());

        if(!ok) return false;

        int h,w;

        SDL_QueryTexture(mMBTexture.getPtr(),
                         nullptr, nullptr,
                         &w, &h);

        // I want this msgbox to be shown in the upper right corner as a texture
        mTextureRect.dim.x = 0.4f;
        mTextureRect.dim.y = (mTextureRect.dim.x*h)/w;
        mTextureRect.pos.x = 1.0f - mTextureRect.dim.x;
        mTextureRect.pos.y = 0.0f;

        mMBSurface.tryToDestroy();
    }

    return true;
}

void CMessageBoxGalaxy::initGalaxyFrame()
{
    SDL_Surface *dst = mMBSurface.getSDLSurface();

	// first draw a blank rect
	SDL_Rect rect;
	rect.x = 4;
	rect.y = 4;
    rect.w = mMBRect.w-16;
	rect.h = mMBRect.h-16;
	SDL_FillRect(dst, &rect, 0xFFFFFFFF);

	// Draw the borders
	rect = mMBRect;
	rect.x += 8;
	rect.y += 8;
    rect.w -= 16;
	rect.h -= 16;

	GsTilemap &Tilemap = gGraphics.getTileMap(3);

	/// Now draw the borders
	// Upper Left corner
	Tilemap.drawTile(dst, 0, 0, 0);

	// Upper border
    for(int x=8 ; x<rect.w ; x+=8)
		Tilemap.drawTile(dst, x, 0, 1);

	// Upper Right corner
    Tilemap.drawTile(dst, rect.w, 0, 2);

	// Left border
	for(int y=8 ; y<rect.h ; y+=8)
		Tilemap.drawTile(dst, 0, y, 3);

	// Right border
	for(int y=8 ; y<rect.h ; y+=8)
        Tilemap.drawTile(dst, rect.w, y, 5);

	// Lower Left corner
	Tilemap.drawTile(dst, 0, rect.h, 6);

	// Lower border
    for(int x=8 ; x<rect.w ; x+=8)
		Tilemap.drawTile(dst, x, rect.h, 7);

	// Lower Right corner
    Tilemap.drawTile(dst, rect.w, rect.h, 8);
}

bool CMessageBoxGalaxy::initBitmapFrame(const std::string &filename)
{
    if(mMBSurface.loadImg(GetFullFileName(filename)))
    {       
        mMBSurface.makeBlitCompatible();        
        return true;
    }

    return false;
}

bool CMessageBoxGalaxy::initBitmapFrameInternal(const unsigned char *data,
                                                const std::string &name,
                                                const int size)
{
    if(mMBSurface.loadImgInternal(data, name, size))
    {
        mMBSurface.makeBlitCompatible();
        return true;
    }

    return false;
}


void CMessageBoxGalaxy::initText(const SDL_Rect &rect)
{
	auto &Font = gGraphics.getFontLegacy(FONT_ID);

    GsSurface textSfc;
    GsWeakSurface mbSurface(mMBSurface.getSDLSurface());

    Font.createTextSurface(textSfc, mText, 0, 0, 0 );

    textSfc.blitTo(mbSurface, rect);
}

void CMessageBoxGalaxy::ponder(const int deltaT)
{
    if(isModal())
    {
        // Look, if somebody pressed a button, and close this dialog!
        if( gInput.getPressedAnyCommand() )
        {
            std::shared_ptr<CEvent> ev(std::move(mCloseEv));
            gEventManager.add( ev );
            mMustClose = true;
            gInput.flushCommands();
            return;
        }
    }
    else
    {
        if(mTimeout)
        {
            mTimeout -= deltaT;

            if(mTimeout <= 0)
            {
                mTimeout = 0;

                std::shared_ptr<CEvent> ev(std::move(mCloseEv));
                gEventManager.add( ev );
                mMustClose = true;
            }

            return;
        }
    }
}

void CMessageBoxGalaxy::render()
{
    if(mMBTexture) // Render message using a texture
    {
        gVideoDriver.addTextureRefToRender(mMBTexture, mTextureRect);
    }
    else if(mMBSurface) // Just render the MessageBox surface based
    {
        BlitSurface(mMBSurface.getSDLSurface(), nullptr,
                    gVideoDriver.getBlitSurface(), &mMBRect);

    }
}


void showMsg(const int sprVar, const std::string &text,
             CEvent *closeEv, const bool isModal,
             const std::string bmpFilename,
             const CMessageBoxGalaxy::Alignment alignment,
             const int timeout, const bool trophy)
{
    CMessageBoxGalaxy *msgBox = nullptr;

    if(isModal)
    {
        msgBox = new CMessageBoxGalaxy(sprVar, text, closeEv);
    }
    else
    {
        msgBox = new CMessageBoxGalaxy(sprVar, text, nullptr,
                                       false, alignment,timeout);
    }

    if(!bmpFilename.empty())
    {
        if(trophy)
        {
            if(!msgBox->initWithBgBitmapInternal(gTrophyMsg,
                                                 "TrophyMsg",
                                                  sizeof(gTrophyMsg)))
                msgBox->init();
        }
        else
        {
            if(!msgBox->initWithBgBitmapInternal(gBgMsg,
                                                 "BgMsg",
                                                  sizeof(gBgMsg)))
                msgBox->init();
        }
    }
    else
    {
        msgBox->init();
    }

    gEventManager.add( new EventSendDialog( msgBox ) );
}


void showMsgVec( std::vector<CMessageBoxGalaxy*> &msgs )
{
    assert(msgs.size() > 0);

    for( CMessageBoxGalaxy* msg : msgs )
        msg->init();

    const unsigned msgSize = msgs.size();
    for( size_t i=0 ; i<msgSize-1 ; i++ )
    {
        std::unique_ptr<CEvent> ev(new EventSendDialog(msgs[i+1]));
        msgs[i]->setCloseEvent(ev);
    }
    gEventManager.add( new EventSendDialog( msgs[0] ) );
}



}




