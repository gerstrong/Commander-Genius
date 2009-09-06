/*
 * CDialog.h
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#ifndef CDIALOG_H_
#define CDIALOG_H_

#include <vector>
#include <SDL.h>
#include "CDlgObject.h"

class CDialog {
public:
	CDialog(SDL_Surface *DialogSurface, Uint16 x, Uint16 y, Uint16 w, Uint16 h);
	virtual ~CDialog();

	void setFrameTheme( Uint8 theme );
	void addObject( Uint8 type, Uint16 x, Uint16 y,const std::string text );
	void setObjectText( Uint8 ID, const std::string &text );
	void setSDLSurface( SDL_Surface* Surface ) { m_DialogSurface = Surface; }

	int getSelection();

	void processlogic();
	void render();
	void renderTwirl();

private:
	Uint16 m_x;
	Uint16 m_y;
	Uint16 m_w;
	Uint16 m_h;

	Uint8 m_selected_ID;
	Uint8 m_switch;
	Uint8 m_scroll;

	CDlgFrame *m_Frame;
	Uint8 m_alpha;

	struct{
		Uint8  frame;
		Uint8  timer;
		Uint16 posy;
	} m_twirl;

	SDL_Surface *m_DialogSurface;

	std::vector<CDlgObject*> m_dlgobject;
};

#endif /* CDIALOG_H_ */
