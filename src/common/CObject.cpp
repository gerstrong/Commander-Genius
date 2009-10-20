/*
 * CObject.cpp
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 */

#include "CObject.h"
#include <string.h>

CObject::CObject() {
	honorPriority=false;
	exists = false;
	memset(cansupportplayer, false, MAX_PLAYERS*sizeof(bool));
}

CObject::~CObject() {
	// TODO Auto-generated destructor stub
}

