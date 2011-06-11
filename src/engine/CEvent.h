/*
 * CEvent.h
 *
 *  Created on: 03.08.2010
 *      Author: gerstrong
 *
 *  Intendet to represent an event.
 *  Doesn't do any handling.
 */

#ifndef CEVENT_H_
#define CEVENT_H_

#include <stdint.h>
#include <string>
#include "common/direction.h"

struct CEvent { virtual ~CEvent() {} };

struct EventEnterLevel : CEvent {
	uint16_t data;
	EventEnterLevel(const uint16_t d) : data(d) {}
};

struct EventExitLevel : CEvent {
	const uint16_t levelObject;
	const bool sucess;
	EventExitLevel(const uint16_t l, const bool s) : levelObject(l), sucess(s){}
};

struct EventPlayerEndLevel : CEvent {
	const uint16_t levelObject;
	const bool sucess;
	EventPlayerEndLevel(const EventExitLevel &ev) :
					levelObject(ev.levelObject), sucess(ev.sucess) {}
};


struct EventPlayTrack : CEvent {
	const uint32_t track;
	EventPlayTrack(const uint16_t t) : track(t) {}
};


/**
 *	Event designated for a Bitmap Message like those when Keen talks
 *	to the council Members or Princess Lindsey
 */
struct EventSendBitmapDialogMsg : CEvent {
	const uint16_t BitmapID;
	const std::string Msg;
	const direction_t Direction;
	EventSendBitmapDialogMsg(const uint16_t &lBitmapID,
					 	 	 const std::string& lMsg,
					 	 	 const direction_t& lDirection = LEFT) :
							 BitmapID(lBitmapID),
							 Msg(lMsg),
							 Direction(lDirection)
							 {}
};


#endif /* CEVENT_H_ */
