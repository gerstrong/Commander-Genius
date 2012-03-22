/*
 * CGUIDialog.cpp
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *
 *  GUI Class for a Dialog which is able to carry other controls
 */

#include "CGUIDialog.h"
#include "CGUIButton.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "sdl/extensions.h"
#include "graphics/CGfxEngine.h"

CGUIDialog::CGUIDialog(const CRect<float> &SrcRect) :
mRect(SrcRect),
mSelection(0)
{
	setBackground(NONE);
}


void CGUIDialog::setBackground(const Background background)
{
	//SDL_Rect lRect = RectDispCoord.SDLRect();
	const SDL_Rect lRect = mRect.SDLRect();
	mpBackgroundSfc = CG_CreateRGBSurface( lRect );


	if( background == VORTICON )
		drawBackround = &CGUIDialog::drawVorticonBackround;
	else
		drawBackround = &CGUIDialog::drawEmptyBackround;

}






void CGUIDialog::addControl( const SmartPointer<CGUIControl> newControl,
							 const CRect<float>& RelRect )
{
	CRect<float> AbsRect = RelRect;
	AbsRect.transform(mRect);
	newControl->mRect = AbsRect;
	mControlList.push_back( newControl );
}




void CGUIDialog::addControl( const SmartPointer<CGUIControl> newControl )
{
	mControlList.push_back( newControl );
	fit();
}



void CGUIDialog::sendEvent( const SmartPointer<CEvent> &command )
{
	if( CommandEvent *ev = dynamic_cast<CommandEvent*>(command.get()) )
	{
		if(ev->mCommand == IC_DOWN)
		{
			mSelection++;
		}
		else if(ev->mCommand == IC_UP)
		{
			mSelection--;
		}

		if( mSelection >= static_cast<int>(mControlList.size()) )
			mSelection = 0;

		if( mSelection < 0 )
			mSelection = mControlList.size()-1;

		std::list< SmartPointer<CGUIControl> >::iterator it = mControlList.begin();
		for( int i=0 ; it != mControlList.end() ; it++, i++ )
		{
			(*it)->setHovered( (i == mSelection) );

			if(i == mSelection)
				(*it)->sendEvent(ev->mCommand);

		}


	}
}



void CGUIDialog::fit()
{
	std::list< SmartPointer<CGUIControl> >::iterator it = mControlList.begin();
	it++;

	size_t numControls = mControlList.size();
	const float charHeight = ( 1.0f/(float)(numControls+2) );

	for( size_t c = 1; it != mControlList.end() ; it++, c++ )
	{
		CRect<float> rect(	0.05f,
							charHeight*(float)c,
							mRect.w,
							charHeight-0.01f );

		rect.transform(mRect);

		(*it)->setRect( rect );
	}

}




void CGUIDialog::processLogic()
{
	// Render the stuff
	g_pVideoDriver->mDrawTasks.add( new DrawGUIRenderTask(this) );

	// Prepare the subcontrols for rendering
	std::list< SmartPointer<CGUIControl> >::iterator it = mControlList.begin();
	for( int sel=0 ; it != mControlList.end() ; it++, sel++ )
	{
		CGUIControl *ctrl = (*it).get();

		ctrl->processLogic();

		if( CGUIButton *button = dynamic_cast<CGUIButton*>(ctrl) )
		{
			if( button->getHovered() )
			{
				mSelection = sel;
			}

		}

	}

	g_pInput->m_EventList.clear();
}


void CGUIDialog::drawEmptyBackround(SDL_Rect Rect)
{
	SDL_FillRect( g_pVideoDriver->getBlitSurface(), &Rect, 0x00E6E6E6 );
}


void CGUIDialog::drawVorticonBackround( SDL_Rect Rect )
{
	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(0);

	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();


	// Draw the character so the classical vorticon menu is drawn

	// Start with the blank space (normally it's white. Might be different in some mods)
	for( int x=Rect.x+8 ; x<Rect.x+Rect.w-8 ; x+=8 )
	{
		for( int y=Rect.y+8 ; y<Rect.y+Rect.h-2*8 ; y+=8 )
		{
			Font.drawCharacter( Blitsurface, 32, x, y );
		}
	}


	Font.drawCharacter( Blitsurface, 1, Rect.x, Rect.y );

	for( int x=Rect.x+8 ; x<Rect.x+Rect.w-8 ; x+=8 )
	{
		Font.drawCharacter( Blitsurface, 2, x, Rect.y );
	}

	Font.drawCharacter( Blitsurface, 3, Rect.x+Rect.w-8, Rect.y );

	for( int x=Rect.x+8 ; x<Rect.x+Rect.w-8 ; x+=8 )
	{
		Font.drawCharacter( Blitsurface, 7, x, Rect.y+Rect.h-2*8 );
	}

	for( int y=Rect.y+8 ; y<Rect.y+Rect.h-16 ; y+=8 )
	{
		Font.drawCharacter( Blitsurface, 4, Rect.x, y );
	}

	for( int y=Rect.y+8 ; y<Rect.y+Rect.h-16 ; y+=8 )
	{
		Font.drawCharacter( Blitsurface, 5, Rect.x+Rect.w-8, y );
	}

	Font.drawCharacter( Blitsurface, 6, Rect.x, Rect.y+Rect.h-2*8 );
	Font.drawCharacter( Blitsurface, 8, Rect.x+Rect.w-8, Rect.y+Rect.h-2*8 );

}


void CGUIDialog::processRendering()
{
	CRect<Uint16> GameRes = g_pVideoDriver->getGameResolution();
	CRect<float> screenRect(0, 0, GameRes.w, GameRes.h);
	CRect<float> RectDispCoordFloat = mRect;

	// Transform to the blit coordinates
	RectDispCoordFloat.transform(screenRect);

	CRect<Uint16> RectDispCoord;
	RectDispCoord = RectDispCoordFloat;
	SDL_Rect lRect = RectDispCoord.SDLRect();

	(this->*drawBackround)(lRect);

	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = mControlList.begin() ;
		 it != mControlList.end() ; it++ )
	{
		(*it)->processRender(screenRect);
	}

}
