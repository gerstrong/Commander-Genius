#ifndef GSEVENTCONTAINER_H
#define GSEVENTCONTAINER_H

#include "GsEvent.h"
#include <vector>
#include <deque>
#include <list>
#include <memory>
#include <ctime>

#define gEventManager CEventContainer::get()

class CEventContainer : public GsSingleton<CEventContainer>
{
public:

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
        mSinkPtrList.remove(pSink);
    }

    /**
     * @brief processSinks processor of the added event within the applications
     */
    void processSinks();

    void flush()
    {
        m_EventList.clear();
        mFlush = true;
    }

    void add(std::shared_ptr<CEvent>& ev)
    {
        m_EventList.push_back(ev);
    }

    // template version
    template <class _T>
    void add()
    {
        _T *ev = new _T;
        m_EventList.push_back(std::shared_ptr<CEvent>(ev));
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

    void update();

    std::deque< std::shared_ptr<CEvent> >::iterator erase(std::deque< std::shared_ptr<CEvent> >::iterator &it)
    {	return m_EventList.erase(it);	}
    std::deque< std::shared_ptr<CEvent> >::iterator begin() { return m_EventList.begin(); }
    std::deque< std::shared_ptr<CEvent> >::iterator end() { return m_EventList.end(); }
    template<typename T> T* occurredEvent();
    void pop_Event() { m_EventList.pop_front(); }

private:
    std::list< GsEventSink* > mSinkPtrList;

    std::deque< std::shared_ptr<CEvent> > m_EventList;
    std::vector< std::shared_ptr<CEvent> > mPumpEventPtrs;

    clock_t pausetime = 0;
    clock_t timepoint = 0;

    bool mFlush = false;
};

template<typename T>
T* CEventContainer::occurredEvent()
{
    if(m_EventList.empty() || pausetime > 0 )
        return nullptr;

    return dynamic_cast<T*> (m_EventList.front().get());
}


#endif /* GSEVENTCONTAINER_H */
