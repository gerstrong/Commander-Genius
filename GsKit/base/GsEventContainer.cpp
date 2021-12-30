#include "GsEventContainer.h"

#include <functional>
#include <string>
#include <map>

void
CEventContainer::bind(const std::string &name,
                       const std::function<CEvent*()> &fun)
{
    mCreateEventFrom[name] = fun;
}

void CEventContainer::add(const std::string &evName)
{
    std::shared_ptr<CEvent> sharedEvent(
                mCreateEventFrom[evName]());
    m_EventList.push_back(sharedEvent);
}

void CEventContainer::add(CEvent *ev)
{
    std::shared_ptr<CEvent> sharedEvent(ev);
    m_EventList.push_back(sharedEvent);
}


void CEventContainer::processSinks()
{
    // First check if there are pendingEvents to be processed
    if(m_EventList.empty())
        return;

    // Make a copy of that list, because the original
    // could change, while pumping happens
    // Ticks are converted to milliseconds here.
    for(auto &ev : m_EventList)
    {
        if(!ev)
            continue;

        mPumpEventPtrs.push_back(ev);
    }

    // We don't need anything from this list anymore
    m_EventList.clear();

    for( auto sink : mSinkPtrList )
    {
        for( auto &event : mPumpEventPtrs )
        {
            sink->pumpEvent( event.get() );

            if(mFlush)
                break;
        }

        if(mFlush)
            break;
    }

    mPumpEventPtrs.clear();

    mFlush = false;
}


void CEventContainer::update()
{
    if(pausetime<=0.0f)
        return;

    const clock_t newTime = clock();
    const clock_t passed = newTime-timepoint;
    timepoint = newTime;
    pausetime -= passed;
        return;
}
