/*
 * CStory.h
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 */

#ifndef CSTORY_H_
#define CSTORY_H_

#include "CInfoScene.h"

class CStory : public CInfoScene {
public:
	CStory();

	void process();

	virtual ~CStory();
};

#endif /* CSTORY_H_ */
