/*
 * CGUIText.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  This is the text control for our new GUI
 */

#ifndef CGUIBUTTON_H_
#define CGUIBUTTON_H_

#include "CGUIControl.h"
#include <string>
#include "engine/CEvent.h"
#include "SmartPointer.h"

class CGUIButton : public CGUIControl
{
public:
	CGUIButton(const std::string& text, const SmartPointer<CEvent> ev);

	void processLogic();
	void processRender(const CRect<float> &RectDispCoordFloat);

private:
	bool mHovered;
	bool mButtonDown;
	bool mButtonUp;
	std::string mText;
	SmartPointer<CEvent> mEvent;
};

#endif /* CGUIBUTTON_H_ */
