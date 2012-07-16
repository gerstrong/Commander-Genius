/*
 * CWormmouth.h
 *
 *  Created on: 16.07.2012
 *      Author: gerstong
 */

#ifndef CWORMMOUTH_H_
#define CWORMMOUTH_H_

#include "engine/galaxy/ai/CStunnable.h"

namespace galaxy {

class CWormmouth : public CStunnable
{
public:
	CWormmouth(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);
};

} /* namespace galaxy */
#endif /* CWORMMOUTH_H_ */
