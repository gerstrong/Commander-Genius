/*
 * CGUISwitch.h
 *
 *  Created on: 04.03.2012
 *      Author: gerstrong
 */

#include "CGUIComboSelection.h"
#include "Utils.h"

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
};

#endif /* CGUISWITCH_H_ */
