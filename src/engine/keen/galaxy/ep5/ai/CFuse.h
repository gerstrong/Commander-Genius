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

    bool isNearby(CSpriteObject &theObject) override;

    bool loadLuaScript(const std::string &scriptBaseName) override;

	/**
     * What happens if the foe gets touched by another object
	 */
    void getTouchedBy(CSpriteObject &theObject) override;

    /**
     *  Since this class also describes the QED Core, with this flag the whole game can end
     */
    void winGame(const int msecs);
	
private:
    uint32_t mGameEndsTime = 0; // Timer to run down, before game ends

    uint32_t mLevelEndsTime = 0; // Timer to run down, before game ends

    int mBmpIdx = 84; // Portrait for shown messages. May be modified by some mods like Keen 9

};

}

#endif /* CFUSE_H_ */
