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

#include <cstdint>
#include <string>
#include <list>
#include "common/direction.h"
//#include <base/utils/CVec.h>
#include "graphics/GsBitmap.h"
#include <vector>
#include <memory>

#include <base/GsEvent.h>

class CSpriteObject;





/*struct EventSendBitmapDialogMsg : EventSendDialog
{
	const GsBitmap &BitmapRef;
	const direction_t Direction;
	EventSendBitmapDialogMsg(const GsBitmap &lBitmapRef,
				 const std::string& lMsg,
				 const direction_t& lDirection = LEFT) :
				  EventSendDialog(lMsg),
				  BitmapRef(lBitmapRef),
				  Direction(lDirection)
				  {}
};
*/

/**
 * This event collects a vector of EventSendBitmapDialogMsg event, so all
 * the message boxes are loading at once and display one by one holding the paused game
 */

/*struct EventSendBitmapDialogMessages : CEvent
{
	EventSendBitmapDialogMessages( std::vector< std::shared_ptr<EventSendBitmapDialogMsg> > lMsgs ) :
		msgs(lMsgs) {}

	std::vector< std::shared_ptr<EventSendBitmapDialogMsg> > msgs;
};*/




/**
 *  \description small structure which holds a matching selection text to an event.
 *  			 It is used
 */
struct TextEventMatchOption
{
	std::string text;
	std::shared_ptr<CEvent> event;
};





#endif /* CEVENT_H_ */
