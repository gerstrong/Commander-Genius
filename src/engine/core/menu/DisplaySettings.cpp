
/*
 * Display.cpp
 *
 *  Created on: 09.01.2018
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <base/GsTimer.h>
#include <base/video/resolutionlist.h>
#include <base/video/CVideoDriver.h>
#include <base/utils/StringUtils.h>
#include <widgets/GsMenuController.h>

#include "engine/core/CBehaviorEngine.h"
#include "CVideoSettings.h"
#include <base/utils/Utils.h>

#include "widgets/NumberControl.h"
#include "widgets/ComboSelection.h"

#include "engine/core/CSettings.h"
#include "engine/core/videoAspect.h"

#include "DisplaySettings.h"


class toggleFullscreenFunctor : public InvokeFunctorEvent
{
public:
	toggleFullscreenFunctor( DisplaySettings& DispSettings ) :
		mDispSettings(DispSettings) {}

private:

    void operator()() const
	{
        mDispSettings.mUsersConf.mFullscreen = !mDispSettings.mUsersConf.mFullscreen;
		mDispSettings.release();
	}

	DisplaySettings& mDispSettings;
};



DisplaySettings::DisplaySettings(const GsControl::Style style) :
#if defined(EMBEDDED)
GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.25f), style )
#else
GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.55f), style )
#endif
{

#if !defined(EMBEDDED)

	
    mpAspectSelection = new ComboSelection( "Aspect",
		                                    filledStrList(1, "disabled"),
                                            style );
	
	mpMenuDialog->addControl( mpAspectSelection );

    mpFilterSelection =
            new ComboSelection( "Filter",
                                filledStrList( 4, 
                                               "none",
                                               "scale2x",
                                               "scale3x",
                                               "scale4x" ),
                                style );

    mpMenuDialog->addControl( mpFilterSelection );

    mpVSyncSwitch = new Switch( "VSync", style );
	mpMenuDialog->addControl( mpVSyncSwitch );

    mpResolutionSelection = new ComboSelection( "Size",
                                                filledStrList(1, "?x?"),
                                                style);
    mpMenuDialog->addControl( mpResolutionSelection );

    mpFullScreenSwitch = new GameButton( "Unknown mode",
										new toggleFullscreenFunctor(*this),
                                         style);
	mpMenuDialog->addControl( mpFullScreenSwitch );
#endif

#if defined(USE_OPENGL)
    mpOpenGLSwitch = new Switch( "OpenGL", style );
    mpMenuDialog->addControl( mpOpenGLSwitch );

    mpRenderScaleQualitySel = new ComboSelection( "Quality",
                                                  filledStrList( 2, 
                                                                 "nearest", 
                                                                 "linear" ),
                                                  style);

#else
    mpRenderScaleQualitySel = new ComboSelection( "Quality",
                                                  filledStrList( 3, 
                                                                 "nearest", 
                                                                 "linear", 
                                                                 "best" ),
                                                  style);
#endif

    mpMenuDialog->addControl( mpRenderScaleQualitySel );

	setMenuLabel("OPTIONSMENULABEL");

    refresh();
}

void DisplaySettings::refresh()
{
    mUsersConf = gVideoDriver.getVidConfig();

#if defined(USE_OPENGL)    
    mpOpenGLSwitch->enable( mUsersConf.mOpengl );
#endif


    mpRenderScaleQualitySel->setSelection(mUsersConf.mRenderScQuality);



#if !defined(EMBEDDED)
	mpAspectSelection->setList( aspectList, NUM_ASPECTS );		
	std::string arcStr;
    arcStr = itoa(mUsersConf.mAspectCorrection.w);
	arcStr += ":";
    arcStr += itoa(mUsersConf.mAspectCorrection.h);
	
	if( arcStr == "0:0")
	  arcStr = "disabled";
	
	mpAspectSelection->setSelection(arcStr);

	
    mpFilterSelection->setSelection( mUsersConf.m_ScaleXFilter==1 ? "none" : (mUsersConf.m_normal_scale ? "normal" : "scale") + itoa(mUsersConf.m_ScaleXFilter) + "x" );
    mpVSyncSwitch->enable( mUsersConf.mVSync );
    mpFullScreenSwitch->setText( mUsersConf.mFullscreen ? "Go Windowed" : "Go Fullscreen" );


    mpResolutionSelection->setList( ResolutionsList, NUM_MAIN_RESOLUTIONS );

	std::string resStr;

    resStr = itoa(mUsersConf.mDisplayRect.w);
	resStr += "x";
    resStr += itoa(mUsersConf.mDisplayRect.h);
	mpResolutionSelection->setSelection(resStr);        

#endif

}


void DisplaySettings::release()
{
    // Read Aspect correction string
    {
        const std::string arcStr = mpAspectSelection->getSelection();

        const int numRead = sscanf(arcStr.c_str(),"%i:%i",
                               &mUsersConf.mAspectCorrection.w,
                               &mUsersConf.mAspectCorrection.h);

        if(numRead < 2)
        {
            mUsersConf.mAspectCorrection.w = mUsersConf.mAspectCorrection.h = 0;
        }
    }


	// In case the user changed something in the camera settings, reload that.
    mUsersConf.m_CameraBounds = gVideoDriver.getCameraBounds();

	CVidConfig oldVidConf = gVideoDriver.getVidConfig();
    gVideoDriver.setVidConfig(mUsersConf);

	// At this point we also must apply and save the settings
	if( !gVideoDriver.applyMode() )
	{
        gSettings.loadDrvCfg(); // If it fails load the old settings
		return;
	}		

    if( !gVideoDriver.start() ) // Here the same situation
	{
		gVideoDriver.setVidConfig(oldVidConf);
		gVideoDriver.start();
	}
	
	gSettings.saveDrvCfg();

    gMenuController.updateGraphics();
}

