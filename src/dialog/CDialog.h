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

// INPUT_MODE_NOTHING 		== do nothing
// INPUT_MODE_TEXT 			== text input
// INPUT_MODE_INTEGER		== integer input
// INPUT_MODE_UP_DOWN		== up/down movement, enter selection
// INPUT_MODE_LEFT_RIGHT	== left/right movement, enter selection
// INPUT_MODE_SLIDER		== slider, up/down movement
// INPUT_MODE_OPTION		== option, left/right selection, up/down movement
// INPUT_MODE_COUNTER		== counter, up/down movment
enum inputmodes{
	INPUT_MODE_NOTHING,
	INPUT_MODE_TEXT,
	INPUT_MODE_INTEGER,
	INPUT_MODE_UP_DOWN,
	INPUT_MODE_LEFT_RIGHT,
	INPUT_MODE_SLIDER,
	INPUT_MODE_OPTION,
	INPUT_MODE_COUNTER
};

class CDialog {
public:
	CDialog(Uint16 w, Uint16 h, char inputmode, Uint8 theme);
	virtual ~CDialog();

	void setSelection( const Uint8 selection );
	void setInputMode( const char inputmode );
	void addObject( Uint8 type, Uint16 x, Uint16 y,const std::string text="" );
	void setObjectText( Uint8 ID, const std::string &text );
	void setObjectType( Uint8 ID, Uint8 type );
	void setFontID(Uint8 value);

	bool getInputMode( const char inputmode );
	int getSelection();

	void processInput(int move = 0);
	void draw();

	bool m_nokeyletter;
	bool m_noise;
	int m_int;
	int m_min;
	int m_max;
	int m_cursorpos;
	size_t m_length;
	char m_inputmode;
	char m_curletter;
	std::string m_name;
	std::vector<CDlgObject*> m_dlgobject;
	
private:
	void drawTwirl();

	Uint16 m_x;
	Uint16 m_y;
	Uint16 m_w;
	Uint16 m_h;

	Uint8 m_theme;

	Sint8 m_selected_ID;
	int m_dlg_size;
	Uint8 m_switch;
	Uint8 m_scroll;
	int m_blinkctr;
	bool m_blink;
	
	CDlgFrame *mp_Frame;
	Uint8 m_alpha;
	Uint8 m_Font_ID;

	struct{
		Uint8  frame;
		Uint8  timer;
		Uint16 posy;
	} m_twirl;
};

#endif /* CDIALOG_H_ */
