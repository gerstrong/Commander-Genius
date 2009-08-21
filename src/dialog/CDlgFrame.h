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
	DLG_THEME_OLDSCHOOL
};

class CDlgFrame {
public:
	CDlgFrame(int x, int y, int w, int h, Uint8 theme);
	virtual ~CDlgFrame();

	void draw();

	int m_x;
	int m_y;
	int m_w;
	int m_h;

private:

	int m_8x8tileheight;
	int m_8x8tilewidth;

	Uint8 m_theme;

	void drawOldShoolFrame();
};

#endif /* CDLGFRAME_H_ */
