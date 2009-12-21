/*
 * CTantalusRay.h
 *
 *  Created on: 21.12.2009
 *      Author: gerstrong
 */

#ifndef CTANTALUSRAY_H_
#define CTANTALUSRAY_H_

#include "CFinale.h"
#include "../../dialog/CMessageBox.h"

class CTantalusRay : public CFinale
{
public:
	CTantalusRay();
	void process();
	virtual ~CTantalusRay();

private:
	CMessageBox *mp_MessageBox;
};

#endif /* CTANTALUSRAY_H_ */
