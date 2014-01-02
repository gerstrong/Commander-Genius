#ifndef __GSEVENT_H_
#define __GSEVENT_H_

struct CEvent { virtual ~CEvent() {} };

/**
 * @brief The GsEventSink derived objects are those which receive the added event to the container
 */
class GsEventSink
{
public:
    virtual void pumpEvent(const CEvent *ev) = 0;
};

#endif /* __GSEVENT_H_ */
