/*
 * CPlatformHorizontal.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#ifndef CPLATFORMHORIZONTAL_H_
#define CPLATFORMHORIZONTAL_H_

#include "CPlatform.h"

namespace galaxy {

class CPlatformHorizontal : public CPlatform
{
public:
	CPlatformHorizontal(CMap *pmap, const Uint16 foeID, 
			    const direction_t xDir, Uint32 x, Uint32 y,
                const int actionOffset, const int sprVar );

	/**
	 * \brief Processes the platforms AI. It's just simple moving around of course
	 */
	void process();
	
	void draw();

private:
    
	bool drawFire;
	int m_FireSprite;
	int m_fireTimer;

    int moveHorizSpeed;
};

}

#endif /* CPLATFORMHORIZONTAL_H_ */
