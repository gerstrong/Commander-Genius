/*
 * CPixelate.h
 *
 *  Created on: 09.03.2010
 *      Author: gerstrong
 */

#ifndef CPIXELATE_H_
#define CPIXELATE_H_

#include "CEffects.h"

class CPixelate  : public CEffects
{
public:
	CPixelate(unsigned short speed);
	void getSnapshot();
    void ponder();
    void render();
	virtual ~CPixelate();

private:
	SDL_Surface *mp_OldSurface;

	// m_line tells until which line, we have to draw the pixels
	// m_lines_completed gets ++, if a line has no more pixel that need to be drawn
	unsigned short m_line, m_lines_completed;

	// m_pixel_per_line tells at the given line how many pixels have already been drawn.
	unsigned short *m_pixels_per_line;
	bool *m_drawmap;
	unsigned short m_speed;
	Uint32 mColorkey;
};

#endif /* CPIXELATE_H_ */
