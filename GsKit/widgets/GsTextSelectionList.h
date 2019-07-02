/*
 * CGUITextSelectionList.h
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 *
 *  A Graphical List Control which has some item which are shown as Text and can be selected
 */

#ifndef CGUITEXTSELECTIONLIST_H
#define CGUITEXTSELECTIONLIST_H

#include "GsScrollingFrame.h"
#include "GsScrollbar.h"
#include "GsText.h"
#include <base/utils/Color.h>
#include <graphics/GsSurface.h>

#include <vector>
#include <string>
#include <map>
#include <memory>

#if defined(USE_SDL_TTF)
#include <graphics/GsTrueTypeFont.h>
#endif



class CGUITextSelectionList : public GsScrollingFrame
{
public:

    CGUITextSelectionList(const GsRect<float> &rect);

	void setConfirmButtonEvent(CEvent *ev);
	void setBackButtonEvent(CEvent *ev);

    bool sendEvent(const InputCommand command) override;
	void addText(const std::string &text);    

    void updateSelection();

    void processLogic() override;

    void processRenderSimple(const GsRect<float> &RectDispCoordFloat);
    void processRenderTTF(const GsRect<float> &RectDispCoordFloat);
    void processRender(const GsRect<float> &RectDispCoordFloat) override;


    virtual void processPointingStateRel(const GsRect<float> &rect) override;

    const std::string &getItemString(const unsigned int sel) const;

    void unselectAll();

private:

    struct item
    {
        item(const std::string &text) : mText(text) {}

        std::string mText; // Text of the current item
        GsColor mBgColor = GsColor(0xFF, 0xFF, 0xFF, 0xFF);  // Current background color
    };

    std::vector<item> mItemList;

    GsScrollbar mScrollbar;
	
	std::shared_ptr<CEvent> mConfirmEvent;
	std::shared_ptr<CEvent> mBackEvent;
};

#endif /* CGUITEXTSELECTIONLIST_H */
