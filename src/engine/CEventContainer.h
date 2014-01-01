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
#include <deque>
#include <vector>
#include <ctime>

/**
 * @brief The GsEventSink derived objects are those which receive the added event to the container
 */
class GsEventSink
{
public:
    virtual void pumpEvent(const CEvent *ev) = 0;
};


class CEventContainer
{
public:
    
    CEventContainer() :
        pausetime(0),
        timepoint(0)
    {}

	size_t size() { return m_EventList.size(); }
	bool empty() { return m_EventList.empty(); }
	void clear() { m_EventList.clear(); }

    /**
     * @brief regSink will register the sink in the Container
     *        when you are destroying the object, you must call unregSink() first!
     *        Otherwise app might crash.
     * @param pSink pointer to the sink to be registered
     */
    void regSink(GsEventSink *pSink)
    {
        mSinkPtrList.push_back(pSink);
    }

    void unregSink(GsEventSink *pSink)
    {
        // TODO: Code for unregistering the Sink
    }

    void processSinks()
    {
        // First check if there are pendingEvents to be processed
        if(m_EventList.empty())
            return;

        // Make a copy of that list, because the original
        // could change, while pumping happens
        for(auto &ev : m_EventList)
            mPumpEventPtrs.push_back(ev);

        // We don't need anything from this list anymore
        m_EventList.clear();

        for( GsEventSink* sink : mSinkPtrList )
        {
            for( auto &event : mPumpEventPtrs )
            {
                sink->pumpEvent( event.get() );
            }
        }

        mPumpEventPtrs.clear();
    }

	void add(std::shared_ptr<CEvent>& ev) 
	{ 
        m_EventList.push_back(ev);
	}
	
	void add(CEvent *ev) 
	{ 
	    m_EventList.push_back(std::shared_ptr<CEvent>(ev)); 	    
	}
	
	void wait(const float time)
	{
	    timepoint = clock();
	    pausetime = static_cast<clock_t>(time*static_cast<float>(CLOCKS_PER_SEC));
	}
	
	void update()
	{
	    if(pausetime<=0.0f)
		return;
	    	    
	    const clock_t newTime = clock();
	    const clock_t passed = newTime-timepoint;
	    timepoint = newTime;
	    pausetime -= passed;
	    return;
	}
	
	std::deque< std::shared_ptr<CEvent> >::iterator erase(std::deque< std::shared_ptr<CEvent> >::iterator &it)
	{	return m_EventList.erase(it);	}
	std::deque< std::shared_ptr<CEvent> >::iterator begin() { return m_EventList.begin(); }
	std::deque< std::shared_ptr<CEvent> >::iterator end() { return m_EventList.end(); }
	template<typename T> T* occurredEvent();
	void pop_Event() { m_EventList.pop_front(); }

    // Spawning Event for the Foes // TODO: Workaround. I think it should go into another section
	void spawnObj(const CSpriteObject *obj)
	{
	    add(new EventSpawnObject( obj ));
	}
	
private:
    std::list< GsEventSink* > mSinkPtrList;

	std::deque< std::shared_ptr<CEvent> > m_EventList;
    std::vector< std::shared_ptr<CEvent> > mPumpEventPtrs;

	clock_t pausetime;
	clock_t timepoint;
};

template<typename T>
T* CEventContainer::occurredEvent()
{
	if(m_EventList.empty() || pausetime > 0 )
		return NULL;

	return dynamic_cast<T*> (m_EventList.front().get());
}


#endif /* CEVENTCONTAINER_H_ */
