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
#include "graphics/GsGraphics.h"
#include "sdl/extensions.h"
#include <base/utils/StringUtils.h>
#include <memory>

#include "dialog/CMessageBox.h"

namespace galaxy
{

const int FONT_ID = 0;

CMessageBoxGalaxy::CMessageBoxGalaxy(const std::string& Text, CEvent *closeEv) :
mMustClose(false),
mText(Text),
mCloseEv(closeEv)
{
	GsFont &Font = gGraphics.getFont(FONT_ID);

	mTextHeight = Font.getPixelTextHeight()*calcNumLines(mText);
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

	// Create a surface for that
	mMBRect.w = Font.getPixelTextWidth(mText)+16;
	mMBRect.h = Font.getPixelTextHeight()*(calcNumLines(mText)+1)+16;
    mMBRect.x = (gameRes.w-mMBRect.w)/2;
    mMBRect.y = (gameRes.h-mMBRect.h)/2;
}

void CMessageBoxGalaxy::init()
{    
    mpMBSurface.reset(CG_CreateRGBSurface( mMBRect ), &SDL_FreeSurface);
    mpMBSurface.reset(gVideoDriver.convertThroughBlitSfc( mpMBSurface.get() ), &SDL_FreeSurface);

	initGalaxyFrame();

	SDL_Rect rect = mMBRect;
	rect.x = 8;
	rect.y = 8;
	rect.w -= 16;
	rect.h -= 16;

	initText(rect);
}

void CMessageBoxGalaxy::initGalaxyFrame()
{
	SDL_Surface *dst = mpMBSurface.get();

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

void CMessageBoxGalaxy::initText(const SDL_Rect &rect)
{
	GsFont &Font = gGraphics.getFont(FONT_ID);

    GsSurface textSfc;
    GsWeakSurface mbSurface(mpMBSurface.get());

    Font.createTextSurface(textSfc, mText, 0, 0, 0 );

    textSfc.blitTo(mbSurface, rect);
}

void CMessageBoxGalaxy::ponder()
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

void CMessageBoxGalaxy::render()
{
    // Just render the MessageBox
    BlitSurface(mpMBSurface.get(), nullptr, gVideoDriver.getBlitSurface(), &mMBRect);
}


void showMsg( const std::string &text, CEvent *closeEv )
{
    CMessageBoxGalaxy *msgBox = new CMessageBoxGalaxy(text, closeEv);
    msgBox->init();
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



