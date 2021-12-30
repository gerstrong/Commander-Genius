/*
 * GsButton.cpp
 *
 *  Created on: 26.11.2011
 *      Author: gerstrong
 */

#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include <widgets/GsMenuController.h>
#include <base/PointDevice.h>
#include <base/GsTTFSystem.h>
#include <base/GsLogging.h>

#include "GsButton.h"

std::shared_ptr<GsButton> createButtonFrom(const GsKit::ptree &node)
{
    GsRect<float> dim;
    std::string name;
    std::string event;
    std::string id;
    std::string tag;

    try
    {
        for( auto &item : node )
        {
            if(item.first == "<xmlattr>")
            {
                dim = GsRect<float>
                      (item.second.get<std::string>("dim"));
                name = item.second.get<std::string>("name");
                event = item.second.get<std::string>("event");
                id = item.second.get<std::string>("id");
                tag = item.second.get<std::string>("tag");
            }
        }
    }
    catch(std::exception const& ex)
    {
        gLogging << "Exception while building button: "
                 << ex.what() << "\n";
        return nullptr;
    }
    catch(...)
    {
        gLogging << "Unknown Exception while reading menu node."
                 << CLogFile::endl;
        return nullptr;
    }



    std::shared_ptr<GsButton> btn
        (new GsButton( name, dim,
                       [](){ gEventManager.add("GMQuit"); }, -1 ));

    return btn;
}


GsButton::GsButton(const std::string& text,
                   const GsRect<float> &rect,
            CEvent *ev,
            const int fontId,
            const float red,
            const float green,
            const float blue) :
GsControl(rect),
mTextWidget(text,
            GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f),
            fontId),
mText(text),
mEvent(ev),
mSelectedColor(Uint8(red*255.0f), Uint8(green*255.0f), Uint8(blue*255.0f))
{}

GsButton::GsButton(const std::string& text,
         const GsRect<float> &rect,
         CEvent *ev,
         const int fontId,
         const GsColor &color) :
GsButton(text, rect, ev, fontId,
         float(color.r)/float(0xFF),
         float(color.g)/float(0xFF),
         float(color.b)/float(0xFF) )
{}

GsButton::GsButton(const std::string& text,
            CEvent *ev,
            const int fontId,
            const float red,
            const float green,
            const float blue) :
GsControl(),
mTextWidget(text,
            GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f),
            fontId),
mText(text),
mEvent(ev),
mSelectedColor(Uint8(red*255.0f), Uint8(green*255.0f), Uint8(blue*255.0f))
{}

GsButton::GsButton(const std::string& text,
                   const std::function <void ()>& f,
                   const int fontId,
                   const float red,
                   const float green,
                   const float blue) :
GsControl(),
mTextWidget(text,
            GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f),
            fontId),
mText(text),
mSelectedColor(Uint8(red*255.0f), Uint8(green*255.0f), Uint8(blue*255.0f)),
mFunction(f)
{}

GsButton::GsButton(const std::string& text,
         const GsRect<float> &rect,
         const std::function <void ()>& f,
         const int fontId,
         const float red,
         const float green,
         const float blue) :
    GsControl(rect),
    mTextWidget(text,
                GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f),
                fontId),
    mText(text),
    mSelectedColor(Uint8(red*255.0f), Uint8(green*255.0f), Uint8(blue*255.0f)),
    mFunction(f)
{}


GsButton::GsButton(const std::string& text,
         const GsRect<float> &rect,
         const std::function <void ()>& f,
         const int fontId,
         const GsColor &color):
GsButton(text,rect,f,fontId,
         float(color.r)/float(0xFF),
         float(color.g)/float(0xFF),
         float(color.b)/float(0xFF))
{}




bool GsButton::sendEvent(const InpCmd command)
{
    if(!mEnabled)
        return false;

    if(command == IC_STATUS || command == IC_JUMP)
    {
        if(mEvent)
            gEventManager.add(mEvent);

        if(mFunction)
            gEventManager.add(mFunction);

        return true;
    }
    return false;
}


void GsButton::updateGraphics()
{
    if( auto menuEv = std::dynamic_pointer_cast<OpenMenuEvent>(mEvent)  )
    {
        menuEv->mMenuDialogPointer->updateGraphics();
    }
}


void GsButton::processLogic()
{
    mFillColor.ponder(0.05f);
    mBorderColor.ponder(0.05f);

    // Color operations
    if(!mEnabled)
    {
        mFillColor.setTargetColor(mDisabledColor);
        mBorderColor.setTargetColor(mDisabledColor);
    }
    else
    {
        GsColor selectFillColor = mSelected ? mSelectedColor : mEnabledColor;
        GsColor selectFillColorDark = selectFillColor;
        GsColor selectFillColorLight = selectFillColor;

        if(mSelected)
        {
            selectFillColorLight = selectFillColor * 2.0f;
            selectFillColorDark = selectFillColor * 0.5f;
        }
        else
        {
            selectFillColorLight.converge(GsColor(255,255,255));
            selectFillColorDark.converge(mSelectedColor);
        }

        if(mHovered && mPressed)
        {
            selectFillColor = selectFillColorDark;
        }
        else if(mHovered)
        {
            selectFillColor = selectFillColorLight;
        }
        else if(mPressed)
        {
            selectFillColor = selectFillColorDark;
            selectFillColor.converge(selectFillColorLight);
        }
        else if(mReleased)
        {
            selectFillColor = mSelectedColor;
        }

        mFillColor.setTargetColor(selectFillColor);
        mBorderColor.setTargetColor(mSelected ? GsColor(0,0,0) : mDisabledColor);
    }

    // Functor/event handling
    if(mEnabled)
    {
        // If button was pushed and gets released, trigger the assigned event.
        if(mReleased)
        {
            if(mFunction)
            {
                mFunction();
            }
            else
            {
                gEventManager.add(mEvent);
            }
        }
    }

    mTextWidget.processLogic();
}

void GsButton::drawNoStyle(const SDL_Rect& lRect)
{
    if(lRect.h == 0 || lRect.w == 0)
        return;

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    const auto fillColor = mFillColor.toUint32(blitsfc);
    const auto borderColor = mBorderColor.toUint32(blitsfc);

    GsRect<Uint16> rect(lRect);

    if(mShowBorders)
    {
        blitsfc.drawRect( rect, 2, borderColor, fillColor );
    }
    else if(mUseBackground)
    {
        blitsfc.drawRect( rect, fillColor );
    }
}


void GsButton::drawEnabledButton(GsWeakSurface &,
                                 const SDL_Rect&,
                                 const bool )
{}


void GsButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    auto displayRect = getRect();

    displayRect.transform(RectDispCoordFloat);
    auto lRect = displayRect.SDLRect();

    drawNoStyle(lRect);

    mTextWidget.processRender(displayRect);
}

void GsButton::processRender(const GsRect<float> &backRect,
                             const GsRect<float> &frontRect)
{
    // Transform this object display coordinates
    auto objBackRect = backRect.transformed(getRect());
    auto objFrontRect = objBackRect.clipped(frontRect);

    drawNoStyle( objFrontRect.SDLRect() );


    mTextWidget.processRender(objBackRect,
                              objFrontRect);
}
