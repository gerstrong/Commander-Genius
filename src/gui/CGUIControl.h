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
	CGUIControl();

	virtual void processLogic() = 0;
	virtual void processRender(const CRect<float> &RectDispCoordFloat) = 0;

	CRect<float> mRect;
};


#endif /* CGUICONTROL_H_ */
