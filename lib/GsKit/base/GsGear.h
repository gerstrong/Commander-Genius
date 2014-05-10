#ifndef __GSGEAR_H__
#define __GSGEAR_H__

/**
  \description GsGear is pretty much what ensures a correct connection
               to running Application. Based on the game structure
               it represents an functional object, that is run at LPS rate
               and specified renders at FPS. It can also be started and stopped
               at any time wherever needed. Finally it also can get events and process
               them through pumpEvent a routine which also might pass it to children if any.
*/

#include <base/GsEvent.h>

class GsGear
{
public:
    virtual void pumpEvent(const CEvent *evPtr) {}
    virtual void run(const float deltaT) = 0;
    virtual void render() {}
};

#endif // __GSGEAR_H__
