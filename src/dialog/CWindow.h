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

	// Creation routines
	void addObject(CTextBox* newTextBox);

	// Property set methods
	void Resize(float width, float height);

	// Property retrieval methods
	float getWidth();

	// Drawing routines
	void render();

	std::vector<CTextBox*> m_TextBox;

private:
	float m_x;
	float m_y;
	float m_h;
	float m_w;

	float m_8x8tileheight;
	float m_8x8tilewidth;

	std::list<int> m_ID_List;

	void drawWindow();
};

#endif /* CWINDOW_H_ */
