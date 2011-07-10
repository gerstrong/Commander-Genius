/*
 * CPlatformHorizontal.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#ifndef CPLATFORMHORIZONTAL_H_
#define CPLATFORMHORIZONTAL_H_

#include "CPlatform.h"
#include "CEngineParticleSprites.h"

namespace galaxy {

class CPlatformHorizontal : public CPlatform {
public:
	CPlatformHorizontal(CMap *pmap, Uint32 x, Uint32 y);

	/**
	 * \brief Processes the platforms AI. It's just simple moving around of course
	 */
	void process();

private:
	CEngineParticleSprites *mp_BoostEngObjLeft;
	CEngineParticleSprites *mp_BoostEngObjRight;
};

}

#endif /* CPLATFORMHORIZONTAL_H_ */
