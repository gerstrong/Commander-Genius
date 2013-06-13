/*
 * CTextViewer.h
 *
 *  Created on: 12.08.2009
 *      Author: gerstrong
 */

#ifndef CTEXTVIEWER_H_
#define CTEXTVIEWER_H_

#include <SDL.h>
#include <vector>
#include <string>
#include <memory>
#include "core/LogicDrawSplitter.h"

class CTextViewer : public GameState
{
public:
	CTextViewer(int x, int y, int w, int h);

	void initialize();
	void renderBox();
    void ponder();
    void render();

	void formatText(const std::string &text);
	bool loadTextfromFile(const std::string &filename);
	bool hasClosed() { return m_mustclose; }

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
	bool m_mustclose;
	int m_timer;

	std::shared_ptr<SDL_Surface> mpTextVSfc;
};

#endif /* CTEXTVIEWER_H_ */
