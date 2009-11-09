/*
 * CFinale.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CFINALE_H_
#define CFINALE_H_

#include "CShipFlySys.h"

class CFinale {
public:
	CFinale();

	void init_ToBeContinued();

	virtual void process() {}
	bool getHasFinished() { return m_mustfinishgame; }

	virtual ~CFinale();

protected:
	bool m_mustfinishgame;
};

#endif /* CFINALE_H_ */
