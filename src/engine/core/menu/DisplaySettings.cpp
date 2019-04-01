
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
        mDispSettings.mMyNewConf.mFullscreen = !mDispSettings.mMyNewConf.mFullscreen;
		mDispSettings.release();
	}

    virtual ~toggleFullscreenFunctor();

	DisplaySettings& mDispSettings;
};

toggleFullscreenFunctor::~toggleFullscreenFunctor()
{}


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
    // Copy current config to my new Config.
    // The change are taken from the menu settings
    mMyNewConf = gVideoDriver.getVidConfig();


#if !defined(EMBEDDED)

    #if defined(USE_OPENGL)
    mpOpenGLSwitch->enable( mMyNewConf.mOpengl );
    #endif


    mpRenderScaleQualitySel->setSelection(mMyNewConf.mRenderScQuality);


	mpAspectSelection->setList( aspectList, NUM_ASPECTS );		
	std::string arcStr;
    arcStr = itoa(mMyNewConf.mAspectCorrection.w);
	arcStr += ":";
    arcStr += itoa(mMyNewConf.mAspectCorrection.h);
	
	if( arcStr == "0:0")
	  arcStr = "disabled";
	
	mpAspectSelection->setSelection(arcStr);

	
    mpFilterSelection->setSelection( mMyNewConf.m_ScaleXFilter==1 ? "none" :
                                    (mMyNewConf.m_normal_scale ? "normal" : "scale") +
                                        itoa(mMyNewConf.m_ScaleXFilter) + "x" );
    mpVSyncSwitch->enable( mMyNewConf.mVSync );
    mpFullScreenSwitch->setText( mMyNewConf.mFullscreen ? "Go Windowed" : "Go Fullscreen" );


    mpResolutionSelection->setList( ResolutionsList, NUM_MAIN_RESOLUTIONS );

	std::string resStr;

    resStr = itoa(mMyNewConf.mDisplayRect.w);
	resStr += "x";
    resStr += itoa(mMyNewConf.mDisplayRect.h);
	mpResolutionSelection->setSelection(resStr);        

#endif

}


void DisplaySettings::release()
{

#if !defined(EMBEDDED)

    // OpenGL Flag
    mMyNewConf.mOpengl = mpOpenGLSwitch->isEnabled();

    // Render Quality
    mMyNewConf.mRenderScQuality = mpRenderScaleQualitySel->getSelection();

    // Read Aspect correction string
    {
        const std::string arcStr = mpAspectSelection->getSelection();

        const int numRead = sscanf(arcStr.c_str(),"%i:%i",
                               &mMyNewConf.mAspectCorrection.w,
                               &mMyNewConf.mAspectCorrection.h);

        if(numRead < 2)
        {
            mMyNewConf.mAspectCorrection.w = mMyNewConf.mAspectCorrection.h = 0;
        }
    }

    // Filter
    {
        mMyNewConf.m_ScaleXFilter = NONE;

        const std::string filterText = mpFilterSelection->getSelection();

        if(filterText == "normal2x")
        {
            mMyNewConf.m_normal_scale = true;
            mMyNewConf.m_ScaleXFilter = SCALE_2X;
        }
        if(filterText == "normal3x")
        {
            mMyNewConf.m_normal_scale = true;
            mMyNewConf.m_ScaleXFilter = SCALE_3X;
        }
        if(filterText == "normal4x")
        {
            mMyNewConf.m_normal_scale = true;
            mMyNewConf.m_ScaleXFilter = SCALE_4X;
        }
        if(filterText == "scale2x")
        {
            mMyNewConf.m_normal_scale = false;
            mMyNewConf.m_ScaleXFilter = SCALE_2X;
        }
        if(filterText == "scale3x")
        {
            mMyNewConf.m_normal_scale = false;
            mMyNewConf.m_ScaleXFilter = SCALE_3X;
        }
        if(filterText == "scale4x")
        {
            mMyNewConf.m_normal_scale = false;
            mMyNewConf.m_ScaleXFilter = SCALE_4X;
        }
    }

    // Vsync
    mMyNewConf.mVSync = mpVSyncSwitch->isEnabled();

    // Fullscreen
    const auto fsBtnText = mpFullScreenSwitch->getText();
    mMyNewConf.mFullscreen = (fsBtnText == "Go Windowed") ? true : false;


    // Read correct resolution
    {
        const std::string resStr = mpResolutionSelection->getSelection();
        int w, h;

        const int numRead = sscanf(resStr.c_str(),"%ix%i", &w, &h);
        std::cerr << "numRead: " << numRead << std::endl;
        if(numRead == 2)
        {
            GsRect<Uint16> res((Uint16(w)),
                               (Uint16(h)));
            mMyNewConf.setResolution(res);
        }
    }

	// In case the user changed something in the camera settings, reload that.
    mMyNewConf.m_CameraBounds = gVideoDriver.getCameraBounds();

#endif

	CVidConfig oldVidConf = gVideoDriver.getVidConfig();
    gVideoDriver.setVidConfig(mMyNewConf);




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

