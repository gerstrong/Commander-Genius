/*
 * CPlatformHorizontal.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#ifndef CPLATFORMHORIZONTAL_H_
#define CPLATFORMHORIZONTAL_H_

#include "CPlatform.h"
#include <memory>

namespace galaxy {

class CPlatformHorizontal : public CPlatform
{
public:
	CPlatformHorizontal(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	/**
	 * \brief Processes the platforms AI. It's just simple moving around of course
	 */
	void process();
	
	void draw();

private:
    
	bool drawFire;
	int m_FireSprite;
	int m_fireTimer;
};

}

#endif /* CPLATFORMHORIZONTAL_H_ */
