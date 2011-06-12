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
#include <list>
#include "common/direction.h"
#include "SmartPointer.h"

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


/**
 *	\description This event triggers a MessageBox where you can select multiple item
 *
 *	\param		Message This Text will be shown when the Box is triggered
 *	\param 		OptionStrings The Text to the option which can be selected
 *							  Depending on the size of the
 */
struct EventSendSelectionDialogMsg : CEvent {

	struct Option
	{
		std::string text;
		SmartPointer<CEvent> event;
	};

	const std::string Message;
	std::list<Option> Options;

	EventSendSelectionDialogMsg(const std::string& lMsg) :
								Message(lMsg){}

	void addOption(const std::string& ltext, CEvent *levent)
	{
		Option NewOption;
		NewOption.text = ltext;
		NewOption.event = levent;
		Options.push_back(NewOption);
	}
};



#endif /* CEVENT_H_ */
