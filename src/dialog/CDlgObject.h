/*
 * CDlgObject.h
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGOBJECT_H_
#define CDLGOBJECT_H_

#include "CDlgFrame.h"
#include <SDL.h>
#include "../sdl/CInput.h"

class CDlgObject {
public:
	CDlgObject();
	virtual ~CDlgObject();

	//void create(Uint8 type, Uint16 ID, Uint16 x, Uint16 y, const std::string &text, unsigned int delimiter);
	void create(Uint16 ID, Uint16 x, Uint16 y);
	//void change(unsigned int delimiter, const std::string &text, Uint8 type);
	void looseFocus();
	bool setFocus();
	bool getPrevItem();
	bool getNextItem();

	virtual void processInput() {}
	virtual void render(SDL_Surface *dst, Uint8 scrollamt, Uint8 highlight);

	Uint16 m_ID;
	Uint8 m_type;
	bool m_selectable;
	bool m_selected;

	bool m_previtem;
	bool m_nextitem;

	Uint16 m_x;
	Uint16 m_y;
};

#endif /* CDLGOBJECT_H_ */
