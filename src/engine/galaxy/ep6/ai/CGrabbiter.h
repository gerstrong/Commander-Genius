#ifndef CGRABBITER_H
#define CGRABBITER_H

#include <engine/galaxy/common/ai/CStunnable.h>

namespace galaxy
{

class CGrabbiter : public CStunnable
{
public:
    CGrabbiter(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
    
    void processHungry();
    
    void processNapping();
    
    void process();

    /**
     * What happens it gets touched by another object
     */
    void getTouchedBy(CSpriteObject &theObject);    
};

};

#endif // CGRABBITER_H
