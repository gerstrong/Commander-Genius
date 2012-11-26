/*
 * CMessageBoxSelection.cpp
 *
 *  Created on: 12.06.2011
 *      Author: gerstrong
 */

#include "CMessageBoxSelection.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "graphics/CGfxEngine.h"
#include "common/CBehaviorEngine.h"
#include "StringUtils.h"

const int FONT_ID = 0;

CMessageBoxSelection::CMessageBoxSelection( const std::string& Text, const std::list<TextEventMatchOption> &Options ) :
CMessageBoxGalaxy(Text),
m_Options(Options),
m_selection(0)
{
	mText += "\n";
	// Center that dialog box	
	for( auto &op : m_Options )
	{
	    mText += "\n";
	    mText += op.text;	    
	}
	
	mText += "\n\n";
	
	CFont &Font = g_pGfxEngine->getFont(FONT_ID);

	mTextHeight = Font.getPixelTextHeight()*calcNumLines(mText);

	// Create a surface for that
	mMBRect.w = Font.getPixelTextWidth(mText)+16;
	mMBRect.h = Font.getPixelTextHeight()*(calcNumLines(mText)+1)+16;
	mMBRect.x = (320-mMBRect.w)/2;
	mMBRect.y = (200-mMBRect.h)/2;	
}


void CMessageBoxSelection::init()
{
       	mpMBSurface.reset(CG_CreateRGBSurface( mMBRect ), &SDL_FreeSurface);
	mpMBSurface.reset(SDL_DisplayFormatAlpha( mpMBSurface.get() ), &SDL_FreeSurface);
    
	initGalaxyFrame();

	SDL_Rect rect = mMBRect;
	rect.x = 8;
	rect.y = 10;
	rect.w -= 16;
	rect.h -= 16;
    
	CFont &Font = g_pGfxEngine->getFont(FONT_ID);

	SDL_PixelFormat *format = g_pVideoDriver->getBlitSurface()->format;
	
	SDL_Surface *pColoredTextSurface = CG_CreateRGBSurface(rect);

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
		temp = SDL_DisplayFormatAlpha(pColoredTextSurface);
	else // or
		temp = SDL_DisplayFormat(pColoredTextSurface);

	SDL_FreeSurface(pColoredTextSurface);
	pColoredTextSurface = temp;

	Font.setupColor( oldColor );

	std::unique_ptr<SDL_Surface,SDL_Surface_Deleter> pTextSfc(pColoredTextSurface);			
	SDL_BlitSurface(pTextSfc.get(), NULL, mpMBSurface.get(), const_cast<SDL_Rect*>(&rect));
	
	
	SDL_Rect selRect;
	
	selRect.x = selRect.y = 0;
	selRect.w = selRect.h = 10;
	
    	mpSelSurface.reset(CG_CreateRGBSurface( selRect ), &SDL_FreeSurface);
	mpSelSurface.reset(SDL_DisplayFormatAlpha( mpSelSurface.get() ), &SDL_FreeSurface);
	
	// TODO: Create the Border and with two Surfaces of different colors create the rectangle
	SDL_FillRect( mpSelSurface.get(), &selRect, SDL_MapRGB( format, 0, 0, 0 ) );	
}


void CMessageBoxSelection::process()
{
	// Look, if somebody pressed a button, and close this dialog!
	if(g_pInput->getPressedCommand(IC_JUMP))
	{
		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

		for( int c=0 ; c<m_selection ; c++ )
			m_Options.pop_front();

		EventContainer.add( m_Options.front().event );

		mMustClose = true;
		g_pInput->flushCommands();
		return;
	}
	else if(g_pInput->getPressedCommand(IC_DOWN))
	{
		if(m_selection >= ((int)m_Options.size()-1) )
			m_selection = 0;
		else
			m_selection++;
	}
	else if(g_pInput->getPressedCommand(IC_UP))
	{
		if(m_selection <= 0 )
			m_selection = m_Options.size()-1;
		else
			m_selection--;
	}

	g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mpMBSurface, NULL, &mMBRect ) );

	SDL_Rect cursorSel;
	
	cursorSel.w = cursorSel.h = 10;
	cursorSel.x = mMBRect.x + 8;
	
	auto it = m_Options.begin();
	for( int i=0 ; it != m_Options.end() ; it++, i++)
	{
		if(i == m_selection)
		{
		    cursorSel.y = mMBRect.y + 11*i + 46;
		    g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mpSelSurface, NULL, &cursorSel ) );
		}
	}	
}

