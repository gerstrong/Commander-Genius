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
#include "engine/CEvent.h"
#include "graphics/CBitmap.h"
#include <sdl/extensions.h>
#include <list>
#include <memory>

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
	void addControl( std::unique_ptr<CGUIControl> &newControl,
                    const CRect<float>& RelRect );
    
	void addControl( std::unique_ptr<CGUIControl> &newControl );
    
	void addControl( CGUIControl *newControl,
                    const CRect<float>& RelRect );
    
	void addControl( CGUIControl *newControl );
    
    
	bool sendEvent( const std::shared_ptr<CEvent> &command );
    
	bool sendEvent( CEvent *pCommand )
	{
	    return sendEvent(std::shared_ptr<CEvent>(pCommand));
	}
    
	void fit();
    
	void setRect(const CRect<float> &rect);
	void setPosition(const float x, const float y);
    
	std::list< std::shared_ptr<CGUIControl> >& getControlList()
	{	return mControlList;	}
    
	const int Selection() const
	{	return mSelection;	}
    
	void setSelection(const unsigned int sel);
    
	CGUIControl* CurrentControl()
	{	return 	mpCurrentCtrl;	}
    
protected:
    
	void selectPrevItem();
    
	void selectNextItem();
    
    
	CRect<float> mRect;
    
private:
    
	// List of Controls that the Dialog has.
	std::list< std::shared_ptr<CGUIControl> > mControlList;
    
	// SDL_Surface of the Background
	std::shared_ptr<SDL_Surface>	mpBackgroundSfc;
    
	int mSelection;
    
	CGUIControl *mpCurrentCtrl;
};


#endif /* CGUIDIALOG_H_ */
