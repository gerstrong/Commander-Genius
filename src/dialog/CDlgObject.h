/*
 * CDlgObject.h
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGOBJECT_H_
#define CDLGOBJECT_H_

#include "CDlgFrame.h"
#include "CDlgOptionText.h"
#include <SDL.h>

////////////////////////
////  Enumerations  ////
////////////////////////
enum{
	DLG_OBJ_OPTION_TEXT = 1,
	DLG_OBJ_BLANK_LINE,
	DLG_OBJ_DISABLED,
	DLG_OBJ_TEXT
};

class CDlgObject {
public:
	CDlgObject();
	virtual ~CDlgObject();

	void create(Uint8 type, Uint16 ID, Uint16 x, Uint16 y, const std::string &text,
							unsigned int delimiter, Uint8 Fontmap_ID=0, Uint8 theme=DLG_THEME_VORTICON);
	void change(unsigned int delimiter, const std::string &text, Uint8 type);
	void setSelection(bool value);

	void render(SDL_Surface *dst, Uint8 scrollamt, bool highlight);

	CDlgOptionText 	*m_Option;

	Uint16 m_ID;
	Uint8 m_type;
	Uint8 m_theme;
	bool m_selectable;
	bool m_selected;
	Uint32 m_colour;

	Uint16 m_x;
	Uint16 m_y;
};

#endif /* CDLGOBJECT_H_ */
