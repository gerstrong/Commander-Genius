/*
 * CSlugSlime.h
 *
 *  Created on: 28.05.2011
 *      Author: gerstrong
 *
 *  This header handles the slug poo. It just shows it and make it disappear
 *  In this time it is lethal of course.
 */

#ifndef CSLUGSLIME_H_
#define CSLUGSLIME_H_

#include "../../common/CGalaxySpriteObject.h"

namespace galaxy
{

class CSlugSlime : public CGalaxySpriteObject
{
public:

	/** Standard Constructor for the slug poo */
    CSlugSlime(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar);

	/** Main Process cycle */
	void process();

	/** What happens when something touches that slime */
	void getTouchedBy(CSpriteObject &theObject);
private:
    int m_timer;

};

};

#endif /* CSLUGSLIME_H_ */
