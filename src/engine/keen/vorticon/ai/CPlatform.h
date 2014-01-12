/*
 * CPlatform.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CPLATFORM_H_
#define CPLATFORM_H_

#include "engine/vorticon/ai/CCarrier.h"
#include "common/CPlayer.h"
#include <vector>

class CPlatform : public CCarrier
{
public:
	CPlatform(CMap *p_map, Uint32 x, Uint32 y);

	void process();

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
};

#endif /* CPLATFORM_H_ */
