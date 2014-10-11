/*
 * CMessageBoxSelection.cpp
 *
 *  Created on: 12.06.2011
 *      Author: gerstrong
 */

#include "CMessageBoxSelection.h"
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include "graphics/GsGraphics.h"
#include "engine/core/CBehaviorEngine.h"
#include <base/utils/StringUtils.h>

namespace galaxy
{

const int FONT_ID = 0;

const int BLEND_SPEED = 15;

CMessageBoxSelection::CMessageBoxSelection( const std::string& Text,
                                            const std::list<TextEventMatchOption> &Options) :
CMessageBoxGalaxy(Text, nullptr),
m_Options(Options),
m_selection(0),
blend(0),
blendup(true)
{
	mText += "\n";
	// Center that dialog box	
	for( auto &op : m_Options )
	{
	    mText += "\n";
	    mText += op.text;	    
	}
	
	mText += "\n\n";
	
	GsFont &Font = gGraphics.getFont(FONT_ID);

	mTextHeight = Font.getPixelTextHeight()*calcNumLines(mText);

	// Create a surface for that
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
	mMBRect.w = Font.getPixelTextWidth(mText)+16;
	mMBRect.h = Font.getPixelTextHeight()*(calcNumLines(mText)+1)+16;
    mMBRect.x = (gameRes.w-mMBRect.w)/2;
    mMBRect.y = (gameRes.h-mMBRect.h)/2;
}


void CMessageBoxSelection::init()
{
    auto *blit = gVideoDriver.getBlitSurface();
    SDL_PixelFormat *format = blit->format;

    mpMBSurface.reset( SDL_CreateRGBSurface( SDL_SWSURFACE,
                mMBRect.w,
                mMBRect.h,
                RES_BPP,
                format->Rmask,
                format->Gmask,
                format->Bmask,
                format->Amask ), &SDL_FreeSurface );


    mpMBSurface.reset(gVideoDriver.convertThroughBlitSfc( mpMBSurface.get() ), &SDL_FreeSurface);

	initGalaxyFrame();

	SDL_Rect rect = mMBRect;
	rect.x = 8;
	rect.y = 10;
	rect.w -= 16;
	rect.h -= 16;
    
	GsFont &Font = gGraphics.getFont(FONT_ID);

    SDL_Surface *pColoredTextSurface = SDL_CreateRGBSurface( SDL_SWSURFACE,
                                                             rect.w,
                                                             rect.h,
                                                             RES_BPP,
                                                             format->Rmask,
                                                             format->Gmask,
                                                             format->Bmask,
                                                             format->Amask );


	const Uint32 oldColor = Font.getFGColor();

	Font.setupColor( SDL_MapRGB( format, 0, 0, 0 ) );

	auto textList = explode(mText, "\n");
	
	int yoff = 0;
	for( auto &it : textList  )
	{	    
	    int xmid = (rect.w-Font.getPixelTextWidth(it))/2+rect.x;
	    Font.drawFont( pColoredTextSurface, it, xmid, yoff);
	    yoff += 12;
	}	

	// Adapt the newly created surface to the running screen.
	SDL_Surface *temp;


    if(RES_BPP == 32) // Only if there is an Alpha Channel (32 BPP)
        temp = gVideoDriver.convertThroughBlitSfc(pColoredTextSurface);
	else // or
        temp = gVideoDriver.convertThroughBlitSfc(pColoredTextSurface);

	SDL_FreeSurface(pColoredTextSurface);
	pColoredTextSurface = temp;

	Font.setupColor( oldColor );

	std::unique_ptr<SDL_Surface,SDL_Surface_Deleter> pTextSfc(pColoredTextSurface);			
	BlitSurface(pTextSfc.get(), NULL, mpMBSurface.get(), const_cast<SDL_Rect*>(&rect));
	
	
	// Create the Border and with two Surfaces of different colors create the rectangle
	SDL_Rect selRect;
	SDL_Rect cutRect;
	selRect.x = selRect.y = 0;
	selRect.w = rect.w-rect.x;
	selRect.h = 14;
	cutRect = selRect;
	cutRect.x += 2;
	cutRect.y += 2;
	cutRect.w -= 4;
	cutRect.h -= 4;
		
    mpSelSurface1.reset( SDL_CreateRGBSurface( SDL_SWSURFACE,
                                               selRect.w,
                                               selRect.h,
                                               RES_BPP,
                                               format->Rmask,
                                               format->Gmask,
                                               format->Bmask,
                                               0 ),
                         &SDL_FreeSurface);

    mpSelSurface2.reset( SDL_CreateRGBSurface( SDL_SWSURFACE,
                                               selRect.w,
                                               selRect.h,
                                               RES_BPP,
                                               format->Rmask,
                                               format->Gmask,
                                               format->Bmask,
                                               0 ),
                         &SDL_FreeSurface);

	SDL_FillRect( mpSelSurface1.get(), &selRect, SDL_MapRGB( format, 255, 0, 0 ) );
	SDL_FillRect( mpSelSurface2.get(), &selRect, SDL_MapRGB( format, 0, 0, 255 ) );
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetColorKey(mpSelSurface1.get(), SDL_TRUE, SDL_MapRGB( format, 0, 0, 0 ));
    SDL_SetColorKey(mpSelSurface2.get(), SDL_TRUE, SDL_MapRGB( format, 0, 0, 0 ));    
    SDL_SetSurfaceBlendMode(mpSelSurface1.get(), SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceBlendMode(mpSelSurface2.get(), SDL_BLENDMODE_BLEND);
#else
    SDL_SetColorKey( mpSelSurface1.get(), SDL_SRCCOLORKEY, SDL_MapRGB( format, 0, 0, 0 ) );
    SDL_SetColorKey( mpSelSurface2.get(), SDL_SRCCOLORKEY, SDL_MapRGB( format, 0, 0, 0 ) );
#endif
	SDL_FillRect( mpSelSurface1.get(), &cutRect, SDL_MapRGB( format, 0, 0, 0 ) );
	SDL_FillRect( mpSelSurface2.get(), &cutRect, SDL_MapRGB( format, 0, 0, 0 ) );
}


void CMessageBoxSelection::ponder()
{
	// Look, if somebody pressed a button, and close this dialog!
	if(gInput.getPressedCommand(IC_JUMP) || gInput.getPressedKey(KENTER) )
	{
		for( int c=0 ; c<m_selection ; c++ )
			m_Options.pop_front();

        gEventManager.add( m_Options.front().event );

		mMustClose = true;
		gInput.flushCommands();
		return;
	}
	else if(gInput.getPressedCommand(IC_DOWN))
	{
		if(m_selection >= ((int)m_Options.size()-1) )
            m_selection = 0;
		else
			m_selection++;
	}
	else if(gInput.getPressedCommand(IC_UP))
	{
		if(m_selection <= 0 )
			m_selection = m_Options.size()-1;
		else
			m_selection--;
	}
}

void CMessageBoxSelection::render()
{
    BlitSurface(mpMBSurface.get(), nullptr, gVideoDriver.getBlitSurface(), &mMBRect);

    // now draw the glowing rectangle. It fades here!
    if(blendup)
        blend+= BLEND_SPEED;
    else
        blend-= BLEND_SPEED;

    if(blend <= SDL_ALPHA_TRANSPARENT)
    {
        blendup = true;
        blend = SDL_ALPHA_TRANSPARENT;
    }

    if(blend >= SDL_ALPHA_OPAQUE)
    {
        blendup = false;
        blend = SDL_ALPHA_OPAQUE;
    }

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod( mpSelSurface1.get(), blend );
    SDL_SetSurfaceAlphaMod( mpSelSurface2.get(), SDL_ALPHA_OPAQUE-blend );
#else
    SDL_SetAlpha( mpSelSurface1.get(), SDL_SRCALPHA, blend );
    SDL_SetAlpha( mpSelSurface2.get(), SDL_SRCALPHA, SDL_ALPHA_OPAQUE-blend );
#endif

    SDL_Rect cursorSel;

    cursorSel.w = cursorSel.h = 10;
    cursorSel.x = mMBRect.x + 8;

    auto it = m_Options.begin();
    for( int i=0 ; it != m_Options.end() ; it++, i++)
    {
        if(i == m_selection)
        {
            cursorSel.y = mMBRect.y + 12*i + 44;
            BlitSurface(mpSelSurface1.get(), nullptr, gVideoDriver.getBlitSurface(), &cursorSel);
            BlitSurface(mpSelSurface2.get(), nullptr, gVideoDriver.getBlitSurface(), &cursorSel);
        }
    }
}

}
