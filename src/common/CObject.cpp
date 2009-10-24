/*
 * CObject.cpp
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 */

#include "CObject.h"
#include <string.h>

///
// Initialization Routine
///
CObject::CObject() {
	honorPriority = false;
	exists = false;
	memset(cansupportplayer, false, MAX_PLAYERS*sizeof(bool));
}

void CObject::setPos( int px, int py )
{
	scrx = px;
	scry = py;
}

///
// Cleanup Routine
///
CObject::~CObject() {
	// TODO Auto-generated destructor stub
}

