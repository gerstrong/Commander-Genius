/*
 * InputEvents.h
 *
 *  Created on: 20.11.2011
 *      Author: gerstrong
 *
 *  Input Events are defined here, for now we only have events for the mouse,
 *  but more stuff will come.
 */

#include "GsEvent.h"
#include "Vector2D.h"

#ifndef INPUTEVENTS_H_
#define INPUTEVENTS_H_


enum InpCmd
{
	IC_LEFT = 0,
	IC_RIGHT,
	IC_UP,
	IC_DOWN,
	IC_UPPERLEFT,
	IC_UPPERRIGHT,
	IC_LOWERLEFT,
	IC_LOWERRIGHT,
	IC_JUMP,
	IC_POGO,
	IC_FIRE,
    IC_RUN,
	IC_STATUS,
	IC_CAMLEAD,
	IC_HELP,
    IC_BACK,
    IC_QUICKSAVE,
    IC_QUICKLOAD,
    MAX_COMMANDS
};

const int MID_COMMANDS_OFFSETS = 8;

enum PointingDevEventTypes
{
    PDE_MOVED = 0,
    PDE_BUTTONDOWN,
    PDE_BUTTONUP
};

struct PointingDevEvent : CEvent
{
    GsVec2D<float> Pos;
    PointingDevEventTypes Type;
    PointingDevEvent(const GsVec2D<float> &New_Pos, PointingDevEventTypes New_Type ) :
		Pos(New_Pos), Type(New_Type) {}
};


struct MouseWheelEvent : CEvent
{
    GsVec2D<float> amount;
    MouseWheelEvent(const GsVec2D<float> &newAmount ) :
        amount(newAmount) {}
};


struct CommandEvent : public CEvent
{
    CommandEvent(const InpCmd command) :
		mCommand(command) {}

    InpCmd mCommand;
};



#endif /* INPUTEVENTS_H_ */
