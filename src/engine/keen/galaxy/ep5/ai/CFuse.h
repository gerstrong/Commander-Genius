/*
 * CFuse.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Fuses in Keen 5
 */
 
#ifndef CFUSE_H_
#define CFUSE_H_

#include <vector>
#include "../../common/CGalaxySpriteObject.h"

namespace galaxy {


class CFuse : public CGalaxySpriteObject
{
public:
    CFuse(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int sprVar);

	void process();

    bool loadPythonScripts(const std::string &scriptBaseName);

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;

    int mLevelTestBmp = 84; /** Index to use for the hint message (Keen 9 and other mods) */
};

}

#endif /* CFUSE_H_ */
