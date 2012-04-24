/*
 * CMessageBoxGalaxy.h
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXGALAXY_H_
#define CMESSAGEBOXGALAXY_H_

#include "dialog/CMessageBox.h"
#include "CVec.h"

class CMessageBoxGalaxy //: public CMessageBox
{
public:

	/**
	 * \brief This constructor creates a typical Keen 4 Message Box
	 * \param Text 				Message to be shown
	 */
	CMessageBoxGalaxy(const std::string& Text);
	virtual ~CMessageBoxGalaxy() {};

	virtual void process();

	bool m_mustclose;

protected:
	SDL_Rect m_TextPos;

};

#endif /* CMESSAGEBOXGALAXY_H_ */
