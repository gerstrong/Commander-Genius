#ifndef GSSELECTIONLIST_H
#define GSSELECTIONLIST_H

#include "GsScrollingFrame.h"

#include "GsScrollingFrame.h"
#include "GsScrollbar.h"


class GsSelectionList : public GsFrame
{
public:

    GsSelectionList(const GsRect<float> &rect);

    void fitWithHeight(const float padding,
                       const float width,
                       const float height) override;

    void updateScrollBar();

    virtual void updateSelection();

    void processLogic() override;

    void processRenderSimple(const GsRect<float> &RectDispCoordFloat);
    void processRenderTTF(const GsRect<float> &RectDispCoordFloat);
    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    virtual bool sendEvent(const std::shared_ptr<CEvent> &evPtr) override;


    virtual void processPointingStateRel(const GsRect<float> &rect) override;

    virtual void setBackgroundColor(const GsColor &color) override;

    GsScrollingFrame &ScrollingFrame()
    {
        return mScrollingFrame;
    }

    /**
     * @brief getStringList
     * @return Returns a full list of entries of the list control
     */
    std::vector<std::string> getStringList();

protected:

    GsScrollingFrame mScrollingFrame;
    GsScrollbar mScrollbar;

    float mMaxScrollY = 0.0f;
    float mMinScrollY = 0.0f;

    std::unique_ptr<CEvent> mConfirmEvent;
    std::unique_ptr<CEvent> mBackEvent;

    std::function <void ()> mConfirmFunction;
    std::function <void ()> mBackFunction;

    // If enabled, the scroll position follows the selected item.
    // (Useful when browsing the list using keys)
    bool mFollowSelectedItem = false;
};

#endif // GSSELECTIONLIST_H

std::shared_ptr<GsSelectionList> createSelectionListFrom(const GsKit::ptree &node);
