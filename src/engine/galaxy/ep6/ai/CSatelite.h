#ifndef CSATELITE_H
#define CSATELITE_H
#include <engine/galaxy/common/CGalaxySpriteObject.h>
#include <engine/galaxy/common/ai/CMoveTarget.h>
#include <engine/galaxy/common/ai/CPlayerWM.h>

namespace galaxy
{

class CSatelite : public CGalaxySpriteObject, public CMoveTarget
{
    
public:
	CSatelite(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * When it is moving normally
	 */
	void processFlying();
	
	bool calcVisibility();
	
	void process();

	/**
	 * What happens if it gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
private:
	CPlayerWM *mpCarriedPlayer;
	int mTilesUntilumount;
	VectorD2<int> mPlayerPos;
};

};

#endif // CSATELITE_H
