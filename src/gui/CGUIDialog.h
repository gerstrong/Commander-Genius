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
#include "engine/CEvent.h"
#include "graphics/CBitmap.h"
#include <list>

class CGUIDialog
{

public:

	// Constructor which needs the Rect for the placement of the Dialog
	CGUIDialog(const CRect<float> &NewRect);

	~CGUIDialog();

	// Automatic Background draw function. It creates the background depending on what engine is currently running
	void initBackground();

	// Empty Background draw function. Just fills the Rect with a standard gray color
	void initEmptyBackround();

	// Vorticon Menu draw function
	void initVorticonBackground(SDL_Rect Rect);

	// Galaxy Menu draw function
	void initGalaxyBackround(SDL_Rect Rect);

	// processes the whole logic of the Dialog
	void processLogic();

	// processes the whole rendering of the Dialog
	void processRendering();

	// Adds a control instance to the list of controls to be processed.
	void addControl( const SmartPointer<CGUIControl> newControl,
			 	 	 const CRect<float>& RelRect );

	void addControl( const SmartPointer<CGUIControl> newControl );

	void sendEvent( const SmartPointer<CEvent> &command );

	void fit();

	void setRect(const CRect<float> &rect);
	void setPosition(const float x, const float y);

	std::list< SmartPointer<CGUIControl> >& getControlList()
	{	return mControlList;	}

	const int Selection() const
	{	return mSelection;	}

	CGUIControl* CurrentControl()
	{	return 	mpCurrentCtrl;	}

protected:
	CRect<float> mRect;

private:

	// List of Controls that the Dialog has.
	std::list< SmartPointer<CGUIControl> > mControlList;

	// SDL_Surface of the Background
	SmartPointer<SDL_Surface>	mpBackgroundSfc;

	int mSelection;

	CGUIControl *mpCurrentCtrl;
};


#endif /* CGUIDIALOG_H_ */
