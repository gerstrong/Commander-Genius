/*
 * CHQBitmap.h
 *
 *  Created on: 18.04.2009
 *      Author: gerstrong
 */

#ifndef CHQBITMAP_H_
#define CHQBITMAP_H_

#include <SDL.h>
#include <string>

class CHQBitmap {
public:
	CHQBitmap(SDL_Rect screenrect);
	virtual ~CHQBitmap();

	void setScrollposition(unsigned int xpos, unsigned int ypos);
	void updateHQBitmap(SDL_Surface *m_surface, SDL_Rect *p_srcrect, SDL_Rect *p_dstrect);
	bool loadImage(const std::string& pFilename, int wsize, int hsize);
	void setAlphaBlend(Uint8 alpha);
	void offsetAlphaBlend(Uint8 alpha);

private:
	bool m_active;

	SDL_Surface *m_blackscreen;	// used to put over the bitmap, it can be darkened
	SDL_Surface *m_scrimg;
	SDL_Rect	m_imagerect;
	SDL_Rect	m_screenrect;
	Uint8	m_alpha;
};

#endif /* CHQBITMAP_H_ */
