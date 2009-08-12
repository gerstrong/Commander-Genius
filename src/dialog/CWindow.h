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

enum Objecttypes{
	OBJ_TYPE_TEXT
};

enum Windowtypes{
	WND_STANDARD,			// Standard Dialogbox in where you can see items and close them by pressing a button
							// Example: Youseeinyourmind
	WND_PAGESCROLLER,		// This one can also be closed by pressing *Cancel*, but it has scrolling feature for many lines of text
							// Example: Help screen, Story text.
	WND_INTERACTIV			// This is one is the most important one. You can control the game with it.
							// Example: MainMenu, GamesMenu, Options, etc.
};

class CWindow {
public:
	CWindow(float x, float y, float w, float h, char window_type = WND_STANDARD);
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

	char m_window_type;

	std::list<int> m_ID_List;

	void drawWindow();
};

#endif /* CWINDOW_H_ */
