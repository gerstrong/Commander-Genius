/*
 * CArachnut.h
 *
 *  Created on: 17.07.2012
 *      Author: gerstong
 */

#ifndef CARACHNUT_H_
#define CARACHNUT_H_

#include "engine/galaxy/common/CGalaxySpriteObject.h"

namespace galaxy {
    
    class CArachnut : public CGalaxySpriteObject
    {
    public:
        CArachnut(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);
        
        
        bool isNearby(CSpriteObject &theObject);
        
        void getTouchedBy(CSpriteObject &theObject);
        
        int checkSolidD( int x1, int x2, int y2, const bool push_mode );
        
        void process();
        
        
    };
    
} /* namespace galaxy */
#endif /* CARACHNUT_H_ */
