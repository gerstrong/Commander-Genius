#ifndef POINTDEVICE_H_
#define POINTDEVICE_H_

//#include <base/Event.h>
//#include <base/Vector2D.h>
#include "Vector2D.h"
#include <base/Singleton.h>
#include <SDL.h>

struct GsPointingState
{
    GsPointingState(const Vector2D<float> &pos,
                 const Uint32 actionbutton) :
        mPos(pos),
        mActionButton(actionbutton)   {}

    GsPointingState()
        {}

    Vector2D<float> mPos;
    Uint32 mActionButton;
};

class PointDevice : public GsSingleton<PointDevice>
{
  public:
    GsPointingState mPointingState;
};

#define gPointDevice PointDevice::get()

#endif // POINTDEVICE_H_
