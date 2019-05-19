
/*
 * Display.cpp
 *
 *  Created on: 09.01.2018
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <base/GsTimer.h>
#include <base/video/CVideoDriver.h>
#include <base/utils/StringUtils.h>
#include <widgets/GsMenuController.h>

#include "engine/core/CBehaviorEngine.h"
#include "CVideoSettings.h"
#include <base/utils/Utils.h>

#include "widgets/NumberControl.h"
#include "widgets/ComboSelection.h"

#include "engine/core/CSettings.h"

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

    mpVSyncSwitch =
        mpMenuDialog->addControl( new Switch( "VSync", style ) );

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
    mpOpenGLSwitch =
        mpMenuDialog->addControl( new Switch( "OpenGL", style ) );

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

    mpMenuDialog->fit();

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

    const std::string oglFilter =
            (mMyNewConf.mRenderScQuality == CVidConfig::RenderQuality::LINEAR) ?
            "linear" : "nearest";

    mpRenderScaleQualitySel->setSelection(oglFilter);

    const auto aspSet = gVideoDriver.getAspectStrSet();
    mpAspectSelection->setList( aspSet );
	std::string arcStr;
    arcStr = itoa(mMyNewConf.mAspectCorrection.dim.x);
	arcStr += ":";
    arcStr += itoa(mMyNewConf.mAspectCorrection.dim.y);
	
	if( arcStr == "0:0")
	  arcStr = "disabled";
	
	mpAspectSelection->setSelection(arcStr);

	
    mpFilterSelection->setSelection( mMyNewConf.m_ScaleXFilter==1 ? "none" :
                                    (mMyNewConf.m_normal_scale ? "normal" : "scale") +
                                        itoa(mMyNewConf.m_ScaleXFilter) + "x" );
    mpVSyncSwitch->enable( mMyNewConf.mVSync );
    mpFullScreenSwitch->setText( mMyNewConf.mFullscreen ? "Go Windowed" : "Go Fullscreen" );


    const auto resList = gVideoDriver.getResolutionStrSet();
    mpResolutionSelection->setList( resList );

	std::string resStr;

    resStr = itoa(mMyNewConf.mDisplayRect.dim.x);
	resStr += "x";
    resStr += itoa(mMyNewConf.mDisplayRect.dim.y);
	mpResolutionSelection->setSelection(resStr);        

#endif

}


void DisplaySettings::release()
{

#if !defined(EMBEDDED)

    // OpenGL Flag
    mMyNewConf.mOpengl = mpOpenGLSwitch->isEnabled();

    // Render Quality


    const std::string oglFilter = mpRenderScaleQualitySel->getSelection();

    mMyNewConf.mRenderScQuality =
            (oglFilter == "linear") ?
                CVidConfig::RenderQuality::LINEAR :
                CVidConfig::RenderQuality::NEAREST;

    // Read Aspect correction string
    {
        const std::string arcStr = mpAspectSelection->getSelection();

        const int numRead = sscanf(arcStr.c_str(),"%i:%i",
                               &mMyNewConf.mAspectCorrection.dim.x,
                               &mMyNewConf.mAspectCorrection.dim.y);

        if(numRead < 2)
        {
            mMyNewConf.mAspectCorrection.dim.x = 0;
            mMyNewConf.mAspectCorrection.dim.y = 0;
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
            GsVec2D<Uint16> res = {w, h};
            mMyNewConf.setResolution(res);
        }
    }

	// In case the user changed something in the camera settings, reload that.
    mMyNewConf.m_CameraBounds = gVideoDriver.getCameraBounds();

#endif

	CVidConfig oldVidConf = gVideoDriver.getVidConfig();

    if(oldVidConf == mMyNewConf)
    {
        return;
    }

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

