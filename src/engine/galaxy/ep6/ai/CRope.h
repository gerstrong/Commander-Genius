#ifndef CROPE_H
#define CROPE_H

#include <engine/galaxy/common/ai/CGalaxyActionSpriteObject.h>

namespace galaxy
{

class CRope : public CGalaxyActionSpriteObject
{
public:
    static const unsigned int FOE_ID = 201;
    
    CRope(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

    void processActive();
    void processThrown();

    void getTouchedBy(CSpriteObject &theObject);

    void process();
    
private:
    int mTimer;
};

};

#endif // CROPE_H
