#ifndef CROPE_H
#define CROPE_H

#include "../../common/ai/CGalaxyActionSpriteObject.h"

namespace galaxy
{

class CRope : public CGalaxyActionSpriteObject
{
public:
    static const unsigned int FOE_ID = 201;
    
    CRope(std::shared_ptr<CMap> pMap, Uint32 x, Uint32 y);

    void processActive();
    void processThrown();

    void deserialize(CSaveGameController &savedGame);

    void serialize(CSaveGameController &savedGame);

    void serialize(GsKit::ptree &node);

    void getTouchedBy(CSpriteObject &theObject);

    void process();
    
private:
    int mTimer;
};

};

#endif // CROPE_H
