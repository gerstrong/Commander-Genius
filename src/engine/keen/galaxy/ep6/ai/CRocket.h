#ifndef CROCKET_H
#define CROCKET_H
#include "../../common/ai/CGalaxyActionSpriteObject.h"
#include "../../common/ai/CMoveTarget.h"
#include "../../common/ai/CPlayerBase.h"

namespace galaxy
{
 

class CRocket : public CGalaxyActionSpriteObject, public CMoveTarget
{
    
public:
	CRocket(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y);

    void deserialize(CSaveGameController &savedGame);

    void serialize(CSaveGameController &savedGame);

    void serialize(GsKit::ptree &node);

    void deserialize(GsKit::ptree &node);


	/**
	 * When it is moving normally
	 */
	void processSitting();
	
	void processFlying();
	
	void process();

	/**
	 * What happens if it gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	CPlayerBase *mpCarriedPlayer;
};

};

#endif // CROCKET_H
