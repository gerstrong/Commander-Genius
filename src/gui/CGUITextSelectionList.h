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
#include "CGUIControl.h"

class CGUITextSelectionList : public CGUIControl {
public:

	CGUITextSelectionList() :
	mHoverSelection(0),
	mSelection(0) {}

	void addText(const std::string &text);
	void processLogic();
	void processRender();

	std::list<std::string> mItemList;

	int mHoverSelection;
	int mSelection;
};

#endif /* CGUITEXTSELECTIONLIST_H_ */
