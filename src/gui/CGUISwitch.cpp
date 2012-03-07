/*
 * CGUISwitch.cpp
 *
 *  Created on: 04.03.2012
 *      Author: gerstrong
 */

#include "CGUISwitch.h"

CGUISwitch::CGUISwitch(	const std::string& text,
						const Style	style ) :
CGUIComboSelection( text, filledStrList( 2, "off", "on" ), style )
{}
