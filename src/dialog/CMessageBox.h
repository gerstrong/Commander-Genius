/*
 * CMessageBox.h
 *
 *  Created on: 04.12.2009
 *      Author: gerstrong
 *
 *  This is a normal Message box that shows some text and exits
 *  when someone presses a key or button (command input)
 */

#ifndef CMESSAGEBOX_H_
#define CMESSAGEBOX_H_

#include "../dialog/CDlgFrame.h"
#include <string>
#include <vector>

class CMessageBox {
public:
	CMessageBox(const std::string& Text);
	void process();
	bool isFinished();
	virtual ~CMessageBox();

private:
	bool m_mustclose;
	std::vector<std::string> m_Lines;
	CDlgFrame *mp_DlgFrame;
	SDL_Rect m_gamerect;
};

#endif /* CMESSAGEBOX_H_ */
