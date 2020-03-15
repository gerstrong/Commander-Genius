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

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "GsWidget.h"

#include <graphics/GsTexture.h>
#include <graphics/GsSurface.h>

#if defined(USE_SDL_TTF)
#include <graphics/GsTrueTypeFont.h>
#endif


class CGUIText : public GsWidget
{
public:

    CGUIText(const std::string &text,
             const GsRect<float> &rect);

    CGUIText(const std::string &text,
             const GsRect<float> &rect,
             const int fontId);

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

    void setFontId(const int fontId)
    {
        mFontId = fontId;
    }

    int getFontId() const
    {
        return mFontId;
    }


protected:

    void updateTTFTextSfc(const GsRect<float> &displayRect);

    void updateLegacyTextSfc(const GsRect<float> &displayRect);

    std::vector<std::string> mTextVec;

    std::map< GsColor, std::vector<GsSurface> > mTextSfcVecByColor;

    GsColor mTextColor;
	
private:

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

    bool mHCentered = true;

    enum class ScrollDir
    {
        NONE,
        LEFT,
        RIGHT
    } mScrollDir = ScrollDir::NONE;        

    int mFontId = -1;
};

#endif /* CGUITEXT_H_ */
