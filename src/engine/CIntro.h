/*
 * CIntro.h
 *
 *  Created on: 19.06.2009
 *      Author: gerstrong
 */

#ifndef CINTRO_H_
#define CINTRO_H_

#include "graphics/GsGraphics.h"

class CIntro
{
public:
	CIntro();
	
	void init();
	
    void ponder();
    void render();
	
	bool isFinished() { return m_finished; }
	
private:
	CBitmap* mp_bm[6];
	SDL_Surface *mp_bmp_surface;
	int m_mid[6];
	int m_timer;
	int m_introtime;  // Total time to elapse until Main menu opens
	int m_scrolly;
	bool m_finished;
};

#endif /* CINTRO_H_ */
