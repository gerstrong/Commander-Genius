#ifndef CSATELITE_H
#define CSATELITE_H
#include <engine/galaxy/common/CGalaxySpriteObject.h>
#include <engine/galaxy/common/ai/CMoveTarget.h>
#include <engine/galaxy/common/ai/CPlayerBase.h>

namespace galaxy
{

class CSatelite : public CGalaxySpriteObject, public CMoveTarget
{
    
public:
	CSatelite(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

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

#endif // CSATELITE_H
