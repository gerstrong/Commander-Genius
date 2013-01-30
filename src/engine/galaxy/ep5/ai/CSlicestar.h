/*
 * CSlicestar.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */

#ifndef CSLICESTAR_H_
#define CSLICESTAR_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {
    
    
    class CSlicestar : public CStunnable
    {
    public:
        CSlicestar(CMap *pmap, const Uint16 foeID,
                   const Uint32 x, const Uint32 y,
                   const bool useBlocker, direction_t horDir,
                   direction_t verDir);
        
        void process();
        
        /**
         * What happens if the slug gets touched by another object
         */
        void getTouchedBy(CSpriteObject &theObject);
        
    private:
        bool mUseBlocker;
    };
    
}

#endif /* CSLICESTAR_H_ */
