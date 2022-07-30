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

#include "GsButton.h"
#include "GsBitmapBox.h"
#include "GsTextSelectionList.h"
#include "GsSelectionList.h"
#include "GsBanner.h"


const unsigned int MAX_STEPS = 20;

bool buildDialogWidgetsFrom(CGUIDialog &dlg,
                            const GsKit::ptree &tree)
{
    try
    {
        for( auto &node : tree )
        {
            // Filter the comments ...
            if(node.first == "<xmlcomment>")
                continue;

            const auto key = node.first;
            const auto subtree = node.second;

            auto switchStr = [&](const std::string &text,
                                  auto func)
            { if(key == text)  dlg.add(func(subtree)); };

            switchStr("Button", createButtonFrom);
            switchStr("Text", createTextFrom);
            switchStr("BitmapBox", createBitmapBoxFrom);
            switchStr("GsSelectionList", createSelectionListFrom);
            switchStr("GUITextSelectionList", createGUITextSelectionListFrom);
            switchStr("GUIBanner", createGUIBannerFrom);

        }
    }
    catch(std::exception const& ex)
    {
        gLogging << "Exception while reading menu node: " << ex.what() << "\n";
        return false;
    }
    catch(...)
    {
        gLogging << "Unknown Exception while reading menu node\n.";
        return false;
    }

    return true;
}



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


bool CGUIDialog::sendEvent(const std::shared_ptr<CEvent> &evPtr)
{
    auto &ctrlList = getControlsList();

    if( CommandEvent *ev = dynamic_cast<CommandEvent*>(evPtr.get()) )
    {
        // Send all the other events the active control element
        for( auto &control : ctrlList )
        {
            if( control->isSelected() )
            {
                if( control->sendEvent(ev->mCommand) )
                {
                    return true;
                }
                else
                {
                    control->select( false );
                }
            }
            else
            {
                control->select( false );
            }
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
    else
    {
        return GsWidgetsManager::sendEvent(evPtr);
    }

    return false;
}


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
    std::shared_ptr<CGUIInputText> pInputCtrl;
    for( auto &it : getWidgetList() )
    {
        pInputCtrl =
             std::dynamic_pointer_cast<CGUIInputText>(it);
        if(pInputCtrl)
        {
            if(pInputCtrl->Typing())
                break;
            pInputCtrl = nullptr;
        }
    }

    processPointingState(getRect());

    if(pInputCtrl)
    {
        pInputCtrl->processLogic();
    }

    // Process the subcontrols inputs
    const auto localRect = getRect();

    auto wList = getWidgetList();

    for( auto &widget : wList )
    {
        widget->processLogic();
        widget->processPointingStateRel(localRect);
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

    auto &wList = getWidgetList();
    for( auto &it : wList )
    {
        it->processRender(displayRect);
    }

    // If Dialog is disabled, make all included items a bit darker.
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
