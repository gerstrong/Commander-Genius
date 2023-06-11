/*
 * GsTextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include "GsTextSelectionList.h"
#include "GsSelectableText.h"

#include <graphics/GsGraphics.h>
#include <graphics/cgttf.h>
#include <base/GsTTFSystem.h>
#include <base/CInput.h>
#include <base/PointDevice.h>
#include <base/interface/Color.h>
#include <base/GsLogging.h>

#include <algorithm>

std::shared_ptr<GsTextSelectionList> createGUITextSelectionListFrom(const GsKit::ptree &node)
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
                tag = item.second.get<std::string>("tag");
            }
        }
    }
    catch(std::exception const& ex)
    {
        gLogging << "Exception while building GsTextSelectionList: "
                 << ex.what() << "\n";
        return nullptr;
    }
    catch(...)
    {
        gLogging << "Unknown Exception while reading GsTextSelectionList node."
                 << CLogFile::endl;
        return nullptr;
    }



    std::shared_ptr<GsTextSelectionList> w
        (new GsTextSelectionList( dim ));

    w->setTag(tag);

    return w;
}


GsTextSelectionList::
GsTextSelectionList(const GsRect<float> &rect)  :
GsSelectionList(rect)
{
}



void GsTextSelectionList::setConfirmButtonEvent(CEvent *ev)
{
    mConfirmEvent.reset(ev);
}

void GsTextSelectionList::setConfirmButtonEvent(const std::function <void ()>& f)
{
    mConfirmFunction = f;
}

void GsTextSelectionList::setBackButtonEvent(CEvent *ev)
{
    mBackEvent.reset(ev);
}

bool GsTextSelectionList::sendEvent(const std::shared_ptr<CEvent> &evPtr)
{
    return GsSelectionList::sendEvent(evPtr);
}

bool GsTextSelectionList::sendEvent(const InpCmd command)
{
    auto &controlsList = mScrollingFrame.getControlsList();

    if(command == IC_UP)
    {
        mFollowSelectedItem = true;
        auto it = controlsList.rbegin();
        auto lastSelected = controlsList.rend();

        // Find iterator of last selected item
        while(it != lastSelected)
        {
            auto ctrl = std::dynamic_pointer_cast<GsControl>(*it);

            assert(ctrl);

            if(ctrl->isSelected())
            {
                setSelection(-1);
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

            const auto numControls = int(controlsList.size());
            const auto dist = int(std::distance(controlsList.rbegin(), it));
            auto selNum = numControls - dist -1;

            // Last selection cannot be larger than number of elements
            selNum = std::min(selNum, numControls-1);
            setSelection(selNum);

            return true;
        }

        return ScrollingFrame().sendEvent(command);
    }
    else if(command == IC_DOWN)
    {
        mFollowSelectedItem = true;
        auto it = controlsList.begin();
        auto lastSelected = controlsList.end();

        // Find iterator of last selected item
        while(it != lastSelected)
        {
            auto ctrl = std::dynamic_pointer_cast<GsControl>(*it);

            assert(ctrl);

            if(ctrl->isSelected())
            {
                setSelection(-1);
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
            gEventManager.add(std::move(mConfirmEvent));
        if(mConfirmFunction)
            gEventManager.add(mConfirmFunction);
        return true;
    }
    else if(command == IC_BACK)
    {
        if(mBackEvent)
            gEventManager.add(std::move(mBackEvent));
        return true;
    }

    return false;
}

void GsTextSelectionList::addText(const std::string &text)
{
    const auto numElem = mItemList.size();

    mItemList.push_back( item(text) );

    const auto txtYPos = float(numElem)*0.1f;

    GsRect<float> frameRect(0.1f, txtYPos,
                            0.9f, 0.1f);

    auto selTextPtr = new GsSelectableText(frameRect, text);

    auto selectionFrame = mScrollingFrame.add( selTextPtr );

    selectionFrame->setParent(this);

    selectionFrame->setActivationEvent( [this]()
                                        {this->updateSelection();} );

    // Calculate the limits of the scroll area
    auto listRect = getRect();

    const auto scrollPosY = listRect.dim.y-txtYPos;

    mMinScrollY = std::min(scrollPosY, mMinScrollY);
    mMaxScrollY = 0.0f;

    mScrollbar.updateState(mScrollingFrame.getScrollY(),
                           mMinScrollY,
                           mMaxScrollY);
}


void GsTextSelectionList::updateSelection()
{
    auto &controlsList = mScrollingFrame.getControlsList();
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



const std::string &
GsTextSelectionList::getItemString(const unsigned int sel) const
{
    return mItemList[sel].mText;
}

void GsTextSelectionList::unselectAll()
{
    auto &controlsList = mScrollingFrame.getControlsList();
    for(auto &widget : controlsList)
    {
        auto ctrl = std::dynamic_pointer_cast<GsSelectableText>(widget);

        if(ctrl)
        {
            ctrl->select(false);
        }
    }
}
