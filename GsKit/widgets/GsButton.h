/*
 * CGUIText.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  This is the text control for our new GUI
 */

#ifndef GsButton_H_
#define GsButton_H_

#include "GsControl.h"
#include <string>
#include <memory>
#include <map>
#include <graphics/GsSurface.h>
#include <base/utils/Color.h>
#include <widgets/GsText.h>

#include <functional>

class GsButton : public GsControl
{
public:

    GsButton(const std::string& text,
             const GsRect<float> &rect,
             CEvent *ev = nullptr,
             const int fontId = -1,
             const GsColor &color = GsColor(0xEE, 0xEE, 0xFF));


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
             const float red = 0.933f,
             const float green = 0.933f,
             const float blue = 1.0f);

    GsButton(const std::string& text,
             const std::function <void ()>& f,
             const int fontId = -1,
             const float red = 0.933f,
             const float green = 0.933f,
             const float blue = 1.0f);


    GsButton(const std::string& text,
             const GsRect<float> &rect,
             const std::function <void ()>& f,
             const int fontId = -1,
             const GsColor &color = GsColor(0xEE, 0xEE, 0xFF));


    GsButton(const std::string& text,
             const GsRect<float> &rect,
             const std::function <void ()>& f,
             const int fontId,
             const float red,
             const float green,
             const float blue);



    bool sendEvent(const InputCommand command) override;

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
    void drawEnabledButton(GsWeakSurface &blitsfc,
                           const SDL_Rect &lRect,
                           const bool alternate);

    virtual void processLogic() override;

    void drawNoStyle(const SDL_Rect &lRect);

	void drawVorticonStyle(SDL_Rect& lRect);

    virtual void processRender(const GsRect<float> &RectDispCoordFloat) override;

    virtual void processRender(const GsRect<float> &srcRectFloat,
                               const GsRect<float> &dstRectFloat) override;

    void setText(const std::string& text)
	{
		mText = text;
        mTextWidget.setText(text);
        setupButtonSurface("  " + mText);
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


	std::string getText()
	{	return mText;	}

    virtual void setupButtonSurface(const std::string &text = "");

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

protected:

    std::string mText;
    CGUIText mTextWidget;

private:        

	std::shared_ptr<CEvent> mEvent;

    // Default-> Light Blue. This will colorize the buttons a bit. Effects continue working.
    const float mRed = 0.93f;
    const float mGreen = 0.93f;
    const float mBlue = 1.0f;


    std::function <void ()> mFunction;

    bool mShowBorders = true;

    bool mUseBackground = false; // Standard, some controls may be highlighted
                                 // if hovered or selected
};


#endif /* GsButton_H_ */
