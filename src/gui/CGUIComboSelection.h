/*
 * CGUIComboSelection.h
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#ifndef CGUICOMBOSELECTION_H_
#define CGUICOMBOSELECTION_H_

#include "CGUIControl.h"
#include <string>
#include "engine/CEvent.h"
#include "SmartPointer.h"


class CGUIComboSelection : public CGUIControl
{
public:

	enum Style
	{
		NONE,
		VORTICON
	};

	CGUIComboSelection(	const std::string& text,
			const std::list<std::string>& optionsList,
			const Style	style = NONE);

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);


	void processRender(const CRect<float> &RectDispCoordFloat);

private:
	bool mHovered;
	bool mButtonDown;
	bool mButtonUp;
	std::string mText;
	static int mTwirliconID;
	std::list<std::string> mOptionsList;
	std::list<std::string>::const_iterator mOLCurrent;

	void (CGUIComboSelection::*drawButton)(SDL_Rect&);

};

#endif /* CGUICOMBOSELECTION_H_ */
