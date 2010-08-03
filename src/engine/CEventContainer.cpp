/*
 * CEventContainer.cpp
 *
 *  Created on: 03.08.2010
 *      Author: gerstrong
 */

#include "CEventContainer.h"
#include <string.h>

CEventContainer::CEventContainer() {
	// TODO Auto-generated constructor stub

}

bool CEventContainer::empty()
{	return m_EventList.empty();	}


/**
 * Add a new event. This function formats it to the CEvent class
 */
void CEventContainer::add(const wm_event event_type)
{	add(event_type, NULL, 0);	}
void CEventContainer::add(const wm_event event_type, const void *data, const size_t size)
{
	CEvent Event(event_type, data, size);
	m_EventList.push_front(Event);
}

/**
 * Checks for the next occurred event
 */
bool CEventContainer::occurredEvent( const wm_event event_type )
{
	if(m_EventList.empty()) return false;
	CEvent &Event = m_EventList.front();
	return Event.occurred(event_type);
}

/**
 * Reads the data in case the event had some of it attached
 */
bool CEventContainer::ReadData(void *data, const size_t size)
{
	CEvent &Event = m_EventList.front();
	if(size != Event.Size() || Event.getData() == NULL)
		return false;

	memcpy(data, Event.getData(), size);
	return true;
}

/**
 *  Just removes the Event that was used
 */
void CEventContainer::pop_Event()
{	m_EventList.pop_front();	}

CEventContainer::~CEventContainer() {
	// TODO Auto-generated destructor stub
}
