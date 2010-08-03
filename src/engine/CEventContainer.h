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
	void add(const wm_event event_type);
	template< typename _DataType > void add(wm_event event_type, const _DataType data);
	bool occurredEvent( const wm_event event_type );
	template< typename _DataType > bool ReadData(_DataType &data);
	void pop_Event();

	virtual ~CEventContainer();

private:
	void add(const wm_event event_type, const void* data, size_t size);
	bool ReadData(void *data, const size_t size);

	std::list<CEvent> m_EventList;
};

/*
 * These are functions that use templates, they must be defined in the header
 */
template< typename _DataType >
void CEventContainer::add(const wm_event event_type, const _DataType data)
{	add(event_type, &data, sizeof(data));	}

template< typename _DataType >
bool CEventContainer::ReadData(_DataType &data)
{	return ReadData(&data, sizeof(data));	}

#endif /* CEVENTCONTAINER_H_ */
