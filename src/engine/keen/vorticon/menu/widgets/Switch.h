/*
 * CGUISwitch.h
 *
 *  Created on: 04.03.2012
 *      Author: gerstrong
 */

#include "ComboSelection.h"

#ifndef __VORTION_SWITCH_H__
#define __VORTION_SWITCH_H__

namespace vorticon
{


class Switch : public ComboSelection
{
public:
    Switch( const std::string& text );

	void enable( const bool sel )
    {	ComboSelection::setSelection( sel ? "on" : "off" );	}

	bool isEnabled()
    {	return ComboSelection::getSelection() == "on";	}

	void drawVorticonStyle(SDL_Rect& lRect);

	void processRender(const GsRect<float> &RectDispCoordFloat);
};

#endif /* __VORTION_SWITCH_H__ */
