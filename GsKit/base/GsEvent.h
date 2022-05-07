#ifndef GSEVENT_H
#define GSEVENT_H

#include <functional>
#include <memory>

struct CEvent
{
    virtual ~CEvent();
};

struct GsFunctor
{
    virtual void operator()() = 0;
};

struct FunctionToEvent : CEvent
{
public:
    FunctionToEvent(const std::function<void()> &func):
        mFunction(func) {}

    void runFunction() const
    {  mFunction();  }

private:

    std::function<void()> mFunction;
};


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

    virtual void pumpEvent(const std::shared_ptr<CEvent> &evPtr) = 0;
};




#endif /* GSEVENT_H */
