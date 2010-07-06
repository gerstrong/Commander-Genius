/*
 * CPlatform.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CPLATFORM_H_
#define CPLATFORM_H_

#include "../../../common/CObject.h"

class CPlatform : public CObject
{
public:
	CPlatform(CMap *p_map, Uint32 x, Uint32 y);
	virtual void process();
private:
};

class CPlatformVert : public CPlatform
{
public:
	CPlatformVert(CMap *p_map, Uint32 x, Uint32 y);
	void process();
private:
};

#endif /* CPLATFORM_H_ */
