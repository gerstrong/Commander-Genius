#ifndef CGRABBITER_H
#define CGRABBITER_H

#include "../../common/ai/CStunnable.h"

#include <base/utils/property_tree/property_tree.h>

namespace galaxy
{

class CGrabbiter : public CStunnable
{
public:
    CGrabbiter(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
    
    void deserialize(CSaveGameController &savedGame) override;    
    void serialize(CSaveGameController &savedGame) override;

    void serialize(GsKit::ptree &node) override;
    void deserialize(GsKit::ptree &node) override;

    
    void processHungry();
    
    void processNapping();
    
    void process() override;

    /**
     * What happens it gets touched by another object
     */
    void getTouchedBy(CSpriteObject &theObject) override;
};

}

#endif // CGRABBITER_H
