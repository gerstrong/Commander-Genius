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
#include "CGUIInputText.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "sdl/extensions.h"
#include "graphics/CGfxEngine.h"
#include "common/CBehaviorEngine.h"

const unsigned int MAX_ELEMENTS_PER_PAGE = 7;

CGUIDialog::CGUIDialog(const CRect<float> &SrcRect) :
mRect(SrcRect),
mSelection(0)
{
	if( g_pBehaviorEngine->getEngine() == ENGINE_VORTICON )
	{
		const SDL_Rect lRect = g_pVideoDriver->toBlitRect(mRect);
		mpBackgroundSfc = CG_CreateRGBSurface( lRect );
		mpBackgroundSfc = SDL_DisplayFormatAlpha( mpBackgroundSfc.get() );
		initVorticonBackground( lRect );
	}
	else if( g_pBehaviorEngine->getEngine() == ENGINE_GALAXY )
	{
		const SDL_Rect lRect = g_pVideoDriver->getGameResolution().SDLRect();
		mpBackgroundSfc = CG_CreateRGBSurface( lRect );
		mpBackgroundSfc = SDL_DisplayFormatAlpha( mpBackgroundSfc.get() );
		initGalaxyBackround( lRect );
	}
	else
	{
		const SDL_Rect lRect = g_pVideoDriver->toBlitRect(mRect);
		mpBackgroundSfc = CG_CreateRGBSurface( lRect );
		mpBackgroundSfc = SDL_DisplayFormatAlpha( mpBackgroundSfc.get() );
		initEmptyBackround();
	}
}

CGUIDialog::~CGUIDialog()
{
	CVideoDriver *pVideoDriver = CVideoDriver::GetNoPtrChk();
	if( pVideoDriver ) // This checks if the video object still exists, because if
	{								   // deleting drawing routine, it is not needed anymore
		pVideoDriver->clearDrawingTasks();
	}
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

			// Ensures that disabled items are skipped
			std::list< SmartPointer<CGUIControl> >::iterator it = mControlList.begin();
			for( int i=0 ; it != mControlList.end() ; it++, i++ )
			{
				if( i ==  mSelection )
					break;
			}

			for( ; it != mControlList.end() ; it++ )
			{
				if( (*it)->mEnabled )
					break;

				mSelection++;
			}

			if( mSelection >= static_cast<int>(mControlList.size()) )
				mSelection = 0;


		}
		else if(ev->mCommand == IC_UP)
		{
			mSelection--;

			if( mSelection < 0 )
				mSelection = mControlList.size()-1;

			// Ensures that disabled items are skipped
			std::list< SmartPointer<CGUIControl> >::iterator it = mControlList.begin();
			for( int i=0 ; it != mControlList.end() ; it++, i++ )
			{
				if( i ==  mSelection )
					break;
			}

			for( ; it != mControlList.end() ; it++ )
			{
				if( (*it)->mEnabled )
					break;

				mSelection--;
			}
		}




		// Send all the other events the active control element
		std::list< SmartPointer<CGUIControl> >::iterator it = mControlList.begin();
		for( int i=0 ; it != mControlList.end() ; it++, i++ )
		{
			(*it)->setHovered( (i == mSelection) );

			if(i == mSelection)
			{
				(*it)->sendEvent(ev->mCommand);
			}

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


void CGUIDialog::setRect(const CRect<float> &rect)
{
	mRect = rect;
}


void CGUIDialog::setPosition(const float x, const float y)
{
	mRect.x = x;
	mRect.y = y;
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

		if( dynamic_cast<CGUIButton*>(ctrl) || dynamic_cast<CGUIInputText*>(ctrl) )
		{
			if( ctrl->getHovered() )
			{
				mpCurrentCtrl = ctrl;
				mSelection = sel;
			}

		}

	}

	if(!g_pInput->m_EventList.empty())
		g_pInput->m_EventList.clear();

}


void CGUIDialog::initEmptyBackround()
{
	SDL_FillRect( mpBackgroundSfc.get(), NULL, 0xFFE6E6E6 );
}

void CGUIDialog::initVorticonBackground( SDL_Rect Rect )
{
	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(1);

	SDL_Surface *backSfc = mpBackgroundSfc.get();


	// Draw the character so the classical vorticon menu is drawn

	// Start with the blank space (normally it's white. Might be different in some mods)
	for( int x=8 ; x<Rect.w-8 ; x+=8 )
	{
		for( int y=8 ; y<Rect.h-2*8 ; y+=8 )
		{
			Font.drawCharacter( backSfc, 32, x, y );
		}
	}


	// Now draw the borders
	Font.drawCharacter( backSfc, 1, 0, 0 );

	for( int x=8 ; x<Rect.w-8 ; x+=8 )
	{
		Font.drawCharacter( backSfc, 2, x, 0 );
	}

	Font.drawCharacter( backSfc, 3, Rect.w-8, 0 );

	for( int x=8 ; x<Rect.w-8 ; x+=8 )
	{
		Font.drawCharacter( backSfc, 7, x, Rect.h-2*8 );
	}

	for( int y=8 ; y<Rect.h-16 ; y+=8 )
	{
		Font.drawCharacter( backSfc, 4, 0, y );
	}

	for( int y=8 ; y<Rect.h-16 ; y+=8 )
	{
		Font.drawCharacter( backSfc, 5, Rect.w-8, y );
	}

	Font.drawCharacter( backSfc, 6, 0, Rect.h-2*8 );
	Font.drawCharacter( backSfc, 8, Rect.w-8, Rect.h-2*8 );

}


void CGUIDialog::initGalaxyBackround(SDL_Rect Rect)
{
	// Besides the Background Bitmap we need to draw two scores. One is underline the other upper line
	SDL_Surface *backSfc = mpBackgroundSfc.get();

	g_pGfxEngine->getBitmap("KEENSWATCH")->_draw(backSfc, 0, 0);

	Uint32 color = SDL_MapRGB( backSfc->format, 84, 234, 84);
	SDL_Rect scoreRect;
	scoreRect.w = 150;
	scoreRect.h = 1;
	scoreRect.x = 80;
	scoreRect.y = 55;

	SDL_FillRect(backSfc, &scoreRect, color);

	scoreRect.y = 143;

	SDL_FillRect(backSfc, &scoreRect, color);

}


void CGUIDialog::processRendering()
{
	SDL_Rect lRect = g_pVideoDriver->toBlitRect(mRect);
	CRect<Uint16> GameRes = g_pVideoDriver->getGameResolution();
	CRect<float> screenRect(0, 0, GameRes.w, GameRes.h);

	if( g_pBehaviorEngine->getEngine() == ENGINE_GALAXY )
	{
		SDL_BlitSurface( mpBackgroundSfc.get(), NULL, g_pVideoDriver->getBlitSurface(), NULL );
	}
	else
	{
		SDL_BlitSurface( mpBackgroundSfc.get(), NULL, g_pVideoDriver->getBlitSurface(), &lRect );
	}

	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = mControlList.begin() ;
		 it != mControlList.end() ; it++ )
	{
		(*it)->processRender(screenRect);
	}

}
