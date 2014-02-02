/*
 * CHelp.h
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 */

#ifndef CHELP_H_
#define CHELP_H_

#include "CInfoScene.h"
#include "common/CMap.h"
#include "dialog/CTextViewer.h"
#include "fileio/CExeFile.h"
#include <string.h>
#include <memory>

class CHelp : public CInfoScene
{
public:
	CHelp(const std::string &type);

	void init();
	void teardown();

    void ponder();
    void render();

private:
	std::string mTextType;
	std::unique_ptr<CTextViewer> mpTextViewer;
};

#endif /* CHELP_H_ */
