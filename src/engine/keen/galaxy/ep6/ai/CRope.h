#ifndef CROPE_H
#define CROPE_H

#include "../../common/ai/CGalaxyActionSpriteObject.h"

namespace galaxy
{

class CRope : public CGalaxyActionSpriteObject
{
public:
    static const unsigned int FOE_ID = 201;
    
    CRope(CMap *pmap, Uint32 x, Uint32 y);

    void processActive();
    void processThrown();

    void deserialize(CSaveGameController &savedGame);

    void serialize(CSaveGameController &savedGame);

    void serialize(boost::property_tree::ptree &node);

    void getTouchedBy(CSpriteObject &theObject);

    void process();
    
private:
    int mTimer;
};

};

#endif // CROPE_H
