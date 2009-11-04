/*
 * CEndingEp1.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CENDINGEP1_H_
#define CENDINGEP1_H_

#include "CFinale.h"
#include <SDL.h>

class CEndingEp1 : public CFinale {
public:
	CEndingEp1();
	void process();

	void ReturnsToShip();
	void ShipFlys(bool flyback);
	void BackAtHome();

	virtual ~CEndingEp1();

private:
	char m_step;
	Uint32 m_starttime;
	Uint32 m_timepassed;
};

#endif /* CENDINGEP1_H_ */
