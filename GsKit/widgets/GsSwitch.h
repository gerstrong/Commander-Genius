/*
 * CGUISwitch.h
 *
 *  Created on: 04.03.2012
 *      Author: gerstrong
 */

#include "GsComboSelection.h"
//#include "Utils.h"

#ifndef CGUISWITCH_H_
#define CGUISWITCH_H_

class CGUISwitch : public CGUIComboSelection
{
public:
	CGUISwitch( const std::string& text );

	void enable( const bool sel )
	{	CGUIComboSelection::setSelection( sel ? "on" : "off" );	}

	bool isEnabled()
	{	return CGUIComboSelection::getSelection() == "on";	}

	void drawVorticonStyle(SDL_Rect& lRect);

	void processRender(const GsRect<float> &RectDispCoordFloat);
};

#endif /* CGUISWITCH_H_ */
