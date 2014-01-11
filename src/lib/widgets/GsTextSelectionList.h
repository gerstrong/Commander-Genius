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

#include <list>
#include <string>
#include "GsControl.h"

class CGUITextSelectionList : public CGUIControl
{
public:

	CGUITextSelectionList() :
	mHoverSelection(0),
    mPressedSelection(-1),
    mReleasedSelection(-1) {}

	void setConfirmButtonEvent(CEvent *ev);
	void setBackButtonEvent(CEvent *ev);

	bool sendEvent(const InputCommands command);
	void addText(const std::string &text);
	void processLogic();
    void processRender(const GsRect<float> &RectDispCoordFloat);

    int getSelection() const
    { return mReleasedSelection; }

    void setSelection(const int sel)
    { mReleasedSelection = sel; }

	std::list<std::string> mItemList;
	
private:

    int mHoverSelection;
    int mPressedSelection;
    int mReleasedSelection;

    
	int mTextWidthLimit;
	
	std::shared_ptr<CEvent> mConfirmEvent;
	std::shared_ptr<CEvent> mBackEvent;
};

#endif /* CGUITEXTSELECTIONLIST_H_ */
