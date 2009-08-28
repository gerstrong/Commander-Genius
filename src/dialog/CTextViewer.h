/*
 * CTextViewer.h
 *
 *  Created on: 12.08.2009
 *      Author: gerstrong
 */

#ifndef CTEXTVIEWER_H_
#define CTEXTVIEWER_H_

#include <vector>
#include <string>

class CTextViewer {
public:
	CTextViewer(SDL_Surface *TextVSfc, int x, int y, int w, int h);

	void initialize();
	void renderBox();
	void processCycle();

	void loadText(const std::string text);

	void setNextPos();
	void setPrevPos();
	void setPosition(int pos);

	void scrollDown();
	void scrollUp();

private:

	unsigned char getnextwordlength(const std::string nextword);
	void drawTextlines();

	int m_x;
	int m_y;
	int m_w;
	int m_h;

	int m_8x8tileheight;
	int m_8x8tilewidth;

	std::string mp_text;
	std::vector<std::string> m_textline;

	int m_linepos;
	char m_scrollpos; // Goes from 0 to textheight and is only used for a smooth scroll effect

	SDL_Surface *m_TextVSfc;
};

#endif /* CTEXTVIEWER_H_ */
