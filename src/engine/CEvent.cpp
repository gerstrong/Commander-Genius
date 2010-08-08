/*
 * CEvent.cpp
 *
 *  Created on: 03.08.2010
 *      Author: gerstrong
 */

#include "CEvent.h"
#include "fileio/TypeDefinitions.h"

/**
 * \brief This constructor sets up the Event
 *		  with it's event type and data if necessary
 * \param eventtype the type of the event that is to be designated
 * \param data		if the event should also carry additional data, here they will be stored
 */
CEvent::CEvent(const wm_event event_type, const void *data, const size_t size) :
event_type(event_type)
{
	if(size != 0)
		this->data = std::vector<char>((char*)data, (char*)data + size);
}

/**
 * This function just looks if it is the same event we are looking for and removes it, because
 * in case it was detected
 */
bool CEvent::occurred( wm_event event_type)
{	return (this->event_type == event_type); }
