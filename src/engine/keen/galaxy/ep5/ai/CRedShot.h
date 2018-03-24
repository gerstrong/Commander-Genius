#ifndef CREDSHOT_H
#define CREDSHOT_H

#include "../../common/ai/CStunnable.h"

namespace galaxy {  

class CRedShot : public CStunnable
{
public:
    CRedShot(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xDir, const int yDir);    
    void processMove();
    void smash();

    void getTouchedBy(CSpriteObject &theObject);
    
    void process();
    
private:
	int mTimer;    
};

};

#endif // CREDSHOT_H
