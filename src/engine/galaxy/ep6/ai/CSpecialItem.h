#ifndef CSPECIALITEM_H
#define CSPECIALITEM_H

#include <engine/galaxy/common/CGalaxySpriteObject.h>

namespace galaxy
{

class CSpecialItem : public CGalaxySpriteObject
{
public:
	CSpecialItem(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int actionoffset);

	/** Normal process routine */
	void process();

	void getTouchedBy(CSpriteObject &theObject);

private:
	std::map<int, std::string>	answermap;    
};

};

#endif // CSPECIALITEM_H
