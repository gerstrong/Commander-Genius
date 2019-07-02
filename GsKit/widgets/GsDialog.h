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
#include "GsBitmapBox.h"

#include "GsControlsManager.h"

class CGUIDialog : public GsControlsManager
{    

public:

    enum class FXKind
    {
        NONE,
        EXPAND
    };


	// Constructor which needs the Rect for the placement of the Dialog
    CGUIDialog(const GsRect<float> &NewRect, const FXKind fx = FXKind::NONE);

    bool sendEvent(const std::shared_ptr<CEvent> &event ) override;

	// Empty Background draw function. Just fills the Rect with a standard gray color
    void initEmptyBackground();

	// Vorticon Menu draw function
    void initVorticonBackground();

    // This draws the borders around the dialog
    void drawBorderRect(SDL_Surface *backSfc, const SDL_Rect &Rect);

	// processes the whole logic of the Dialog
    virtual void processLogic() override;

    // processes the whole rendering of theCGUIDialog Dialog
    void processRendering();    

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

	// processes the whole rendering of the Dialog
    void processRendering(SDL_Surface*);

    void setRect(const GsRect<float> &rect)
    {
        GsWidget::setRect(rect);
    }


protected:

    GsSurface    mBackgroundSfc;

    GsSurface    mDarkOverlaySfc;

private:

	// SDL_Surface of the Background
    GsSurface mTempSfc;

    // Unused in Galaxy,
    // this is for some dialog
    // effects used in Vorticons
    FXKind mFXSetup = FXKind::NONE;
    int mFXhStep = 0;
    int mFXvStep = 0;
};



#endif /* CGUIDIALOG_H_ */
