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

#include <SDL/SDL.h>

class CTextBox {
public:
	CTextBox(int x, int y, const std::string& message);
	CTextBox(SDL_Surface *surface, int x, int y, const std::string& message);

	void process();
private:

	std::string m_String;
	SDL_Rect m_rect;
	int m_fontwidth, m_fontheight;
	bool m_border_relative;
	SDL_Surface *m_surface;
};

#endif /* CTEXTBOX_H_ */
