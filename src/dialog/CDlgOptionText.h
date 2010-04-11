/*
 * CDlgOptionText.h
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGOPTIONTEXT_H_
#define CDLGOPTIONTEXT_H_

#include <string>
#include <SDL.h>

class CDlgOptionText {
public:
	CDlgOptionText(const std::string &text, unsigned int delimit);
	virtual ~CDlgOptionText();

	void setText(const std::string &text, unsigned int delimit);
	void draw(SDL_Surface *Textsurface, Uint16 x,Uint16 y, bool highlight, Uint32 colour = 0x0);

	int m_value;
	std::string	m_text;

private:
	bool m_selected;
};

#endif /* CDLGOPTIONTEXT_H_ */
