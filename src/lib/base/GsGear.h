#ifndef __GSGEAR_H__
#define __GSGEAR_H__

#include <base/GsEvent.h>

class GsGear
{
public:

    virtual void start() = 0;
    virtual void run(const float deltaT) = 0;
    virtual void render() = 0;
};

#endif // __GSGEAR_H__
