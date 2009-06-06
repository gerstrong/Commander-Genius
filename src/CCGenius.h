/*
 * CCGenius.h
 *
 *  Created on: 18.05.2009
 *      Author: gerstrong
 */

#ifndef CCGENIUS_H_
#define CCGENIUS_H_

#include "CSingleton.h"
#define g_pCGenius CCGenius::Get()

class CCGenius : public CSingleton<CCGenius>
{
public:
	CCGenius();
	virtual ~CCGenius();

private:

};

#endif /* CCGENIUS_H_ */
