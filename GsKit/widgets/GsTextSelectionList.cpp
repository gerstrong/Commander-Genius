/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include "GsTextSelectionList.h"
#include "GsSelectableText.h"

#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>
#include <graphics/cgttf.h>
#include <base/GsTTFSystem.h>
#include <base/CInput.h>
#include <base/PointDevice.h>
#include <base/utils/Color.h>


#include <algorithm>

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
        auto it = mWidgetList.rbegin();
        auto lastSelected = mWidgetList.rend();

        // Find iterator of last selected item
        while(it != lastSelected)
        {
            auto ctrl = std::dynamic_pointer_cast<GsControl>(*it);

            assert(ctrl);

            if(ctrl->isSelected())
            {
                ctrl->select(false);
                lastSelected = it;
                break;
            }

            it++;
        }

        it++;

        // If previous item was not the last one select it.
        if(it != mWidgetList.rend())
        {
            auto ctrl = std::dynamic_pointer_cast<GsControl>(*it);
            ctrl->select(true);
            mSelectedIdx =
                    int(mWidgetList.size()) -
                    int(std::distance(mWidgetList.rbegin(), it));
            return true;
        }

        return false;
    }
	else if(command == IC_DOWN)
	{
        auto it = mWidgetList.begin();
        auto lastSelected = mWidgetList.end();

        // Find iterator of last selected item
        while(it != lastSelected)
        {
            auto ctrl = std::dynamic_pointer_cast<GsControl>(*it);

            assert(ctrl);

            if(ctrl->isSelected())
            {
                ctrl->select(false);
                lastSelected = it;
                break;
            }

            it++;
        }

        it++;

        // If previous item was not the last one select it.
        if(it != mWidgetList.end())
        {
            auto ctrl = std::dynamic_pointer_cast<GsControl>(*it);
            ctrl->select(true);
            mSelectedIdx = int(std::distance(mWidgetList.begin(), it));
            return true;
        }

        return false;
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
    {
		return false;
    }
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
                setSelection(idx);
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

    GsControl::processPointingStateRel(rect);

    if(this->isReleased())
    {
        this->select(true);
    }

    for(auto &widget : mWidgetList)
    {
        widget->processPointingStateRel(absRect);
    }

}

int CGUITextSelectionList::getSelection() const
{
    return mSelectedIdx;
}

void CGUITextSelectionList::setSelection(const int sel)
{
    const int last = static_cast<int>(mItemList.size());

    if(sel < 0)        
    {
        mSelectedIdx = 0;
        mSelected = false;
        return;
    }
    if(sel >= last)
    {
        mSelectedIdx = last-1;
        mSelected = false;
        return;
    }

    mSelectedIdx = sel;
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
