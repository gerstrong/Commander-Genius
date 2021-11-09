
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
#include <engine/core/GameEngine.h>

#include "engine/core/CBehaviorEngine.h"
#include "CVideoSettings.h"
#include <base/utils/Utils.h>

#include "widgets/NumberControl.h"
#include "widgets/ComboSelection.h"

#include "engine/core/CSettings.h"

#include "DisplaySettings.h"


DisplaySettings::DisplaySettings(const Style style) :
#if defined(EMBEDDED)
GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.25f), style )
#else
GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.55f), style )
#endif
{

#if !defined(EMBEDDED)


    mpAspectSelection =
            mpMenuDialog->add( new ComboSelection( "Aspect",
                                                          filledStrList(1, "disabled"),
                                                          style ) );

    mpFilterSelection =
            mpMenuDialog->add( new ComboSelection( "Filter",
                                                  filledStrList( 4,
                                                                 "none",
                                                                 "scale2x",
                                                                 "scale3x",
                                                                 "scale4x" ),
                                                  style ) );

    mpVSyncSwitch =
        mpMenuDialog->add( new Switch( "VSync", style ) );

    mpResolutionSelection =
        mpMenuDialog->add( new ComboSelection( "Size",
                                                  filledStrList(1, "?x?"),
                                                  style) );

    mpFullScreenSwitch =
        mpMenuDialog->add( new Switch( "Fullscreen", style ) );

    mpIntegerScalingSwitch =
        mpMenuDialog->add( new Switch( "IntScaling", style ) );

#endif


#if defined(USE_OPENGL)
    mpOpenGLSwitch =
        mpMenuDialog->add( new Switch( "OpenGL", style ) );

    mpRenderScaleQualitySel =
            mpMenuDialog->add( new ComboSelection( "Quality",
                                                          filledStrList( 2,
                                                                         "nearest",
                                                                         "linear" ),
                                                          style) );
#else
    mpRenderScaleQualitySel =
            mpMenuDialog->add( new ComboSelection( "Quality",
                                                          filledStrList( 3,
                                                                         "nearest",
                                                                         "linear",
                                                                         "best" ),
                                                          style) );
#endif

    setMenuLabel("OPTIONSMENULABEL");

    mpMenuDialog->fit();
    refresh();
    select(1);
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


    mpFilterSelection->setSelection( mMyNewConf.m_ScaleXFilter==VidFilter::NONE ? "none" :
                                    (mMyNewConf.m_normal_scale ? "normal" : "scale") +
                                    itoa(int(mMyNewConf.m_ScaleXFilter)) + "x" );
    mpVSyncSwitch->enable( mMyNewConf.mVSync );
    mpFullScreenSwitch->enable(mMyNewConf.mFullscreen);
    mpIntegerScalingSwitch->enable(mMyNewConf.mIntegerScaling);


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

#if defined(USE_OPENGL)
    // OpenGL Flag
    mMyNewConf.mOpengl = mpOpenGLSwitch->isEnabled();
#else
    mMyNewConf.mOpengl = false;
#endif

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
        mMyNewConf.m_ScaleXFilter = VidFilter::NONE;

        const std::string filterText = mpFilterSelection->getSelection();

        if(filterText == "normal2x")
        {
            mMyNewConf.m_normal_scale = true;
            mMyNewConf.m_ScaleXFilter = VidFilter::SCALE_2X;
        }
        if(filterText == "normal3x")
        {
            mMyNewConf.m_normal_scale = true;
            mMyNewConf.m_ScaleXFilter = VidFilter::SCALE_3X;
        }
        if(filterText == "normal4x")
        {
            mMyNewConf.m_normal_scale = true;
            mMyNewConf.m_ScaleXFilter = VidFilter::SCALE_4X;
        }
        if(filterText == "scale2x")
        {
            mMyNewConf.m_normal_scale = false;
            mMyNewConf.m_ScaleXFilter = VidFilter::SCALE_2X;
        }
        if(filterText == "scale3x")
        {
            mMyNewConf.m_normal_scale = false;
            mMyNewConf.m_ScaleXFilter = VidFilter::SCALE_3X;
        }
        if(filterText == "scale4x")
        {
            mMyNewConf.m_normal_scale = false;
            mMyNewConf.m_ScaleXFilter = VidFilter::SCALE_4X;
        }
    }

    // Vsync
    mMyNewConf.mVSync = mpVSyncSwitch->isEnabled();

    // Fullscreen
    mMyNewConf.mFullscreen = mpFullScreenSwitch->isEnabled();

    // Integer Scaling
    mMyNewConf.mIntegerScaling = mpIntegerScalingSwitch->isEnabled();

    // Read correct resolution
    {
        const std::string resStr = mpResolutionSelection->getSelection();
        int w, h;

        const int numRead = sscanf(resStr.c_str(),"%ix%i", &w, &h);
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

    gEventManager.add( new SetNativeResolutionEv() );

    gSettings.saveDrvCfg();

    gMenuController.updateGraphics();
}

