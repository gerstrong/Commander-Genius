#ifndef CORBATRIX_H
#define CORBATRIX_H

#include "../../common/ai/CGalaxyActionSpriteObject.h"

namespace galaxy
{
    
class COrbatrix : public CGalaxyActionSpriteObject
{
public:
    COrbatrix(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar);

    void processFloat();
    void processMove();
    void processCurl();
    void processUncurl();
    void processBounce();
    void processLand();
    void processSlide();

    bool isNearby(CSpriteObject &theObject);
    
    void getTouchedBy(CSpriteObject &theObject);

    void process();    
    
private:
    int mTimer;
    bool mGivesKey;
    
};

};

#endif // CORBATRIX_H
