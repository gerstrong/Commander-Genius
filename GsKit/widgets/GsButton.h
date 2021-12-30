/*
 * GsButton.h
 *
 *  Created on: 26.04.2020
 *      Author: gerstrong
 *
 *  Widget for simple buttons
 */

#ifndef GsButton_H_
#define GsButton_H_

#include "GsControl.h"
#include <string>
#include <memory>
#include <map>
#include <graphics/GsSurface.h>
#include <base/utils/Color.h>
#include <graphics/GsDynColor.h>
#include <widgets/GsText.h>

#include <functional>

#include <base/utils/property_tree/property_tree.h>


class GsButton : public GsControl
{
public:

    GsButton(const std::string& text,
             const GsRect<float> &rect,
             CEvent *ev = nullptr,
             const int fontId = -1,
             const GsColor &color = GsColor(0x52, 0xfb, 0x52));


    GsButton(const std::string& text,
             const GsRect<float> &rect,
             CEvent *ev,
             const int fontId,
             const float red,
             const float green,
             const float blue);

    GsButton(const std::string& text,
             CEvent *ev = nullptr,
             const int fontId = -1,
             const float red = 0.32f,
             const float green = 0.98f,
             const float blue = 0.32f);

    GsButton(const std::string& text,
             const std::function <void ()>& f,
             const int fontId = -1,
             const float red = 0.32f,
             const float green = 0.98f,
             const float blue = 0.32f);


    GsButton(const std::string& text,
             const GsRect<float> &rect,
             const std::function <void ()>& f,
             const int fontId = -1,
             const GsColor &color = GsColor(0x52, 0xfb, 0x52));


    GsButton(const std::string& text,
             const GsRect<float> &rect,
             const std::function <void ()>& f,
             const int fontId,
             const float red,
             const float green,
             const float blue);



    bool sendEvent(const InpCmd command) override;

    void updateGraphics() override;

    void setActivationEvent(const std::function <void ()>& f)
    {
        mFunction = f;
    }

    void activateFunction()
    {
        mFunction();
    }

    /**
     * @brief drawEnabledButton Drawing of a button with fading effects.
     * @param blitsfc sfc where to draw
     * @param lRect Coordinates
     * @param alternate Alterante color wanted?
     */
    void drawEnabledButton(GsWeakSurface &, const SDL_Rect &, const bool);

    virtual void processLogic() override;

    virtual void drawNoStyle(const SDL_Rect &lRect);

    void drawVorticonStyle(SDL_Rect& lRect);

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) override;

    virtual void processRender(const GsRect<float> &srcRectFloat,
                               const GsRect<float> &dstRectFloat) override;

    virtual void setText(const std::string& text)
    {
        mText = text;
        mTextWidget.setText(text);
    }

    void setTextColor(const GsColor &color)
    {
        mTextWidget.setTextColor(color);
    }

    std::shared_ptr<CEvent> &event()
    {
        return mEvent;
    }

    void setEvent(std::shared_ptr<CEvent> &event)
    {
        mEvent = event;
    }


    virtual std::string getText()
    {	return mText;	}

    void enableBorder(const bool value)
    {
        mShowBorders = value;
    }

    void enableBackground(const bool value)
    {
        mUseBackground = value;
    }

    void enableCenteringH(const bool value)
    {
        mTextWidget.enableCenteringH(value);
    }

    void setFontId(const int fontId)
    {
        mTextWidget.setFontId(fontId);
    }

    void setEnabledColor(GsColor&& color)
    {
        mEnabledColor = color;
    }

protected:

    CGUIText mTextWidget;
    std::string mText;

private:


    std::shared_ptr<CEvent> mEvent;

    // Different fixed colors to be used for the button
    GsColor mSelectedColor = GsColor(255, 255, 255);
    GsColor mEnabledColor  = GsColor(230, 230, 230);
    GsColor mDisabledColor = GsColor(128, 128, 128);


    // Current colors used for the Buttons
    GsDynColor mFillColor = GsDynColor(0.5f,0.5f,0.5f);
    GsDynColor mBorderColor = GsDynColor(0.5f,0.5f,0.5f);


    std::function <void ()> mFunction;

    bool mShowBorders = true;

    bool mUseBackground = false; // Standard, some controls may be highlighted
                                 // if hovered or selected
};

std::shared_ptr<GsButton> createButtonFrom(const GsKit::ptree &node);

#endif /* GsButton_H_ */

