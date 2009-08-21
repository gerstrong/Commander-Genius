/*
 * CDlgOptionText.h
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGOPTIONTEXT_H_
#define CDLGOPTIONTEXT_H_

#include <string>
#include <SDL/SDL.h>

class CDlgOptionText {
public:
	CDlgOptionText(const std::string &text, unsigned int delimit);
	virtual ~CDlgOptionText();

	void setText(const std::string &text, unsigned int delimit);
	void draw(Uint16 x,Uint16 y, bool highlight);

private:
	std::string	m_text;
	bool m_selected;
};

#endif /* CDLGOPTIONTEXT_H_ */
