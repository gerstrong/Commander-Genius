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

struct MouseMoveEvent : CEvent
{
	CVec Pos;
	MouseMoveEvent(const CVec &New_Pos) : Pos(New_Pos) {}
};



#endif /* INPUTEVENTS_H_ */
