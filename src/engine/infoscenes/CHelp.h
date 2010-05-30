/*
 * CHelp.h
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 */

#ifndef CHELP_H_
#define CHELP_H_

#include "CInfoScene.h"
#include "../../common/CMap.h"
#include "../../dialog/CTextViewer.h"
#include "../../fileio/CExeFile.h"
#include <string.h>

class CHelp : public CInfoScene {
public:
	CHelp(CExeFile &ExeFile, const std::string &type);

	void process();

	virtual ~CHelp();

private:
	CTextViewer *mp_TextViewer;
};

#endif /* CHELP_H_ */
