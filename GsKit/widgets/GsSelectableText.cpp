
#include "GsSelectableText.h"
#include "GsTextSelectionList.h"
#include "GsText.h"
#include <base/GsLogging.h>

GsSelectableText::
GsSelectableText(const GsRect<float> &rect,
                 const std::string &text) :
GsFrame(rect)
{
    enableBackground(false);

    GsRect<float> textRect(0.1f, 0.1f,
                           0.8f, 0.8f);

    auto newTextWidget =
            add( new GsText(text, textRect) );

    newTextWidget->enableCenteringH(false);
    enableBackground(true);
}

bool GsSelectableText::sendEvent( const std::shared_ptr<CEvent> & )
{
    return true;
}

void GsSelectableText::processLogic()
{
    mBgColor.ponder(0.1f);

    // Color operations
    if(!mEnabled)
    {
        mBgColor.setTargetColor(mBgDisabledColor);
    }
    else
    {
        if(mSelected)
        {
            GsColor selectFillColor = mBgSelectedColor;

            if(mPressed)
                selectFillColor.converge(mBgPressColor);

            mBgColor.setTargetColor(selectFillColor);
        }
        else
        {
            GsColor selectFillColor = mBgEnabledColor;

            if(mHovered)
                selectFillColor.converge(mBgSelectedColor);
            if(mPressed)
                selectFillColor.converge(mBgPressColor);
            else if(mReleased)
                selectFillColor = mBgSelectedColor;

            mBgColor.setTargetColor(selectFillColor);
        }
    }

    if(mReleased)
    {
        auto parent =
                dynamic_cast<GsTextSelectionList*>(getParent());

        if(parent)
        {
            parent->unselectAll();
        }

        mSelected = true;
    }

    setBackgroundColor(mBgColor.getCurColor());

    GsFrame::processLogic();
}


