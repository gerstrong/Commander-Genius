/*
 * CGUIDialog.cpp
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *
 *  GUI Class for a Dialog which is able to carry other controls
 */

#include "GsDialog.h"
#include "GsButton.h"
#include "GsInputText.h"
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <graphics/GsGraphics.h>
#include <base/GsLogging.h>

//const unsigned int MAX_ELEMENTS_PER_PAGE = 7;
const unsigned int MAX_STEPS = 20;


CGUIDialog::CGUIDialog(const GsRect<float> &SrGsRect,
                       const FXKind fx) :
GsWidgetsManager(SrGsRect),
mFXSetup(fx)
{
    if( mFXSetup == FXKind::EXPAND )
    {
        mFXhStep = MAX_STEPS;
        mFXvStep = MAX_STEPS-3;
    }
}
/*
void CGUIDialog::updateGraphics()
{
    for( auto &control : mControlList )
    {
        control->updateGraphics();
    }
}
*/
/*
std::shared_ptr<GsControl>
CGUIDialog::addWidget( std::shared_ptr<GsControl> &newControl,
                        const GsRect<float>& RelRect )
{
    GsRect<float> AbsRect = RelRect;
    AbsRect.transform(mRect);
    newControl->mRect = AbsRect;

    auto ctrlPtr = newControl;

    mControlList.push_back( ctrlPtr );

    if(mControlList.size() == 1)
    {
        mpCurrentCtrl = mControlList.front().get();
    }

    return ctrlPtr;
}

*/
/*
std::shared_ptr<GsControl> 
CGUIDialog::addWidget( std::unique_ptr<GsControl> &newControl )
{    
    auto ctrlPtr = std::shared_ptr<GsControl>( move(newControl) );

    mControlList.push_back( ctrlPtr );

    fit();

	if(mControlList.size() == 1)
	{
	    mpCurrentCtrl = mControlList.front().get();
	}

    return ctrlPtr;
}
*/

/*
std::shared_ptr<GsControl> 
CGUIDialog::addWidget( GsControl *newControl,
                        const GsRect<float>& RelRect )
{
    std::unique_ptr<GsControl> ctrl(newControl);
    return addWidget( ctrl, RelRect );
}
*/

/*
std::shared_ptr<GsControl> 
CGUIDialog::addWidget( GsControl *newControl )
{
    std::unique_ptr<GsControl> ctrl(newControl);
    return addWidget(ctrl);
}
*/

/*

void CGUIDialog::selectPrevItem()
{
    if(!mpCurrentCtrl)
    {
        return;
    }

    if(mpCurrentCtrl->isSelected())
    {
        mpCurrentCtrl->select(false);
    }

	mSelection--;

	if( mSelection < 0 )
    {
		mSelection = mControlList.size()-1;
    }


	auto it = mControlList.begin();
	for( int i=0 ; it != mControlList.end() ; it++, i++ )
	{
		if( i ==  mSelection )
			break;
	}

	// Ensures that disabled items are skipped
	for( ; it != mControlList.begin() ; it-- )
	{
        if( (*it)->isEnabled() )
			break;

		mSelection--;
	}

    if( mSelection < 0 ) {
		mSelection = mControlList.size()-1;
        it = mControlList.end();
    }

    (*it)->select(true);
	mpCurrentCtrl = it->get();
}
*/


/*
void CGUIDialog::selectNextItem()
{
    if(!mpCurrentCtrl)
        return;

    if(mpCurrentCtrl->isSelected())
    {
        mpCurrentCtrl->select(false);
    }

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
        if( (*it)->isEnabled() )
			break;

		mSelection++;
	}

    if( mSelection >= static_cast<int>(mControlList.size()) ) {
		mSelection = 0;
        it = mControlList.begin();
    }

    (*it)->select(true);
	mpCurrentCtrl = it->get();
}
*/


/*
void CGUIDialog::setSelection(const int sel)
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

*/


/*
bool CGUIDialog::sendEvent(const std::shared_ptr<CEvent> &event )
{
    if( CommandEvent *ev = dynamic_cast<CommandEvent*>(event.get()) )
	{
		// Send all the other events the active control element
		int i=0;
        for( auto &it : mControlList )
        {
            if( i == mSelection )
            {
               if( it->sendEvent(ev->mCommand) )
                   return true;
               else
                   it->select( false );
            }
            else
            {
                it->select( false );
            }
            i++;
        }

        if(ev->mCommand == IC_DOWN || ev->mCommand == IC_RIGHT)
		{
			selectNextItem();
			return true;
		}
        else if(ev->mCommand == IC_UP || ev->mCommand == IC_LEFT)
		{
			selectPrevItem();
			return true;
		}
	}

	return false;
}

*/

/*

void CGUIDialog::fit()
{
	auto it = mControlList.begin();
	it++;

	size_t numControls = mControlList.size();
	const float charHeight = ( 1.0f/(float)(numControls+1) );

	size_t c = 1;
	for( ; it != mControlList.end() ; it++ )
	{
        GsRect<float> rect( 0.05f,
				   charHeight*((float)c),
				   mRect.dim.x,
				   charHeight-0.01f );

		rect.transform(mRect);

		(*it)->setRect( rect );
		c++;
	}

}
*/



void CGUIDialog::initEmptyBackground()
{
    const SDL_Rect lRect = gVideoDriver.toBlitRect(getRect());

    mBackgroundSfc.create(0, lRect.w, lRect.h, RES_BPP, 0, 0, 0, 0);
    mBackgroundSfc.fillRGB(230, 230, 230);
}


void CGUIDialog::drawBorderRect(SDL_Surface *backSfc, const SDL_Rect &Rect)
{
    GsFontLegacy &Font = gGraphics.getFontLegacy(1);
    Font.drawCharacter( backSfc, 1, 0, 0 );

    for( int x=8 ; x<Rect.w-8 ; x+=8 )
    {
        Font.drawCharacter( backSfc, 2, Uint16(x), 0 );
    }

    Font.drawCharacter( backSfc, 3, Uint16(Rect.w-8), 0 );

    for( int x=8 ; x<Rect.w-8 ; x+=8 )
    {
        Font.drawCharacter( backSfc, 7, Uint16(x), Uint16(Rect.h-8) );
    }

    for( int y=8 ; y<Rect.h-8 ; y+=8 )
    {
        Font.drawCharacter( backSfc, 4, 0, Uint16(y) );
    }

    for( int y=8 ; y<Rect.h-8 ; y+=8 )
    {
        Font.drawCharacter( backSfc, 5, Uint16(Rect.w-8), Uint16(y) );
    }

    Font.drawCharacter( backSfc, 6, 0, Uint16(Rect.h-8) );
    Font.drawCharacter( backSfc, 8, Uint16(Rect.w-8), Uint16(Rect.h-8) );
}

void CGUIDialog::processLogic()
{        
    // For the special effect not used in the galaxy engine
    if( mFXSetup == FXKind::EXPAND )
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

    if(!mEnabled)
    {
        return;
    }

    // Try to get a control that is waiting for input to be typed
    CGUIInputText *pInputCtrl = nullptr;
    for( auto &it : getWidgetList() )
    {
        auto *ctrl = it.get();
        pInputCtrl = dynamic_cast<CGUIInputText*>(ctrl);
        if(pInputCtrl)
        {
            if(pInputCtrl->Typing())
                break;
            pInputCtrl = nullptr;
        }
    }

    processPointingState(getRect());

    if(pInputCtrl != nullptr)
    {
        pInputCtrl->processLogic();
        return;
    }

    // Process the subcontrols inputs
    int sel = 0;
    const auto localRect = getRect();
    for( auto &widget : getWidgetList() )
    {
        widget->processLogic();
        widget->processPointingStateRel(localRect);

        if(auto ctrl = std::dynamic_pointer_cast<GsControl>(widget))
        {            
            if( std::dynamic_pointer_cast<GsButton>(ctrl) ||
                std::dynamic_pointer_cast<CGUIInputText>(ctrl) )
            {
                if( ctrl->isSelected() )
                {
                    setCurrentWidget(ctrl);
                    setSelection(sel);
                }
            }

            sel++;
        }
    }


}

void CGUIDialog::processRendering()
{
    processRendering(gVideoDriver.getBlitSurface());
}

void CGUIDialog::processRender(const GsRect<float> &RectDispCoordFloat)
{
    GsWeakSurface weakBlit(gVideoDriver.getBlitSurface());
    const auto rect = getRect();
    SDL_Rect lRect = gVideoDriver.toBlitRect(rect);

    if(mBackgroundSfc)
    {        
        // Check if the dimension are still correct.
        // They might be wrong when the resolution changed.

        const auto bgRectW = mBackgroundSfc.getSDLSurface()->w;
        const auto bgRectH = mBackgroundSfc.getSDLSurface()->h;

        if(bgRectW != lRect.w || bgRectH != lRect.h)
        {
            initEmptyBackground();
            return;
        }


        if( mFXSetup == FXKind::NONE )
        {
            mBackgroundSfc.blitTo(weakBlit, lRect);
        }
        else
        {

            if( mFXhStep == 0 && mFXvStep == 0 )
            {
                mBackgroundSfc.blitTo(weakBlit, lRect);
            }
            else
            {                                
                GsRect<float> fxRect = rect;

                if( mFXhStep > 0 )
                {
                    fxRect.dim.x =
                            (MAX_STEPS-static_cast<unsigned int>(mFXhStep))*
                            (rect.dim.x/float(MAX_STEPS));
                    fxRect.pos.x = fxRect.pos.x + (rect.dim.x-fxRect.dim.x)/2;
                }

                if( mFXvStep > 0 )
                {
                    fxRect.dim.y =
                            (MAX_STEPS-static_cast<unsigned int>(mFXvStep))*
                            (rect.dim.y/float(MAX_STEPS));
                    fxRect.pos.y = fxRect.pos.y + (rect.dim.y-fxRect.dim.y)/2;
                }

                lRect = gVideoDriver.toBlitRect(fxRect);

                // Makes the Border look more like in DOS-Keen
                if( mFXSetup == FXKind::EXPAND && lRect.h < 16 )
                    lRect.h = 16;

                auto srGsRect = lRect;
                srGsRect.y = srGsRect.x = 0;

                if( mTempSfc && mFXSetup == FXKind::EXPAND )
                {
                    mTempSfc.fillRGBA(srGsRect, 0xFF,0xFF,0xFF,0xFF);

                    drawBorderRect( mTempSfc.getSDLSurface(), srGsRect );
                    mTempSfc.blitTo(weakBlit, srGsRect, lRect);
                }
                else
                {
                    mBackgroundSfc.blitTo(weakBlit, srGsRect, lRect);
                }
            }

            if( mFXhStep > 0 || mFXvStep > 0 )
                return;
        }

    }

    
    auto displayRect = rect;
    displayRect.transform(RectDispCoordFloat);


    for( auto &it : getWidgetList() )
    {
        it->processRender(displayRect);
    }

    // If Dialog is disabled, make everthing within a bit darker.
    if(!mEnabled)
    {
        const SDL_Rect lRect = gVideoDriver.toBlitRect(rect);

        // Not yet created, create one
        if(!mDarkOverlaySfc ||
            (mDarkOverlaySfc.height() != lRect.h ||
             mDarkOverlaySfc.width() != lRect.w)  )
        {                                    
            mDarkOverlaySfc.create(0, lRect.w, lRect.h, RES_BPP, 0, 0, 0, 0);
            mDarkOverlaySfc.fillRGB(0, 0, 0);
            mDarkOverlaySfc.setAlpha(128);
        }

        mDarkOverlaySfc.blitTo(weakBlit, lRect);
    }
}


void CGUIDialog::processRendering(SDL_Surface*)
{
    GsRect<Uint16> GameRes = gVideoDriver.getGameResolution();
    GsRect<float> screenRect(0, 0, GameRes.dim.x, GameRes.dim.y);

    processRender(screenRect);
}
