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
mScrollbar(rect)
{
    GsRect<float> scrollRect(0.0f, 0.0f, 0.1f, 1.0f);
    mScrollbar.setRect(scrollRect);

    mScrollbar.setScrollDownFn([this]
                               {
                                this->moveY(-0.05f);
                               });
    mScrollbar.setScrollUpFn([this]
                               {
                                this->moveY(0.05f);
                               });

    mScrollbar.setBackgroundColor(GsColor(0x7F, 0x7F, 0x7F));
}


void CGUITextSelectionList::setConfirmButtonEvent(CEvent *ev)
{
	mConfirmEvent.reset(ev);
}

void CGUITextSelectionList::setConfirmButtonEvent(const std::function <void ()>& f)
{
    mConfirmFunction = f;
}

void CGUITextSelectionList::setBackButtonEvent(CEvent *ev)
{
	mBackEvent.reset(ev);
}


bool CGUITextSelectionList::sendEvent(const InputCommand command)
{

    auto &controlsList = getControlsList();

	if(command == IC_UP)
	{
        auto it = controlsList.rbegin();
        auto lastSelected = controlsList.rend();

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
        if(it != controlsList.rend())
        {
            auto ctrl = std::dynamic_pointer_cast<GsControl>(*it);
            ctrl->select(true);
            setSelection(int(controlsList.size()) -
                         int(std::distance(controlsList.rbegin(), it)));

            return true;
        }

        return false;
    }
	else if(command == IC_DOWN)
	{
        auto it = controlsList.begin();
        auto lastSelected = controlsList.end();

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
        if(it != controlsList.end())
        {
            auto ctrl = std::dynamic_pointer_cast<GsControl>(*it);
            ctrl->select(true);
            setSelection(int(std::distance(controlsList.begin(), it)));
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
        if(mConfirmFunction)
            mConfirmFunction();
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

    GsRect<float> frameRect(0.1f, float(numElem)*0.1f,
                            0.9f, 0.1f);

    auto selectionFrame =
            add( new GsSelectableText(frameRect, text) );

    selectionFrame->setParent(this);

    selectionFrame->setActivationEvent( [this]()
                                        {this->updateSelection();} );    
}

void CGUITextSelectionList::updateSelection()
{
    auto &controlsList = getControlsList();
    int idx = 0;
    for(auto &control : controlsList)
    {
        auto selectableText =
                std::dynamic_pointer_cast<GsSelectableText>(control);

        if(selectableText)
        {
            if(selectableText->isSelected())
            {
                setCurrentControl(control);
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

    mScrollbar.processLogic();

    auto &controlsList = getControlsList();
    for(auto &widget : controlsList)
    {
        widget->processLogic();
    }
}


void CGUITextSelectionList::processPointingStateRel(const GsRect<float> &rect)
{  
    GsScrollingFrame::processPointingStateRel(rect);

    auto absRect = rect.transformed(getRect());
    mScrollbar.processPointingStateRel(absRect);
}

const std::string &
CGUITextSelectionList::getItemString(const unsigned int sel) const
{
    return mItemList[sel].mText;
}

void CGUITextSelectionList::unselectAll()
{
    auto &controlsList = getControlsList();
    for(auto &widget : controlsList)
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

    // Transform to the display coordinates
    GsRect<float> scrollRect = getRect();
    scrollRect.transform(RectDispCoordFloat);

    mScrollbar.processRender(scrollRect);
}

