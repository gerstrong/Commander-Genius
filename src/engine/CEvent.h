/*
 * CEvent.h
 *
 *  Created on: 03.08.2010
 *      Author: gerstrong
 *
 *  Intendet to represent an event.
 *  Doesn't do any handling.
 *  TODO: If more events come and this file gets long we might have to split it up.
 */

#ifndef CEVENT_H_
#define CEVENT_H_

#include <stdint.h>
#include <string>
#include <list>
#include "common/direction.h"
#include "SmartPointer.h"
#include "CVec.h"

class CSpriteObject;


// TODO: These Event Interfaces must go to another file. Those are Inlevel Galaxy Events

struct CEvent { virtual ~CEvent() {} };

struct EventEnterLevel : CEvent {
	uint16_t data;
	EventEnterLevel(const uint16_t d) : data(d) {}
};


struct InvokeFunctorEvent : CEvent
{
	virtual void operator()() = 0;
};


struct EventExitLevel : CEvent {
	const uint16_t levelObject;
	const bool sucess;
	EventExitLevel(const uint16_t l, const bool s) : levelObject(l), sucess(s){}
};

struct EventRestartLevel : CEvent {

};


struct EventPlayerEndLevel : CEvent {
	const uint16_t levelObject;
	const bool sucess;
	EventPlayerEndLevel(const EventExitLevel ev) :
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
struct EventSendDialog : CEvent
{
	const std::string Msg;
	EventSendDialog(const std::string& lMsg) :
					 Msg(lMsg) {}
};

struct EventSendBitmapDialogMsg : EventSendDialog
{
	const uint16_t BitmapID;
	const direction_t Direction;
	EventSendBitmapDialogMsg(const uint16_t &lBitmapID,
					 	 	 const std::string& lMsg,
					 	 	 const direction_t& lDirection = LEFT) :
					 	 	 EventSendDialog(lMsg),
							 BitmapID(lBitmapID),
							 Direction(lDirection)
							 {}
};


/**
 *  \description small structure which holds a matching selection text to an event.
 *  			 It is used
 */
struct TextEventMatchOption
{
	std::string text;
	SmartPointer<CEvent> event;
};

/**
 *	\description This event triggers a MessageBox where you can select multiple items
 *
 *	\param		Message This Text will be shown when the Box is triggered
 *	\param 		OptionStrings The Text to the option which can be selected
 *							  Depending on the size of the
 */
struct EventSendSelectionDialogMsg : CEvent {

	const std::string Message;
	std::list<TextEventMatchOption> Options;

	EventSendSelectionDialogMsg(const std::string& lMsg) :
								Message(lMsg){}

	void addOption(const std::string& ltext, CEvent *levent)
	{
		TextEventMatchOption NewOption;
		NewOption.text = ltext;
		NewOption.event = levent;
		Options.push_back(NewOption);
	}
};


/**
 *	\description This event will spawn a new object
 *
 *	\param		pObject Pointer to the allocated memory of the Object. Caution: This allocation
 */
struct EventSpawnObject : CEvent {

	const CSpriteObject* pObject;

	EventSpawnObject( const CSpriteObject* pObject ) :
					  pObject(pObject) {}
};



#endif /* CEVENT_H_ */
