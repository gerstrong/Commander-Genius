/*
 * CGUIComboSelection.h
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#ifndef CGUICOMBOSELECTION_H_
#define CGUICOMBOSELECTION_H_

#include <string>
#include "CGUIControl.h"
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

	const std::string& getSelection();

	void setSelection( const std::string& selectionText );

	void setList(const char **strArray, const int numElem);

	void processLogic();

	void drawNoStyle(SDL_Rect& lRect);

	void drawVorticonStyle(SDL_Rect& lRect);


	void processRender(const CRect<float> &RectDispCoordFloat);

private:
	std::string mText;
	std::list<std::string> mOptionsList;
	std::list<std::string>::const_iterator mOLCurrent;

	void (CGUIComboSelection::*drawButton)(SDL_Rect&);

};

#endif /* CGUICOMBOSELECTION_H_ */
