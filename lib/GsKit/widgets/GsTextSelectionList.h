/*
 * CGUITextSelectionList.h
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 *
 *  A Graphical List Control which has some item which are shown as Text and can be selected
 */

#ifndef CGUITEXTSELECTIONLIST_H_
#define CGUITEXTSELECTIONLIST_H_

#include "GsControl.h"
#include "GsScrollbar.h"

#include <list>
#include <string>


class CGUITextSelectionList : public CGUIControl
{
public:

    CGUITextSelectionList();

	void setConfirmButtonEvent(CEvent *ev);
	void setBackButtonEvent(CEvent *ev);

	bool sendEvent(const InputCommands command);
	void addText(const std::string &text);    
	void processLogic();
    void processRender(const GsRect<float> &RectDispCoordFloat);

    /**
     * @brief getSelection  return position of currently selected item, or -1 if nothing is selected at al
     * @return position or -1
     */
    int getSelection() const
    {
        return mReleasedSelection;
    }

    void setSelection(const int sel)
    {
        mReleasedSelection = sel;
    }

    const std::string &getItemString(const int sel) const
    {
        return mItemList[sel];
    }

private:

    std::vector<std::string> mItemList;

    int mHoverSelection;
    int mPressedSelection;
    int mReleasedSelection;
    
    GsScrollbar mScrollbar;
	
	std::shared_ptr<CEvent> mConfirmEvent;
	std::shared_ptr<CEvent> mBackEvent;
};

#endif /* CGUITEXTSELECTIONLIST_H_ */
