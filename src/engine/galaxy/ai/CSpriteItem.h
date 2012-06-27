/*
 * CSpriteItem.h
 *
 *  Created on: 24.11.2010
 *      Author: gerstrong
 *
 *  In Keen Galaxy there tile items and sprite items. The basically do the same and the only difference
 *  is that the sprite items can be hidden in secret areas. This is not always done strangely.
 *  Both item types exist altough I think we really don't need items, oh well...
 */

#ifndef CSPRITEITEM_H_
#define CSPRITEITEM_H_

#include "engine/galaxy/CGalaxySpriteObject.h"
#include "SmartPointer.h"

namespace galaxy {


class CSpriteItem : public CGalaxySpriteObject
{
public:
	CSpriteItem(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y,
			std::vector< SmartPointer<CGalaxySpriteObject> > &ObjectPtrs, Uint32 sprite);
	void process();
	void getTouchedBy(CSpriteObject &theObject);

private:
	int m_timer;
	Uint32 m_basesprite;
	std::vector< SmartPointer<CGalaxySpriteObject> > &m_ObjectPtrs;
};

}

#endif /* CSPRITEITEM_H_ */
