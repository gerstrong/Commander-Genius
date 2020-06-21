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

    bool isNearby(CSpriteObject &theObject) override;

	/**
     * What happens if the foe gets touched by another object
	 */
    void getTouchedBy(CSpriteObject &theObject) override;

    /**
     *  Since this class also describes the QED Core, with this flag the whole game can end
     */
    void winGame(const int msecs);
	
private:
    int mLevelTestBmp = 84; /** Index to use for the hint message (Keen 9 and other mods) */

    uint32_t mGameEndsTime = 0; // Timer to run down, before game ends

    uint32_t mLevelEndsTime = 0; // Timer to run down, before game ends

};

}

#endif /* CFUSE_H_ */
