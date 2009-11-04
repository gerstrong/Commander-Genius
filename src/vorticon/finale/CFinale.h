/*
 * CFinale.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CFINALE_H_
#define CFINALE_H_

class CFinale {
public:
	CFinale();

	void init_ToBeContinued();

	virtual void process() {}

	virtual ~CFinale();
};

#endif /* CFINALE_H_ */
