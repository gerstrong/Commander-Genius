/*
 * CEnemyShot.h
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 *
 * This class will process all the enemy shots which kill Keen and go straight one direction
 * Those can Sprite shots and darts in Keen 4 for example.
 */

#ifndef CENEMYSHOT_H_
#define CENEMYSHOT_H_

#include "engine/galaxy/common/CGalaxySpriteObject.h"

namespace galaxy {
    
    class CEnemyShot : public CGalaxySpriteObject
    {
    public:
        CEnemyShot(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
                   const int actionFormatOffset, const int xDir, const int yDir, const int speed);
        
        void getTouchedBy(CSpriteObject &theObject);
        
        void process();
        
    private:
        int m_speed;
    };
    
} /* namespace galaxy */
#endif /* CENEMYSHOT_H_ */
