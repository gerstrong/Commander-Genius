/*
 * CPlatformDrop.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 *
 *  This is the code of the dropping platforms
 */

#ifndef CPLATFORMDROP_H_
#define CPLATFORMDROP_H_

#include "CPlatform.h"

namespace galaxy {

class CPlatformDrop : public CPlatform {
public:
	CPlatformDrop(CMap *pmap, const Uint32 x, const Uint32 y,
			std::vector<CObject*>& ObjectPtrs);

	/**
	 * \brief Processes the platforms AI. It's just simple moving around of course
	 */
	void process();

private:
	int m_delay_for_drop;
	int m_drop_speed;

};

}

#endif /* CPLATFORMDROP_H_ */
