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

#include <list>
#include <memory>

//#include <engine/CEvent.h>
#include <graphics/GsSurface.h>

#include "GsControl.h"
#include "GsBitmap.h"


class CGUIDialog
{    

public:

    enum FXState
    {
        NONE = 0,
        EXPAND
    };


	// Constructor which needs the Rect for the placement of the Dialog
    CGUIDialog(const GsRect<float> &NewRect, const FXState fx = NONE);

	// Automatic Background draw function. It creates the background depending on what engine is currently running
	void initBackground();

	// Empty Background draw function. Just fills the Rect with a standard gray color
    void initEmptyBackground();

	// Vorticon Menu draw function
    void initVorticonBackground();

    // This draws the borders around the dialog
    void drawBorderRect(SDL_Surface *backSfc, const SDL_Rect &Rect);

	// processes the whole logic of the Dialog
	void processLogic();

    // processes the whole rendering of the Dialog
    void processRendering();

	// processes the whole rendering of the Dialog
    void processRendering(SDL_Surface *blit);

    // Update the background. Happens when the Menu is created or graphical settings are changed within the game
    void setBackground(GsSurface &bgSfc);

    // Update all graphics. Happens when Video settings are normally changed
    void updateGraphics();

	// Adds a control instance to the list of controls to be processed.
    std::shared_ptr<CGUIControl> addControl( std::unique_ptr<CGUIControl> &newControl,
                                             const GsRect<float>& RelRect );

    std::shared_ptr<CGUIControl> addControl(std::unique_ptr<CGUIControl> &newControl);

    std::shared_ptr<CGUIControl> addControl( CGUIControl *newControl,
			 	 const GsRect<float>& RelRect );

    std::shared_ptr<CGUIControl> addControl( CGUIControl *newControl );


    bool sendEvent( const std::shared_ptr<CEvent> &event );

    bool sendEvent( CEvent *pEvent )
	{
        return sendEvent(std::shared_ptr<CEvent>(pEvent));
	}

	void fit();

	void setRect(const GsRect<float> &rect);
	void setPosition(const float x, const float y);

	std::list< std::shared_ptr<CGUIControl> >& getControlList()
	{	return mControlList;	}

	const int Selection() const
	{	return mSelection;	}

    void setSelection(const unsigned int sel);

	CGUIControl* CurrentControl()
	{	return 	mpCurrentCtrl;	}

    void setCurrentControl(CGUIControl* control)
    {	mpCurrentCtrl = control;	}


    bool empty() const
    {
        return mControlList.empty();
    }

    void clear()
    {
        mpCurrentCtrl = nullptr;
        mControlList.clear();
    }

    GsRect<float> getRect() const
    {
        return mRect;
    }

protected:

	void selectPrevItem();

	void selectNextItem();


	GsRect<float> mRect;
    GsSurface    mBackgroundSfc;

private:

	// List of Controls that the Dialog has.
	std::list< std::shared_ptr<CGUIControl> > mControlList;

	// SDL_Surface of the Background
    std::shared_ptr<SDL_Surface>    mpTempSfc;

	int mSelection;
	CGUIControl *mpCurrentCtrl;

    // Unused in Galaxy, this is for some dialog effects which are also used in Vorticons Doskeen
    FXState mFXSetup;
    int mFXhStep;
    int mFXvStep;

};


#endif /* CGUIDIALOG_H_ */
