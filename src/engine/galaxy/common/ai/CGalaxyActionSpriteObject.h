#ifndef CGALAXYACTIONSPRITEOBJECT_H
#define CGALAXYACTIONSPRITEOBJECT_H

#include <engine/galaxy/common/CGalaxySpriteObject.h>

namespace galaxy
{

  
#define GASOFctr void(CGalaxyActionSpriteObject::*)()  
  

class CGalaxyActionSpriteObject : public CGalaxySpriteObject
{
public:
	CGalaxyActionSpriteObject(CMap *pmap,
				const Uint16 foeID,
				Uint32 x,
				Uint32 y );

	void setActionForce(const size_t ActionNumber);

protected:
	void (CGalaxyActionSpriteObject::*mp_processState)();
	std::map< size_t, GASOFctr > mActionMap;	
};

};

#endif // CGALAXYACTIONSPRITEOBJECT_H
