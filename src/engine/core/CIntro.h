/*
 * CIntro.h
 *
 *  Created on: 19.06.2009
 *      Author: gerstrong
 */

#ifndef CINTRO_H_
#define CINTRO_H_

#include "graphics/GsGraphics.h"
#include <array>

class CIntro
{
public:
	CIntro();
	
	void init();
	
    void ponder();
    void render();
	
	bool isFinished() { return m_finished; }
	
private:

    std::array<GsBitmap*, 6> mp_bm;
	SDL_Surface *mp_bmp_surface;
    std::array<int, 6> m_mid;
    int m_timer = 0;
    int m_introtime = 10;  // Total time (in seconds) to elapse until Main menu opens
    int m_scrolly = 200;
    bool m_finished = false;
};

#endif /* CINTRO_H_ */
