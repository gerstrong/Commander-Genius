#ifndef AUTOGUN_H
#define AUTOGUN_H

#include "engine/galaxy/common/CGalaxySpriteObject.h"

namespace galaxy
{
    
    class AutoGun : public CGalaxySpriteObject
    {
        
    public:
        AutoGun(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
                direction_t horDir, direction_t vertDir, int basesprite);
        
        void process();
        
        void getTouchedBy(CSpriteObject &theObject);
        
    private:
        
        void waiting();
        void setWaitStatus();
        void flying();
        
        void (AutoGun::*processState)();
        
        static const int FLY_SPEED = 160;
        static const int WAIT_TIME = 150;
        
        VectorD2<Uint32> origin;
        int mBaseSprite;
        int mNumAnimSprites;
        int mTimer;
    };
    
};

#endif // AUTOGUN_H
