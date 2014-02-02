#ifndef POINTDEVICE_H_
#define POINTDEVICE_H_

//#include <base/Event.h>
//#include <lib/base/Vector2D.h>
#include "CVec.h"
#include <lib/base/Singleton.h>
#include <SDL.h>

struct GsPointingState
{
    GsPointingState(const CVec &pos,
                 const Uint32 actionbutton) :
        mPos(pos),
        mActionButton(actionbutton)   {}

    GsPointingState()
        {}

    CVec mPos;
    Uint32 mActionButton;
};

class PointDevice : public GsSingleton<PointDevice>
{
  public:
    GsPointingState mPointingState;
};

#define gPointDevice PointDevice::get()

#endif // POINTDEVICE_H_
