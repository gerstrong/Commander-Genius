/*
 * CGUIText.h
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 *
 *  This is the text control for our new GUI
 */

#ifndef CGUITEXT_H_
#define CGUITEXT_H_

#include "CGUIControl.h"
#include <string>

class CGUIText : public CGUIControl
{
public:
	CGUIText(const CRect &lRect, const std::string& text);

	void processLogic();
	void processRender();

private:
	std::string mText;
};

#endif /* CGUITEXT_H_ */
