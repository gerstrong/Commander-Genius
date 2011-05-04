/*
 * CDlgFrame.h
 *
 *  Created on: 20.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGFRAME_H_
#define CDLGFRAME_H_

#include <SDL.h>

enum themes{
	DLG_THEME_NONE,
	DLG_THEME_VORTICON,
	DLG_THEME_GALAXY,
	DLG_THEME_RED
};

class CDlgFrame {
public:
	CDlgFrame(int x, int y, int w, int h, Uint8 theme = DLG_THEME_VORTICON, int tilewidth = 8, int tileheight = 8);

	void draw(SDL_Surface *dst);

	/**
	 * \brief Resizes the dialog box to new dimensions
	 * \param width New width of the box
	 * \param width New height of the box
	 */
	void resize(const Uint32 width, const Uint32 height);

	int m_x;
	int m_y;
	int m_w;
	int m_h;

private:

	int m_8x8tileheight;
	int m_8x8tilewidth;

	Uint8 m_theme;

	void drawOldSchoolFrame(SDL_Surface *dst);
	void drawGalaxyFrame(SDL_Surface *dst);
	void drawRedFrame(SDL_Surface *dst);

};

#endif /* CDLGFRAME_H_ */
