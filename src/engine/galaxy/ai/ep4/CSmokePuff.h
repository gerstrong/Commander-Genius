#ifndef CSMOKEPUFF_H
#define CSMOKEPUFF_H

#include "engine/galaxy/CGalaxySpriteObject.h"


namespace galaxy {

class CSmokePuff : public CGalaxySpriteObject
{
public:
    CSmokePuff(CMap *pmap, const Uint32 x, const Uint32 y);

    void process();
};

};

#endif // CSMOKEPUFF_H
