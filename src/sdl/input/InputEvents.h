/*
 * InputEvents.h
 *
 *  Created on: 20.11.2011
 *      Author: gerstrong
 *
 *  Input Events are defined here, for now we only have events for the mouse,
 *  but more stuff will come.
 */

#include "engine/CEvent.h"
#include "CVec.h"

#ifndef INPUTEVENTS_H_
#define INPUTEVENTS_H_

enum MouseEventTypes
{
	MOUSEEVENT_MOVED = 0,
	MOUSEEVENT_BUTTONDOWN,
	MOUSEEVENT_BUTTONUP
};

struct MouseMoveEvent : CEvent
{
	CVec Pos;
	MouseEventTypes Type;
	MouseMoveEvent(const CVec &New_Pos, MouseEventTypes New_Type ) :
		Pos(New_Pos), Type(New_Type) {}
};



#endif /* INPUTEVENTS_H_ */
