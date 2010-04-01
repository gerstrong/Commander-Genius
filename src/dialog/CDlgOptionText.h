/*
 * CDlgOptionText.h
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGOPTIONTEXT_H_
#define CDLGOPTIONTEXT_H_

#include "CDlgObject.h"

#include <string>
#include <SDL.h>

class CDlgOptionText : public CDlgObject {
public:

	////////////////////////
	////  Enumerations  ////
	////////////////////////
	enum{
		NORMAL,
		BLANK,
		DISABLED
	};

	CDlgOptionText(const std::string &text, Uint8 delimiter=0, Uint8 text_type=NORMAL);
	virtual ~CDlgOptionText();

	void setText(const std::string &text, Uint8 delimiter);
	void render(SDL_Surface *dst, Uint8 scrollamt, Uint8 highlight);
	void processInput();
	void draw(SDL_Surface *Textsurface, Uint16 x,Uint16 y, Uint8 highlight);

	int m_value;
	std::string	m_text;
	Uint8 m_type;

private:
	bool m_selected;
};

#endif /* CDLGOPTIONTEXT_H_ */
