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
	CDialog(SDL_Surface *DialogSurface, Uint16 w, Uint16 h);
	CDialog(SDL_Surface *DialogSurface, Uint16 x, Uint16 y, Uint16 w, Uint16 h, char key);
	virtual ~CDialog();

	void setFrameTheme( Uint8 theme );
	void addObject( Uint8 type, Uint16 x, Uint16 y,const std::string text="" );
	void setObjectText( Uint8 ID, const std::string &text );
	void setObjectType( Uint8 ID, Uint8 type );
	void setSDLSurface( SDL_Surface* Surface ) { m_DialogSurface = Surface; }

	int getSelection() { return m_selected_ID; }

	void processInput(int move = 0);
	void draw();
	void drawTwirl();

	int m_int;
	int m_min;
	int m_max;
	int m_length;
	char m_key;
	std::string m_name;
	std::string m_num;
	std::vector<CDlgObject*> m_dlgobject;
	
private:
	Uint16 m_x;
	Uint16 m_y;
	Uint16 m_w;
	Uint16 m_h;

	Uint8 m_selected_ID;
	int m_dlg_size;
	Uint8 m_switch;
	Uint8 m_scroll;
	int m_blinkctr;
	bool m_blink;
	
	CDlgFrame *m_Frame;
	Uint8 m_alpha;

	struct{
		Uint8  frame;
		Uint8  timer;
		Uint16 posy;
	} m_twirl;

	SDL_Surface *m_DialogSurface;
};

#endif /* CDIALOG_H_ */
