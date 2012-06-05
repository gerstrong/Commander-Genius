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
#include "SmartPointer.h"
#include <list>


class CEventContainer
{
public:

	size_t size() { return m_EventList.size(); }
	bool empty() { return m_EventList.empty(); }
	void clear() { m_EventList.clear(); }
	void add(const SmartPointer<CEvent>& ev) { m_EventList.push_back(ev); }
	std::list< SmartPointer<CEvent> >::iterator erase(std::list< SmartPointer<CEvent> >::iterator &it)
	{	return m_EventList.erase(it);	}
	std::list< SmartPointer<CEvent> >::iterator begin() { return m_EventList.begin(); }
	std::list< SmartPointer<CEvent> >::iterator end() { return m_EventList.end(); }
	template<typename T> T* occurredEvent();
	void pop_Event() { m_EventList.pop_front(); }

private:
	std::list< SmartPointer<CEvent> > m_EventList;
};

template<typename T>
T* CEventContainer::occurredEvent()
{
	if(m_EventList.empty())
		return NULL;

	return dynamic_cast<T*> (m_EventList.front().get());
}


#endif /* CEVENTCONTAINER_H_ */
