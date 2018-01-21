/*
 * CColorMerge.h
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 *
 * This class gets two surface and blends them the way
 * the player will see a smooth scene transition.
 */

#ifndef CCOLORMERGE_H_
#define CCOLORMERGE_H_

#include <SDL.h>
#include <memory>

#include <base/video/GsEffectController.h>
#include <graphics/GsSurface.h>

class CColorMerge : public CEffects
{
public:
	CColorMerge(const Uint8 speed);

    void ponder(const float deltaT) override;
    void render() override;
		
	void getSnapshot();		

private:
	
    Uint8 m_Speed = 0;
    Uint8 m_Alpha = 0;

    GsSurface mOldSurface;
};

#endif /* CCOLORMERGE_H_ */
