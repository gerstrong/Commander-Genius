#ifndef GSEVENT_H
#define GSEVENT_H

#include "base/Singleton.h"

struct CEvent { virtual ~CEvent(); };

struct InvokeFunctorEvent : CEvent
{
    virtual void operator()() const = 0;
};


/**
 * @brief The GsEventSink derived objects are those which receive the added event to the container
 */
class GsEventSink
{
public:

    virtual ~GsEventSink();

    virtual void pumpEvent(const CEvent *ev) = 0;
};




#endif /* GSEVENT_H */
