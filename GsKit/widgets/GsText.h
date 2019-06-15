/*
 * CGUIText.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  This is the text control for our new GUI
 */

#ifndef CGUITEXT_H_
#define CGUITEXT_H_

#include <string>
#include <list>
#include <memory>

#include "GsControl.h"

#include <graphics/GsTexture.h>
#include <graphics/GsSurface.h>

#if defined(USE_SDL_TTF)
#include <graphics/GsTrueTypeFont.h>
#endif


class CGUIText : public GsControl
{
public:

    CGUIText(const std::string& text,
             const GsRect<float> &rect);

    CGUIText(const std::string& text,
             const GsRect<float> &rect,
             const GsControl::Style style);

    void setupButtonSurface(const std::string& text = "");

	virtual void setText(const std::string& text);

    void setTextColor(const GsColor &color);

	virtual void processLogic();
    virtual void processRender(const GsRect<float> &RectDispCoordFloat);

    virtual void processRender(const GsRect<float> &backRect,
                               const GsRect<float> &frontRect);

	// Defines in character units how tall and wide the entire text is.
    GsRect<unsigned int> mTextDim;

    void enableCenteringH(const bool value)
    {
        mHCentered = value;
    }

protected:

    void updateTTFTextSfc(const GsRect<float> &displayRect);

    void updateLegacyTextSfc(const GsRect<float> &displayRect,
                             const GsColor &textColor);

    std::vector<std::string> mTextVec;

#if defined(USE_SDL_TTF)
    std::vector<GsSurface> mTextSfcVec;
#endif

	
private:
	std::unique_ptr<SDL_Surface> mpTextDarkSfc;
	std::unique_ptr<SDL_Surface> mpTextLightSfc;
	std::unique_ptr<SDL_Surface> mpTextDisabledSfc;


    /// Automatic horizontal scrolling in case the text is too big
    float mScrollPosMax = 0.0f;
    float mScrollPos = 0.0f;
    float mScrollVel = 0.3f;


#if defined(USE_SDL_TTF)
    GsTrueTypeFont mTrueTypeFont;
#endif

    std::string mText;
    int mFontSize = 0;
    bool mTextChanged = true;
    GsColor mTextColor;

    bool mHCentered = true;

    enum class ScrollDir
    {
        NONE,
        LEFT,
        RIGHT
    } mScrollDir = ScrollDir::NONE;        

};

#endif /* CGUITEXT_H_ */
