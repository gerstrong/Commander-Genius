/*
 * CWindow.h
 *
 *  Created on: 29.07.2009
 *      Author: gerstrong
 */

#ifndef CWINDOW_H_
#define CWINDOW_H_

#include <list>
#include <vector>
#include <string>
#include "CTextBox.h"

enum{
	OBJ_TYPE_TEXT
};

class CWindow {
public:
	CWindow(float x, float y, float w, float h);
	virtual ~CWindow();

	void addTextBox(float x, float y, float w, float h, const std::string& text, bool border_rel);

	void render();

private:
	float m_x;
	float m_y;
	float m_h;
	float m_w;

	float m_8x8tileheight;
	float m_8x8tilewidth;

	std::list<int> m_ID_List;
	std::vector<CTextBox*> m_TextBox;

	void drawWindow();
};

#endif /* CWINDOW_H_ */
