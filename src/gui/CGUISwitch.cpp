/*
 * CGUISwitch.cpp
 *
 *  Created on: 04.03.2012
 *      Author: gerstrong
 */

#include "CGUISwitch.h"
#include "StringUtils.h"

CGUISwitch::CGUISwitch( const std::string& text ) :
CGUIComboSelection( text, filledStrList( 2, "off", "on" ) )
{}
