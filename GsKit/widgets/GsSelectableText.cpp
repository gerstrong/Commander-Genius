
#include "GsSelectableText.h"
#include "GsTextSelectionList.h"
#include "GsText.h"

GsSelectableText::
GsSelectableText(const GsRect<float> &rect,
                 const std::string &text) :
GsFrame(rect)
{
    enableBackground(false);

    GsRect<float> textRect(0.1f, 0.1f,
                           0.8f, 0.8f);

    auto newTextWidget =
            add( new CGUIText(text, textRect) );

    newTextWidget->enableCenteringH(false);

    mBgColorHovered  = GsColor(0xAF, 0xAF, 0xFF);
    mBgColorPressed  = GsColor(0x3F, 0x3F, 0xFF);
    mBgColorReleased = GsColor(0x3F, 0x3F, 0xFF);
    mBgColorSelected = GsColor(0x3F, 0x3F, 0xFF);
}

bool GsSelectableText::sendEvent( const std::shared_ptr<CEvent> &event )
{
    return true;
}

void GsSelectableText::processLogic()
{
    enableBackground(false);

    if(mHovered || mPressed || mReleased || mSelected)
        enableBackground(true);
    else
        return;

    GsColor bgColor(0xFF, 0xFF, 0xFF);

    if(mHovered)
    {
        bgColor.converge(mBgColorHovered);
    }

    if(mPressed)
    {
        bgColor.converge(mBgColorPressed);
    }

    if(mReleased)
    {
        bgColor.converge(mBgColorReleased);

        auto parent =
                dynamic_cast<CGUITextSelectionList*>(getParent());

        if(parent)
        {
            parent->unselectAll();
        }

        mSelected = true;
    }

    if(mSelected)
    {
        bgColor.converge(mBgColorSelected);
    }

    setBackgroundColor(bgColor);

    GsFrame::processLogic();
}


