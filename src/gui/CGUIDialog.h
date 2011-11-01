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
#include "CGUIControl.h"
#include "SmartPointer.h"
#include <list>

class CGUIDialog
{

public:

	// Constructor which needs the Rect for the placement of the Dialog
	CGUIDialog(const CRect<float> &NewRect);

	// processes the whole logic of the Dialog
	void processLogic();

	// processes the whole rendering of the Dialog
	void processRendering();

	// Adds a control instance to the list of controls to be processed.
	void addControl(CGUIControl* newControl);

private:
	CRect<float> mRect;

	// List of Controls that the Dialog has.
	std::list< SmartPointer<CGUIControl> > mControlList;

};

#endif /* CGUIDIALOG_H_ */
