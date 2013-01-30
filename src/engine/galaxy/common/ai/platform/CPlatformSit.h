/*
 * CPlatformDrop.h
 *
 *  Created on: 01.01.2013
 *      Author: gerstrong
 *
 *  This is the code of the sitting platforms.
 *  In Galaxy this platform doesn't move at all.
 */


#ifndef CPLATFORMSIT_H
#define CPLATFORMSIT_H

#include "CPlatform.h"

namespace galaxy
{
    
    class CPlatformSit : public CPlatform
    {
    public:
        CPlatformSit(CMap *pmap, const Uint16 foeID,
                     const Uint32 x, const Uint32 y, const int actionOff);
        
        /**
         * \brief Processes the platforms AI. It's just simple moving around of course
         */
        void process();
        
    };
    
}


#endif // CPLATFORMSIT_H
