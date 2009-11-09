/*
 * CTextBox.h
 *
 *  Created on: 30.07.2009
 *      Author: gerstrong
 */

#ifndef CTEXTBOX_H_
#define CTEXTBOX_H_

#include <string>
#include <vector>

#include "../graphics/CFont.h"
#include <SDL/SDL.h>

class CTextBox {
public:
	CTextBox(int y, int h, const std::string& message);
	CTextBox(SDL_Surface *surface, int y, int h, const std::string& message);

	void setAttribs(Uint8 typewriterspeed, Uint8 lettertype );
	void enableBorders(bool border){ m_border = border; }
	
	void process();

private:
	void setup(int y, int h, const std::string& message);
	
	std::string m_String;
	int m_fontwidth, m_fontheight;
	bool m_border;
	SDL_Rect m_rect;
	Uint8 m_tw_waittime;
	Uint8 m_lettertype;
	SDL_Surface *m_surface;
	Uint32 m_numchars;
	int m_timer;
};

#endif /* CTEXTBOX_H_ */
