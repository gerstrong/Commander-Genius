/*
 * CMessageBoxGalaxy.h
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXGALAXY_H_
#define CMESSAGEBOXGALAXY_H_

#include "dialog/CMessageBox.h"

class CMessageBoxGalaxy : public CMessageBox {
public:
	CMessageBoxGalaxy(const std::string& Text);

	void process();
};

#endif /* CMESSAGEBOXGALAXY_H_ */
