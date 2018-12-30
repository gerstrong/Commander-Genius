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

const int BLEND_SPEED = 10;

CMessageBoxSelection::CMessageBoxSelection( const std::string& Text,
                                            const std::list<TextEventMatchOption> &Options) :
CMessageBoxGalaxy(0, Text, nullptr),
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
	mMBRect.w = Font.calcPixelTextWidth(mText)+16;
	mMBRect.h = Font.getPixelTextHeight()*(calcNumLines(mText)+1)+16;
    mMBRect.x = (gameRes.w-mMBRect.w)/2;
    mMBRect.y = (gameRes.h-mMBRect.h)/2;
}


void CMessageBoxSelection::init()
{
    GsWeakSurface weakBlit(gVideoDriver.getBlitSurface());

    mMBSurface.createRGBSurface(mMBRect);
    mMBSurface.makeBlitCompatible();

	initGalaxyFrame();

	SDL_Rect rect = mMBRect;
	rect.x = 8;
	rect.y = 10;
	rect.w -= 16;
	rect.h -= 16;
    
	GsFont &Font = gGraphics.getFont(FONT_ID);

    GsSurface coloredTextSurface;
    coloredTextSurface.createRGBSurface(rect);

	const Uint32 oldColor = Font.getFGColor();

    Font.setupColor( weakBlit.mapRGB(0,0,0) );

	auto textList = explode(mText, "\n");
	
	int yoff = 0;
	for( auto &it : textList  )
	{	    
	    int xmid = (rect.w-Font.calcPixelTextWidth(it))/2+rect.x;
        Font.drawFont( coloredTextSurface, it, xmid, yoff, false);
	    yoff += 12;
	}	

    coloredTextSurface.makeBlitCompatible();

	Font.setupColor( oldColor );

    coloredTextSurface.blitTo(mMBSurface, rect);
	
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

    mSelSurface1.createRGBSurface(selRect);
    mSelSurface2.createRGBSurface(selRect);


    mSelSurface1.fillRGB(255, 0,   0);
    mSelSurface2.fillRGB(  0, 0, 255);

    mSelSurface1.setColorKey( mSelSurface1.mapRGB(0, 0, 0) );
    mSelSurface2.setColorKey( mSelSurface2.mapRGB(0, 0, 0) );

    mSelSurface1.setBlendMode(1);
    mSelSurface2.setBlendMode(1);

    mSelSurface1.fillRGB(cutRect, 0, 0, 0);
    mSelSurface2.fillRGB(cutRect, 0, 0, 0);
}


void CMessageBoxSelection::ponder()
{
	// Look, if somebody pressed a button, and close this dialog!
	if(gInput.getPressedCommand(IC_JUMP) || gInput.getPressedKey(KENTER) )
	{
		for( int c=0 ; c<m_selection ; c++ )
        {
			m_Options.pop_front();
        }

        gEventManager.add( m_Options.front().event );

		mMustClose = true;
		gInput.flushCommands();
		return;
	}
	else if(gInput.getPressedCommand(IC_DOWN))
	{
		if(m_selection >= ((int)m_Options.size()-1) )
        {
            m_selection = 0;
        }
		else
        {
			m_selection++;
        }
	}
	else if(gInput.getPressedCommand(IC_UP))
	{
		if(m_selection <= 0 )
        {
			m_selection = m_Options.size()-1;
        }
		else
        {
			m_selection--;
        }
	}

    // Smooth animation of the rectangle
    if(mSmoothCursor < 12*m_selection)
    {
        mSmoothCursor++;
    }
    else if(mSmoothCursor > 12*m_selection)
    {
        mSmoothCursor--;
    }


    // now draw the glowing rectangle. It fades here!
    if(blendup)
    {
        blend += BLEND_SPEED;
    }
    else
    {
        blend -= BLEND_SPEED;
    }

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
}

void CMessageBoxSelection::render()
{
    GsWeakSurface weakBlit(gVideoDriver.getBlitSurface());

    mMBSurface.blitTo(weakBlit, mMBRect);

    mSelSurface1.setAlpha(static_cast<unsigned char>(blend));
    mSelSurface2.setAlpha(static_cast<unsigned char>(SDL_ALPHA_OPAQUE-blend));

    mSelSurface1.applyDisplayFormat();
    mSelSurface2.applyDisplayFormat();
    mSelSurface1.setColorKey(0,0,0);
    mSelSurface2.setColorKey(0,0,0);

    SDL_Rect cursorSel;

    cursorSel.w = cursorSel.h = 10;
    cursorSel.x = mMBRect.x + 8;

    auto it = m_Options.begin();
    for( int i=0 ; it != m_Options.end() ; it++, i++)
    {
        if(i == m_selection)
        {
            cursorSel.y = mMBRect.y + mSmoothCursor + 44;
            mSelSurface1.blitTo(weakBlit, cursorSel);
            mSelSurface2.blitTo(weakBlit, cursorSel);
        }
    }
}

}
