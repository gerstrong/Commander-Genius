#include "GsEventContainer.h"

void CEventContainer::processSinks()
{
    // First check if there are pendingEvents to be processed
    if(m_EventList.empty())
        return;

    // Make a copy of that list, because the original
    // could change, while pumping happens
    for(auto &ev : m_EventList)
    {
        mPumpEventPtrs.push_back(ev);
    }

    // We don't need anything from this list anymore
    m_EventList.clear();

    for( GsEventSink* sink : mSinkPtrList )
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
