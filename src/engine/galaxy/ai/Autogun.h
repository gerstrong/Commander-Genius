#ifndef AUTOGUN_H
#define AUTOGUN_H

#include "engine/galaxy/CGalaxySpriteObject.h"

namespace galaxy
{

class AutoGun : public CGalaxySpriteObject
{
  AutoGun(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
};

};

#endif // AUTOGUN_H
