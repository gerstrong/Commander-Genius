#include "GsBanner.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>
#include <base/GsLogging.h>

const int TIME_UNTIL_CHANGE = 180;
const int TIME_TRANSITION = 30;

std::shared_ptr<CGUIBanner> createGUIBannerFrom(const GsKit::ptree &node)
{
    GsRect<float> dim;
    std::string name, tag;

    try
    {
        for( auto &item : node )
        {
            if(item.first == "<xmlattr>")
            {
                dim = GsRect<float>
                        (item.second.get<std::string>("dim"));
                name = item.second.get<std::string>("name");
                tag = item.second.get<std::string>("tag");
            }
        }
    }
    catch(std::exception const& ex)
    {
        gLogging << "Exception while building CGUIBanner: "
                 << ex.what() << "\n";
        return nullptr;
    }
    catch(...)
    {
        gLogging << "Unknown Exception while reading CGUIBanner node."
                 << CLogFile::endl;
        return nullptr;
    }

    std::shared_ptr<CGUIBanner> w
            (new CGUIBanner( name, dim ));

    w->setTag(tag);

    return w;
}

CGUIBanner::CGUIBanner(const std::string& text, const GsRect<float> &rect) :
GsText(text, rect)
{
}


void CGUIBanner::setText(const std::string& text)
{
    GsText::setText(text);
    mCurLineIdx = 0;
}

void CGUIBanner::processLogic()
{
    if(mTransition)
    {
        if(timer >= TIME_TRANSITION)
        {
            timer = 0;
            alpha = 0;
            mTransition = !mTransition;

            return;
        }
    }
    else
    {
        if(timer >= TIME_UNTIL_CHANGE)
        {
            timer = 0;

            mPreLineIdx = mCurLineIdx;

            mCurLineIdx++;

            if( mCurLineIdx >= mTextVec.size() )
                mCurLineIdx = 0;

            alpha = 0;
            mTransition = !mTransition;

            return;
        }
    }

    alpha = uint8_t((255*timer)/TIME_TRANSITION);

    timer++;

}

void CGUIBanner::processRender(const GsRect<float> &RectDispCoordFloat)
{

    // Transform to the display coordinates
    GsRect<float> displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);

    updateTTFTextSfc(displayRect);

    auto &blit = gVideoDriver.gameSfc();

    auto renderTxt = [&](GsSurface &textSfc)
    {
        if(!textSfc.empty())
        {
            const auto textW = textSfc.width();
            const auto textH = textSfc.height();
            GsVec2D<int> textSfcDim(textW, textH);

            GsRect<float> blitPos = displayRect;
            blitPos.pos = blitPos.pos + (blitPos.dim-textSfcDim)/2;

            textSfc.blitTo(blit, blitPos.SDLRect());
        }
    };

    auto &textSfcVec = mTextSfcVecByColor[mTextColor];

    if(mTransition)
    {
        auto &sfcPrev = textSfcVec[mPreLineIdx];
        auto &sfcCur  = textSfcVec[mCurLineIdx];

        sfcPrev.setAlpha(255-alpha);
        sfcCur.setAlpha(alpha);

        renderTxt( sfcPrev );
        renderTxt( sfcCur );
    }
    else
    {
        renderTxt( textSfcVec[mCurLineIdx] );
    }

/*
    unsigned int totTextSfcH = 0;
    for(auto &textSfc : mTextSfcVec)
    {
        if(textSfc.empty())
            break;

        const auto textW = textSfc.width();
        const auto textH = textSfc.height();
        GsVec2D<int> textSfcDim(textW, textH);

        GsRect<float> blitPos = displayRect;
        blitPos.pos = blitPos.pos + (blitPos.dim-textSfcDim)/2;
        blitPos.pos.y += totTextSfcH;

        textSfc.blitTo(blit, blitPos.SDLRect());
        totTextSfcH += textH;
    }
*/

}
