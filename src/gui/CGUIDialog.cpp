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
const unsigned int MAX_STEPS = 20;


CGUIDialog::CGUIDialog(const CRect<float> &SrcRect, const FXState fx) :
mRect(SrcRect),
mSelection(0),
mFXSetup(fx),
mFXhStep(0),
mFXvStep(0)
{
    if( mFXSetup == EXPAND )
    {
        mFXhStep = MAX_STEPS;
        mFXvStep = MAX_STEPS-3;
    }
}


void CGUIDialog::updateBackground()
{
	if( g_pBehaviorEngine->getEngine() == ENGINE_VORTICON )
	{
        initVorticonBackground();
	}
	else if( g_pBehaviorEngine->getEngine() == ENGINE_GALAXY )
	{
        initGalaxyBackround();
	}
	else
	{
		initEmptyBackround();
	}
}

void CGUIDialog::updateGraphics()
{
    updateBackground();
    for( auto &control : mControlList )
    {
        control->updateGraphics();
    }
}



void CGUIDialog::addControl( std::unique_ptr<CGUIControl> &newControl,
							    const CRect<float>& RelRect )
{
	CRect<float> AbsRect = RelRect;
	AbsRect.transform(mRect);
	newControl->mRect = AbsRect;
	mControlList.push_back( move(newControl) );

	if(mControlList.size() == 1)
	{
	    mpCurrentCtrl = mControlList.front().get();
	}
}



void CGUIDialog::addControl( std::unique_ptr<CGUIControl> &newControl )
{
	mControlList.push_back( move(newControl) );
	fit();

	if(mControlList.size() == 1)
	{
	    mpCurrentCtrl = mControlList.front().get();
	}

}

void CGUIDialog::addControl( CGUIControl *newControl,
		 	 const CRect<float>& RelRect )
{
    std::unique_ptr<CGUIControl> ctrl(newControl);
    addControl( ctrl, RelRect );    
}

void CGUIDialog::addControl( CGUIControl *newControl )
{
    std::unique_ptr<CGUIControl> ctrl(newControl);
    addControl(ctrl);    
}



void CGUIDialog::selectPrevItem()
{
	if(mpCurrentCtrl->getHovered())
		mpCurrentCtrl->setHovered(false);

	mSelection--;

	if( mSelection < 0 )
		mSelection = mControlList.size()-1;


	auto it = mControlList.begin();
	for( int i=0 ; it != mControlList.end() ; it++, i++ )
	{
		if( i ==  mSelection )
			break;
	}

	// Ensures that disabled items are skipped
	for( ; it != mControlList.begin() ; it-- )
	{
		if( (*it)->mEnabled )
			break;

		mSelection--;
	}
    
    if( mSelection < 0 ) {
		mSelection = mControlList.size()-1;
        it = mControlList.end();
    }

	(*it)->setHovered(true);
	mpCurrentCtrl = it->get();
}


void CGUIDialog::selectNextItem()
{
	if(mpCurrentCtrl->getHovered())
		mpCurrentCtrl->setHovered(false);

	mSelection++;

	if( mSelection >= static_cast<int>(mControlList.size()) )
		mSelection = 0;

	// Find the right control!
	auto it = mControlList.begin();
	for( int i=0 ; it != mControlList.end() ; it++, i++ )
	{
		if( i == mSelection )
			break;
	}

	// Ensures that disabled items are skipped
	for( ; it != mControlList.end() ; it++ )
	{
		if( (*it)->mEnabled )
			break;

		mSelection++;
	}
    
    if( mSelection >= static_cast<int>(mControlList.size()) ) {
		mSelection = 0;
        it = mControlList.begin();
    }

	(*it)->setHovered(true);
	mpCurrentCtrl = it->get();
}


void CGUIDialog::setSelection(const unsigned int sel)
{
	const int steps = sel-mSelection;

	if(steps == 0)
		return;

	if(steps > 0)
	{
		for(int c=0 ; c<steps ; c++)
			selectNextItem();
	}
	else
	{
		for(int c=0 ; c<-steps ; c++)
			selectPrevItem();
	}
}

bool CGUIDialog::sendEvent( const std::shared_ptr<CEvent> &command )
{
	if( CommandEvent *ev = dynamic_cast<CommandEvent*>(command.get()) )
	{
		// Send all the other events the active control element
		int i=0;
        for( auto &it : mControlList )
        {            
            if( i == mSelection )
            {
                if( !it->getHovered() )
                {
                    it->setHovered( (i == mSelection) );
                }
                else
                {
                    if( it->sendEvent(ev->mCommand) )
                        return true;
                }
            }
            else
            {
                it->setHovered( false );
            }
            i++;
        }

		if(ev->mCommand == IC_DOWN)
		{
			selectNextItem();
			return true;
		}
		else if(ev->mCommand == IC_UP)
		{
			selectPrevItem();
			return true;
		}
	}

	return false;
}

void CGUIDialog::fit()
{
	auto it = mControlList.begin();
	it++;

	size_t numControls = mControlList.size();
	const float charHeight = ( 1.0f/(float)(numControls+1) );

	size_t c = 1;
	for( ; it != mControlList.end() ; it++ )
	{
		CRect<float> rect( 0.05f,
				   charHeight*((float)c),
				   mRect.w,
				   charHeight-0.01f );

		rect.transform(mRect);

		(*it)->setRect( rect );
		c++;
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



void CGUIDialog::initEmptyBackround()
{
    const SDL_Rect lRect = g_pVideoDriver->toBlitRect(mRect);
    mpBackgroundSfc.reset( CG_CreateRGBSurface( lRect ), &SDL_FreeSurface );

    mpBackgroundSfc.reset( g_pVideoDriver->convertThroughBlitSfc( mpBackgroundSfc.get() ), &SDL_FreeSurface );

	SDL_Surface *sfc = mpBackgroundSfc.get();    
    SDL_FillRect( sfc, NULL, SDL_MapRGB( sfc->format, 230, 230, 230) );        
}


void CGUIDialog::drawBorderRect(SDL_Surface *backSfc, const SDL_Rect &Rect)
{
    CFont &Font = g_pGfxEngine->getFont(1);
    Font.drawCharacter( backSfc, 1, 0, 0 );

    for( int x=8 ; x<Rect.w-8 ; x+=8 )
    {
        Font.drawCharacter( backSfc, 2, x, 0 );
    }

    Font.drawCharacter( backSfc, 3, Rect.w-8, 0 );

    for( int x=8 ; x<Rect.w-8 ; x+=8 )
    {
        Font.drawCharacter( backSfc, 7, x, Rect.h-8 );
    }

    for( int y=8 ; y<Rect.h-8 ; y+=8 )
    {
        Font.drawCharacter( backSfc, 4, 0, y );
    }

    for( int y=8 ; y<Rect.h-8 ; y+=8 )
    {
        Font.drawCharacter( backSfc, 5, Rect.w-8, y );
    }

    Font.drawCharacter( backSfc, 6, 0, Rect.h-8 );
    Font.drawCharacter( backSfc, 8, Rect.w-8, Rect.h-8 );
}


void CGUIDialog::initVorticonBackground()
{
    const SDL_Rect Rect = g_pVideoDriver->toBlitRect(mRect);
    mpBackgroundSfc.reset( CG_CreateRGBSurface( Rect ), &SDL_FreeSurface );
    mpBackgroundSfc.reset( g_pVideoDriver->convertThroughBlitSfc( mpBackgroundSfc.get() ), &SDL_FreeSurface );

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(1);

	SDL_Surface *backSfc = mpBackgroundSfc.get();


	// Draw the character so the classical vorticon menu is drawn

	// Start with the blank space (normally it's white. Might be different in some mods)
	for( int x=8 ; x<Rect.w-8 ; x+=8 )
	{
		for( int y=8 ; y<Rect.h-8 ; y+=8 )
		{
			Font.drawCharacter( backSfc, 32, x, y );
		}
	}            

	// Now draw the borders
    drawBorderRect(backSfc, Rect);

    mpTempSfc.reset( g_pVideoDriver->convertThroughBlitSfc( backSfc ), &SDL_FreeSurface );
}

void CGUIDialog::initGalaxyBackround()
{   
    mBackgroundBmp = *g_pGfxEngine->getBitmapFromStr("KEENSWATCH");

    CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();
    mBackgroundBmp.scaleTo(gameRes);

    SDL_Surface *swatchBmp = mBackgroundBmp.getSDLSurface();

    mpBackgroundSfc.reset( g_pVideoDriver->convertThroughBlitSfc( swatchBmp ), &SDL_FreeSurface );


    // Besides the Background Bitmap we need to draw two lines
	SDL_Surface *backSfc = mpBackgroundSfc.get();

    Uint32 color = SDL_MapRGB( backSfc->format, 84, 234, 84 );
	SDL_Rect scoreRect;
    scoreRect.w = 15*gameRes.w/32;
    scoreRect.h = gameRes.h/200;
    scoreRect.x = gameRes.w/4;

    if(gameRes.h > 200) // This will display the line score correctly.
        scoreRect.y = 55*gameRes.h/202;
    else
        scoreRect.y = 55;

	SDL_FillRect(backSfc, &scoreRect, color);
}


void CGUIDialog::processLogic()
{
    // For the special effect not used in the galaxy engine
    if( g_pBehaviorEngine->getEngine() != ENGINE_GALAXY )
    {
        if( mFXhStep > 0 )
        {
            mFXhStep--;
            return;
        }
        else if( mFXvStep > 0 )
        {
            mFXvStep--;
            return;
        }
    }

    // Try to get a control that is waiting for input to be typed
    CGUIInputText *pInputCtrl = nullptr;
    for( auto &it : mControlList )
    {
        CGUIControl *ctrl = it.get();
        pInputCtrl = dynamic_cast<CGUIInputText*>(ctrl);
        if(pInputCtrl)
        {
            if(pInputCtrl->Typing())
               break;
            pInputCtrl = nullptr;
        }
    }

    if(pInputCtrl != nullptr)
    {
        pInputCtrl->processLogic();
    }
    else
    {

        // Process the subcontrols its logic
        int sel = 0;
        for( auto &it : mControlList )
        {
            CGUIControl *ctrl = it.get();

            ctrl->processLogic();

            if( dynamic_cast<CGUIButton*>(ctrl) || dynamic_cast<CGUIInputText*>(ctrl) )
            {
                if( ctrl->getHovered() )
                {
                    mpCurrentCtrl = ctrl;
                    mSelection = sel;
                }
            }
            sel++;
        }
    }

    if(!g_pInput->m_EventList.empty())
        g_pInput->m_EventList.clear();
}

void CGUIDialog::processRendering()
{
    processRendering(g_pVideoDriver->getBlitSurface());
}


void CGUIDialog::processRendering(SDL_Surface *blit)
{        
    CRect<Uint16> GameRes = g_pVideoDriver->getGameResolution();
    CRect<float> screenRect(0, 0, GameRes.w, GameRes.h);

    auto engine = g_pBehaviorEngine->getEngine();
    auto *bgSfc = mpBackgroundSfc.get();

    if( engine == ENGINE_GALAXY )
	{
        SDL_BlitSurface( bgSfc, nullptr, blit, nullptr );
	}
	else
    {
        SDL_Rect lRect;

        if( mFXhStep == 0 && mFXvStep == 0 )
        {
            lRect = g_pVideoDriver->toBlitRect(mRect);
            SDL_BlitSurface( bgSfc, nullptr, blit, &lRect );
        }
        else
        {
            CRect<float> fxRect = mRect;

            if( mFXhStep > 0 )
            {
                fxRect.w = (MAX_STEPS-mFXhStep)*(mRect.w/float(MAX_STEPS));
                fxRect.x = fxRect.x + (mRect.w-fxRect.w)/2;
            }

            if( mFXvStep > 0 )
            {
                fxRect.h = (MAX_STEPS-mFXvStep)*(mRect.h/float(MAX_STEPS));;
                fxRect.y = fxRect.y + (mRect.h-fxRect.h)/2;
            }

            lRect = g_pVideoDriver->toBlitRect(fxRect);

            // Makes the Border look more like in DOS-Keen
            if( engine == ENGINE_VORTICON && lRect.h < 16 )
                lRect.h = 16;

            auto srcRect = lRect;
            srcRect.y = srcRect.x = 0;

            if( engine == ENGINE_VORTICON )
            {
                auto *tmpSfc = mpTempSfc.get();
                SDL_FillRect( tmpSfc, &srcRect, 0xFFFFFFFF );
                drawBorderRect( tmpSfc, srcRect );
                SDL_BlitSurface( tmpSfc, &srcRect, blit, &lRect );
            }
            else
            {
                SDL_BlitSurface( bgSfc, &srcRect, blit, &lRect );
            }
        }

        if( mFXhStep > 0 || mFXvStep > 0 )
            return;
    }

	for( auto &it : mControlList )
	{
        it->processRender(screenRect);
	}       
}
