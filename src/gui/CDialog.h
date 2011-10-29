/*
 * CDialog.h
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *
 *  This class is the Dialog which will be in
 *  use for the GUIs of Commander Genius
 */

#ifndef CDIALOG_H_
#define CDIALOG_H_

#include "CResourceLoader.h"
#include "utils/Geometry.h"

class CDialog
{

public:

	// Constructor which needs the Rect for the placement of the Dialog
	CDialog(const CRect &NewRect);

	// processes the whole logic of the Dialog
	void process();

private:
	CRect mRect;

};

#endif /* CDIALOG_H_ */
