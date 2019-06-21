/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>
#include <graphics/cgttf.h>
#include <base/GsTTFSystem.h>
#include <base/CInput.h>
#include <base/PointDevice.h>
#include <base/utils/Color.h>


#include "GsTextSelectionList.h"
#include "GsSelectableText.h"

CGUITextSelectionList::
CGUITextSelectionList(const GsRect<float> &rect)  :
GsScrollingFrame(rect, rect),
mScrollbar(this)
{
    GsRect<float> scrollRect(0.0f, 0.0f, 0.1f, 1.0f);
    mScrollbar.setRect(scrollRect);       
}


void CGUITextSelectionList::setConfirmButtonEvent(CEvent *ev)
{
	mConfirmEvent.reset(ev);
}

void CGUITextSelectionList::setBackButtonEvent(CEvent *ev)
{
	mBackEvent.reset(ev);
}

bool CGUITextSelectionList::sendEvent(const InputCommand command)
{
	if(command == IC_UP)
	{
        mReleasedSelection--;

        if(mReleasedSelection < 0)
        {
            mReleasedSelection = 0;
            mSelected = false;
            return false;
        }

        if( mReleasedSelection < mScrollbar.scrollPos() )
        {
            mScrollbar.scrollUp();
        }

		return true;
	}
	else if(command == IC_DOWN)
	{
        const int last = static_cast<int>(mItemList.size());

        mReleasedSelection++;

        if(mReleasedSelection >= last)
        {
            mReleasedSelection = last-1;
            mSelected = false;
            return false;
        }

        if( mReleasedSelection >= mScrollbar.lastToShow() - mScrollbar.scrollPos() )
        {
            mScrollbar.scrollDown();
        }

		return true;
	}
	else if(command == IC_STATUS || command == IC_JUMP ||
            command == IC_POGO || command == IC_FIRE ||
            command == IC_RUN)
	{
		if(mConfirmEvent)
			gEventManager.add(mConfirmEvent);
		return true;
	}
	else if(command == IC_BACK)
	{
		if(mBackEvent)
			gEventManager.add(mBackEvent);
		return true;
	}
	else
		return false;

}

void CGUITextSelectionList::addText(const std::string &text)
{    
    const auto numElem = mItemList.size();

    mItemList.push_back( item(text) );

    GsRect<float> frameRect(0.0f, float(numElem)*0.1f,
                            1.0f, 0.1f);

    auto selectionFrame =
            addWidget( new GsSelectableText(frameRect, text) );

    selectionFrame->setParent(this);

    selectionFrame->setActivationEvent( [this]()
                                        {this->updateSelection();} );
}

void CGUITextSelectionList::updateSelection()
{

    int idx = 0;
    for(auto &widget : mWidgetList)
    {
        auto selectableText =
                std::dynamic_pointer_cast<GsSelectableText>(widget);

        if(selectableText)
        {
            if(selectableText->isSelected())
            {
                mSelection = idx;
                break;
            }

            idx++;
        }
    }
}

void CGUITextSelectionList::processLogic()
{
    if(!mEnabled)
    {
        return;
    }

    for(auto &widget : mWidgetList)
    {
        widget->processLogic();
    }
}


void CGUITextSelectionList::processPointingStateRel(const GsRect<float> &rect)
{
    const auto absRect = rect.transformed(getRect());

    for(auto &widget : mWidgetList)
    {
        widget->processPointingStateRel(absRect);
    }
}

int CGUITextSelectionList::getSelection() const
{
    return mReleasedSelection;
}

void CGUITextSelectionList::setSelection(const int sel)
{
    const int last = static_cast<int>(mItemList.size());

    if(sel < 0)
    {
        mReleasedSelection = 0;
        mSelected = false;
        return;
    }
    if(sel >= last)
    {
        mReleasedSelection = last-1;
        mSelected = false;
        return;
    }

    mReleasedSelection = sel;
}

const std::string &
CGUITextSelectionList::getItemString(const unsigned int sel) const
{
    return mItemList[sel].mText;
}

void CGUITextSelectionList::unselectAll()
{
    for(auto &widget : mWidgetList)
    {
        auto ctrl = std::dynamic_pointer_cast<GsSelectableText>(widget);

        if(ctrl)
        {
            ctrl->select(false);
        }
    }
}


void CGUITextSelectionList::processRenderSimple(const GsRect<float> &RectDispCoordFloat)
{
    // Blit the List surface
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    GsRect<float> displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);

    GsRect<Uint16> origRect(displayRect);
    GsRect<Uint16> rect = origRect;

    GsColor boxColor(0xDF, 0xDF, 0xDF, 0xFF);

    if(mEnabled)
    {
        boxColor = GsColor(0xFF, 0xFF, 0xFF, 0xFF);
    }

    blitsfc.fillRGBA(rect, boxColor);
}

void CGUITextSelectionList::processRenderTTF(const GsRect<float> &RectDispCoordFloat)
{
    // Blit the List surface
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    GsRect<float> displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);

    GsRect<Uint16> origRect(displayRect);
    GsRect<Uint16> rect = origRect;

    GsColor boxColor(0xDF, 0xDF, 0xDF, 0xFF);

    if(mEnabled)
    {
        boxColor = GsColor(0xFF, 0xFF, 0xFF, 0xFF);
    }

    blitsfc.fillRGBA(rect, boxColor);
}


void CGUITextSelectionList::processRender(const GsRect<float> &RectDispCoordFloat)
{
    GsScrollingFrame::processRender(RectDispCoordFloat);
}
