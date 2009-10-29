/*
 * CDlgFrame.h
 *
 *  Created on: 20.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGFRAME_H_
#define CDLGFRAME_H_

#include <SDL/SDL.h>

enum themes{
	DLG_THEME_NONE,
	DLG_THEME_OLDSCHOOL,
	DLG_THEME_RED
};

class CDlgFrame {
public:
	CDlgFrame(int x, int y, int w, int h, Uint8 theme);
	virtual ~CDlgFrame();

	void draw(SDL_Surface *dst);

	int m_x;
	int m_y;
	int m_w;
	int m_h;

private:

	int m_8x8tileheight;
	int m_8x8tilewidth;

	Uint8 m_theme;

	void drawOldSchoolFrame(SDL_Surface *dst);
	void drawRedFrame(SDL_Surface *dst);
};

#endif /* CDLGFRAME_H_ */
