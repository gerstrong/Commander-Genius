/*
 * Display.cpp
 *
 *  Created on: 09.01.2018
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <base/GsTimer.h>
#include <base/video/CVideoDriver.h>
#include <base/interface/StringUtils.h>
#include <widgets/GsMenuController.h>
#include <engine/core/GameEngine.h>

#include "engine/core/CBehaviorEngine.h"
#include <base/interface/Utils.h>

#include "widgets/ComboSelection.h"

#include "engine/core/CSettings.h"

#include "DisplaySettings.h"


CGameSpecificSettings::CGameSpecificSettings(const Style style) :
#if defined(EMBEDDED)
    GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.25f), style )
#else
    GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.55f), style )
#endif
{

    mpGameResSelection =
        mpMenuDialog->add( new ComboSelection( "Size",
                                             filledStrList(1, "?x?"),

                                             style) );

    setMenuLabel("OPTIONSMENULABEL");

    mpMenuDialog->fit();
    refresh();
    select(1);
}

void CGameSpecificSettings::refresh()
{
    // Copy current config to my new Config.
    // The change are taken from the menu settings
    mMyNewConf = gVideoDriver.getVidConfig();

    const std::string oglFilter =
        (mMyNewConf.mRenderScQuality == CVidConfig::RenderQuality::LINEAR) ?
            "linear" : "nearest";

    mpRenderScaleQualitySel->setSelection(oglFilter);




#if defined(USE_OPENGL)
    mpOpenGLSwitch->enable( mMyNewConf.mOpengl );
#endif


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

#if !defined(EMBEDDED)
    mpFullScreenSwitch->enable(mMyNewConf.mFullscreen);
#endif

    mpIntegerScalingSwitch->enable(mMyNewConf.mIntegerScaling);


    const auto resList = gVideoDriver.getResolutionStrSet();
    mpResolutionSelection->setList( resList );

    std::string resStr;

    resStr = itoa(mMyNewConf.mDisplayRect.dim.x);
    resStr += "x";
    resStr += itoa(mMyNewConf.mDisplayRect.dim.y);
    mpGameResSelection->setSelection(resStr);

}


void CGameSpecificSettings::release()
{

    // mpGameResSelection
    // TODO: Set proper ini block entry here


    CVidConfig oldVidConf = gVideoDriver.getVidConfig();

    if(oldVidConf == mMyNewConf)
        return;

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
