/*
 * CEvent.h
 *
 *  Created on: 03.08.2010
 *      Author: gerstrong
 *
 *  This is a new Event class.
 *  The events which are desired
 *  to be used in some section of the
 *  code must be declared through a enum
 *  NOTE: I know there is that other class by Albert
 *  I would like to use, but it seems incomplete and
 *  too overloaded for what we need. We might join it later!!
 */

#ifndef CEVENT_H_
#define CEVENT_H_

#include <list>

enum wm_event{
	ENTER_LEVEL,
	EXIT_LEVEL
};


class CEvent {
public:
	CEvent(const wm_event event_type, const void *data, const size_t size);
	CEvent(const CEvent& Event);

	bool occurred(wm_event event_type);

	wm_event getEvent();
	void *getData();
	size_t Size();

	virtual ~CEvent();

private:
	wm_event event_type;
	void *data;
	size_t size;
};


#endif /* CEVENT_H_ */
