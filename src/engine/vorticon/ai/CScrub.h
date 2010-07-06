/*
 * CScrub.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CSCRUB_H_
#define CSCRUB_H_

#include "../../../common/CObject.h"

class CScrub : public CObject
{
public:
	CScrub(CMap *p_map);
	void process();
private:
};

#endif /* CSCRUB_H_ */
