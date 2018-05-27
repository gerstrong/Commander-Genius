#ifndef CGRABBITER_H
#define CGRABBITER_H

#include "../../common/ai/CStunnable.h"

#include <boost/property_tree/ptree.hpp>

namespace galaxy
{

class CGrabbiter : public CStunnable
{
public:
    CGrabbiter(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
    
    void deserialize(CSaveGameController &savedGame) override;    
    void serialize(CSaveGameController &savedGame) override;

    void serialize(boost::property_tree::ptree &node) override;
    void deserialize(boost::property_tree::ptree &node) override;

    
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
