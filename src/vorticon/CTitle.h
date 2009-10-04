/*
 * CTitle.h
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 * This class manages the titlescreen of the game which 
 * is very simple and maybe the class is a bit 
 * overloaded for such task. Nevertheless we do it
 * that way because if more games are added, 
 * it may get more complicated as the title may
 * differ from other passive modes.   
 */


#ifndef CTITLE_H_
#define CTITLE_H_

#include <SDL/SDL.h>
#include "../graphics/CGfxEngine.h"

class CTitle {
public:

	CTitle();
	virtual ~CTitle();

	bool init();
	void process();
	void cleanup();

	bool isFinished() 
		{ return m_finished; }

private:
	CBitmap *mp_bitmap_title;
	CBitmap *mp_bitmap_f1help;
	SDL_Surface *mp_bmp_surface;
	bool m_finished;
	unsigned int m_time;
};
#endif /* CTITLE_H_ */
