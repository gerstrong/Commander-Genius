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
//#include "SmartPointer.h"
#include <list>

class CEventContainer {
public:
	bool empty() { return m_EventList.empty(); }
	//void add(const SmartPointer<CEvent>& ev) { m_EventList.push_back(ev); }
	void add(CEvent *ev) { m_EventList.push_back(ev); }
	template<typename T> T* occurredEvent();
	void pop_Event()
	{
		delete m_EventList.front();
		m_EventList.pop_front();
	}

	~CEventContainer()
	{
		while(!m_EventList.empty())
			m_EventList.pop_front();
	}

private:
	std::list< CEvent* > m_EventList;
};

template<typename T>
T* CEventContainer::occurredEvent() {
	if(m_EventList.empty()) return NULL;
	//return dynamic_cast<T*> (m_EventList.front().get());
	return dynamic_cast<T*> (m_EventList.front());
}


#endif /* CEVENTCONTAINER_H_ */
