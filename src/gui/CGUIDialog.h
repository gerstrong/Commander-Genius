/*
 * CDialog.h
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *
 *  This class is the Dialog which will be in
 *  use for the GUIs of Commander Genius
 */

#ifndef CGUIDIALOG_H_
#define CGUIDIALOG_H_

#include "CResourceLoader.h"
#include "utils/Geometry.h"

class CGUIDialog
{

public:

	// Constructor which needs the Rect for the placement of the Dialog
	CGUIDialog(const CRect &NewRect);

	// processes the whole logic of the Dialog
	void processLogic();

	// processes the whole rendering of the Dialog
	void processRendering();

private:
	CRect mRect;

};

#endif /* CGUIDIALOG_H_ */
