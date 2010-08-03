/*
 * CEventContainer.h
 *
 *  Created on: 03.08.2010
 *      Author: gerstrong
 *
 * This will store a list of events
 */

#ifndef CEVENTCONTAINER_H_
#define CEVENTCONTAINER_H_

#include "CEvent.h"
#include <list>

class CEventContainer {
public:
	CEventContainer();

	bool empty();
	void add(wm_event event_type, void* data, size_t size);
	bool occurredEvent( wm_event event_type );
	bool ReadData(void *data, size_t size);
	void pop_Event();

	virtual ~CEventContainer();

private:
	std::list<CEvent> m_EventList;
};

#endif /* CEVENTCONTAINER_H_ */
