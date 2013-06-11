/*
 * CVibrate.h
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 */

#ifndef CVIBRATE_H_
#define CVIBRATE_H_

#include <memory>

#include "CEffects.h"
#include "sdl/CTimer.h"

class CVibrate : public CEffects
{
public:
	CVibrate(Uint32 msecs);

    void ponder();
    void render();

private:
	Uint32 m_StartTime;
	Uint32 m_RunTime;

	Sint8 m_dir_x, m_dir_y;

	std::shared_ptr<SDL_Surface> mpVibSfc;

    SDL_Rect mVibRect;
};

#endif /* CVIBRATE_H_ */
