/*
 * CShikadiMaster.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */
 
#ifndef CSHIKADIMASTER_H_
#define CSHIKADIMASTER_H_

#include <vector>
#include "../../common/ai/CStunnable.h"
#include "../../common/ai/CPlayerLevel.h"

namespace galaxy {


class CShikadiMaster : public CStunnable
{
public:
	CShikadiMaster(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);


    bool loadPythonScripts(const std::string &scriptBaseName);

	/**
	 * Action functions
	 */
	void processStanding();	

    void processShootingAgressive();

	void processShooting();	
	void processTeleporting();	
	
	bool isNearby(CSpriteObject &theObject);

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
    int mTimer = 0;
	direction_t mKeenAlignment;
    bool mTeleport = false;
	
    CPlayerLevel *mpPlayer = nullptr;
    bool swapYDir = false;

    /**
     * @brief mMoreAgressive    In Keen 9 they are more agressive, in hard they also can be
     */
    bool mMoreAgressive = false;
};

}

#endif /* CSHIKADIMASTER_H_ */
