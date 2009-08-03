/*
 * CTextBox.h
 *
 *  Created on: 30.07.2009
 *      Author: gerstrong
 */

#ifndef CTEXTBOX_H_
#define CTEXTBOX_H_

#include <string>
#include <vector>

class CTextBox {
public:
	CTextBox(float x, float y, float w, float h,
			const std::string& text, bool border_rel = true);

	void setFontDimensions(float width, float height = 0.0f);
	void format();
	void draw();

	int getNumberOfTextlines();

private:

	std::string m_String;
	std::vector <std::string> m_Textline;
	float m_x, m_y, m_w, m_h;
	float m_fontwidth, m_fontheight;
	bool m_border_relative;
};

#endif /* CTEXTBOX_H_ */
