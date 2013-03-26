#ifndef CROPE_H
#define CROPE_H

#include <engine/galaxy/common/ai/CGalaxyActionSpriteObject.h>

namespace galaxy
{

class CRope : public CGalaxyActionSpriteObject
{
public:
    CRope(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

    void processActive();
    void processThrown();
    void processHidden();

    void getTouchedBy(CSpriteObject &theObject);

    void process();
};

};

#endif // CROPE_H
