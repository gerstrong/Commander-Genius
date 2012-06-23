/*
 * CLindsey.h
 *
 *  Created on: 14.08.2011
 *      Author: gerstrong
 *
 *  The princess that is able to give a hint to Billy Blaze
 */

#ifndef CLINDSEY_H_
#define CLINDSEY_H_

#include "engine/galaxy/CGalaxySpriteObject.h"

namespace galaxy {

class CLindsey : public CGalaxySpriteObject {
public:
	CLindsey(CMap *pmap, Uint32 x, Uint32 y);

	void process();

	void getTouchedBy(CSpriteObject &theObject);

private:
	int m_timer;
	std::map<int, std::string>	answermap;
};

} /* namespace galaxy */
#endif /* CLINDSEY_H_ */
