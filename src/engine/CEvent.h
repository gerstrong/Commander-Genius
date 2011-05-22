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

struct CEvent { virtual ~CEvent() {} };

struct EventEnterLevel : CEvent {
	uint16_t data;
	EventEnterLevel(uint16_t d) : data(d) {}
};

struct EventExitLevel : CEvent {
	uint16_t levelObject;
	bool sucess;
	EventExitLevel(uint16_t l, const bool s) : levelObject(l), sucess(s){}
};


/**
 *	Event designated for a Bitmap Message like those when Keen talks
 *	to the council Members or Princess Lindsey
 */
struct EventSendBitmapDialogMsg : CEvent {
	uint16_t BitmapID;
	std::string Msg;
	EventSendBitmapDialogMsg(const uint16_t &lBitmapID,
					 	 	 const std::string& lMsg) :
							 BitmapID(lBitmapID),
							 Msg(lMsg) {}
};


#endif /* CEVENT_H_ */
