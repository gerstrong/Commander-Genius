/*
 * CEngineParticleSprites.h
 *
 *  Created on: 10.07.2011
 *      Author: gerstrong
 *
 *  This is what you get to see from the platforms. There is shown fire for hovering sprites
 *  and for moving left and right.
 */

#ifndef CENGINEPARTICLESPRITES_H_
#define CENGINEPARTICLESPRITES_H_

#include "engine/galaxy/CGalaxySpriteObject.h"

namespace galaxy {

class CEngineParticleSprites : public CGalaxySpriteObject
{
public:
	CEngineParticleSprites(	CMap *pmap,
			const Uint16 foeID,
			const Uint32 x,
			const Uint32 y,
			const bool vert,
			const bool leftside );


	/**
	 * A simple swap of values between m_nextsprite and sprite variable
	 */
	void swapSprites();

	/**
	 * process cycle which is called in the game loop
	 */
	void process();

private:
	bool m_vert;
	size_t m_nextsprite;
	size_t m_timer;
};

} /* namespace galaxy */
#endif /* CENGINEPARTICLESPRITES_H_ */
