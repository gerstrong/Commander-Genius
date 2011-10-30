/*
 * CGUIControl.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  Base Class for any Type of Control
 */

#ifndef CGUICONTROL_H_
#define CGUICONTROL_H_

#include "utils/Geometry.h"

class CGUIControl
{
public:
	CGUIControl(const CRect &lRect) :
	mRect(lRect) {}

	virtual void processLogic() = 0;
	virtual void processRender() = 0;

	CRect mRect;
};


#endif /* CGUICONTROL_H_ */
