/*
 * CStory.h
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 */

#ifndef CSTORY_H_
#define CSTORY_H_

#include "CInfoScene.h"
#include "common/CMap.h"
#include "dialog/CTextViewer.h"
#include "fileio/CExeFile.h"
#include <memory>
#include <string.h>

class CStory : public CInfoScene
{
public:

	void init();
	void process();
	void teardown();

private:
	std::shared_ptr<CMap> mpMap;
	std::unique_ptr<CTextViewer> mpTextViewer;
};

#endif /* CSTORY_H_ */
