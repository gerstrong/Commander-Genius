#include "GsSelectionList.h"

#include <base/video/CVideoDriver.h>
#include <base/GsLogging.h>

std::shared_ptr<GsSelectionList> createSelectionListFrom(const GsKit::ptree &node)
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



    std::shared_ptr<GsSelectionList> w
        (new GsSelectionList( dim ));

    w->setTag(tag);

    return w;
}



GsSelectionList::
GsSelectionList(const GsRect<float> &rect) :
GsFrame(rect),
mScrollingFrame(rect, rect),
mScrollbar(rect)
{
    const GsRect<float> scrollRect(0.0f, 0.0f, 0.1f, 1.0f);
    const GsRect<float> frameRect(0.1f, 0.0f, 0.9f, 1.0f);

    mScrollbar.setRect(scrollRect);

    mScrollbar.setScrollUpFn([this]
                               {
                                mFollowSelectedItem = false;
                                mScrollingFrame.moveY(0.05f);
                                this->updateScrollBar();
                               });
    mScrollbar.setScrollDownFn([this]
                               {
                                mFollowSelectedItem = false;
                                mScrollingFrame.moveY(-0.05f);
                                this->updateScrollBar();
                               });

    mScrollbar.setBackgroundColor(GsColor(0x7F, 0x7F, 0x7F));

    mScrollingFrame.setRect(frameRect);
}

void GsSelectionList::fitWithHeight(const float padding,
                                    const float width,
                                    const float height)
{
    auto &selFrame = ScrollingFrame();
    selFrame.fitWithHeight(padding, width, height);

    auto &wList = selFrame.getControlsList();

    if(wList.empty())
        return;

    float maxY = 0.0f;

    for( auto &item : wList )
    {
        const auto y2 = item->getRect().pos.y + item->getRect().dim.y;
        maxY = std::max(y2, maxY);
    }

    const auto scrollPosY = maxY;
    mMinScrollY = std::min(-(scrollPosY-1.0f), mMinScrollY);
    mMaxScrollY = 0.0f;

    mScrollbar.updateState(mScrollingFrame.getScrollY(),
                           mMinScrollY,
                           mMaxScrollY);
}

void GsSelectionList::updateScrollBar()
{
    mScrollbar.updateState(mScrollingFrame.getScrollY(),
                           mMinScrollY,
                           mMaxScrollY);
}

void GsSelectionList::updateSelection()
{
}

void GsSelectionList::processLogic()
{
    if(!mEnabled)
    {
        return;
    }

    mScrollingFrame.processLogic();
    mScrollbar.processLogic();

    auto &controlsList = getControlsList();
    for(auto &widget : controlsList)
    {
        if(mFollowSelectedItem)
        {
            if( auto frame =
                    std::dynamic_pointer_cast<GsFrame>(widget) )
            {
                if( frame->isSelected() )
                {
                    auto bound = frame->getOuterbound();

                    if(bound.y > 0.0f)
                    {
                        mScrollingFrame.moveY(-0.05f);
                        this->updateScrollBar();
                    }
                    else if(bound.y < 0.0f)
                    {
                        mScrollingFrame.moveY(+0.05f);
                        this->updateScrollBar();
                    }
                }
            }
        }

        widget->processLogic();
    }
}

bool GsSelectionList::sendEvent(const std::shared_ptr<CEvent> &evPtr)
{
    // Check Scroll events happening on this Launcher
    if( const MouseWheelEvent *mwe = dynamic_cast<const MouseWheelEvent*>(evPtr.get()) )
    {
        // Wrapper for the simple mouse scroll event
        if(mwe->amount.y > 0.0f)
        {
            mFollowSelectedItem = false;
            mScrollingFrame.moveY(-0.05f);
            this->updateScrollBar();
            return true;
        }
        else if(mwe->amount.y < 0.0f)
        {
            mFollowSelectedItem = false;
            mScrollingFrame.moveY(0.05f);
            this->updateScrollBar();
            return true;
        }
        /*if(mwe->amount.x < 0.0f)
        {
            selectNextItem();
            return true;
        }
        else if(mwe->amount.x > 0.0f)
        {
            selectPrevItem();
            return true;
        }*/
    }

    return true;
}

void GsSelectionList::processPointingStateRel(const GsRect<float> &rect)
{
    auto absRect = rect.transformed(getRect());
    mScrollingFrame.processPointingStateRel(absRect);
    mScrollbar.processPointingStateRel(absRect);
}

std::vector<std::string> GsSelectionList::getStringList()
{
    std::vector<std::string> strVec;

    auto &controlsList = ScrollingFrame().getControlsList();
    for(auto &widget : controlsList)
    {
        if( auto button =
                std::dynamic_pointer_cast<GsButton>(widget) )
        {
            strVec.push_back(button->getText());
        }
    }
    return strVec;
}

void GsSelectionList::setBackgroundColor(const GsColor &color)
{
    mScrollingFrame.setBackgroundColor(color);
    mBackgroundColor = color;
}


void GsSelectionList::processRenderSimple(const GsRect<float> &RectDispCoordFloat)
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

void GsSelectionList::processRenderTTF(const GsRect<float> &RectDispCoordFloat)
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



void GsSelectionList::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> scrollRect = getRect();
    scrollRect.transform(RectDispCoordFloat);

    mScrollbar.processRender(scrollRect);
    mScrollingFrame.processRender(scrollRect);
}

