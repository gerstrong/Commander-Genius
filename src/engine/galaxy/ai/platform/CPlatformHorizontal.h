/*
 * CPlatformHorizontal.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#ifndef CPLATFORMHORIZONTAL_H_
#define CPLATFORMHORIZONTAL_H_

#include "CPlatform.h"

namespace galaxy {

class CPlatformHorizontal : public CPlatform {
public:
	CPlatformHorizontal(CMap *pmap, Uint32 x, Uint32 y,
			std::vector<CObject*>& ObjectPtrs);

	/**
	 * \brief Processes the platforms AI. It's just simple moving around of course
	 */
	void process();
};

}

#endif /* CPLATFORMHORIZONTAL_H_ */
