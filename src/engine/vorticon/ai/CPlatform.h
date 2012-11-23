/*
 * CPlatform.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CPLATFORM_H_
#define CPLATFORM_H_

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "common/CPlayer.h"
#include <vector>

class CPlatform : public CVorticonSpriteObject
{
public:
	CPlatform(CMap *p_map, Uint32 x, Uint32 y);

	void process();

	void getTouchedBy(CSpriteObject &theObject);

protected:
	unsigned char state;
	unsigned char animframe;
	unsigned int animtimer;
	unsigned int waittimer;

	direction_t movedir;
};

class CPlatformVert : public CPlatform
{
public:
	CPlatformVert(CMap *p_map, Uint32 x, Uint32 y);
	void process();
	void getTouchedBy(CSpriteObject &theObject);
};

#endif /* CPLATFORM_H_ */
