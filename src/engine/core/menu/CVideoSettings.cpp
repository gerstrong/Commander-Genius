/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
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


CVideoSettings::CVideoSettings(const Style style) :
#if defined(EMBEDDED)
GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.25f), style )
#else
GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.55f), style )
#endif
{


    mpShowCursorSwitch =
            mpMenuDialog->add( new Switch("Cursor", style) );

    mpTiltScreenSwitch =
            mpMenuDialog->add( new Switch("TiltedScr", style) );

    mpFPSSelection =
            mpMenuDialog->add(
                new NumberControl( "FPS", 10, 120, 10, 60,
                                   false, style) );


    mpFrameSkip =
            mpMenuDialog->add( new Switch( "FrameSkip", style ) );


    mpGameResSelection =
            mpMenuDialog->add( new ComboSelection( "GameRes",
                                                   filledStrList(1, "?x?"),
                                                   style ) );

    mpBorderColorSwitch =
            mpMenuDialog->add( new Switch( "Brdr Color", style ) );

    mpHorizBordersSelection =
            mpMenuDialog->add( new NumberControl( "H-Brdr",
                                            0, 80, 5, 0,
                                            false,
                                            style) );

	setMenuLabel("OPTIONSMENULABEL");

    mpMenuDialog->fit();

    refresh();
    select(1);
}

void CVideoSettings::refresh()
{
    mUsersConf = gVideoDriver.getVidConfig();
    
	// Load the config into the GUI
	// TODO: Temporary. This must become a float later...
    const auto iFPS = static_cast<int>( gTimer.FPS() );

    // Only enable this option when VSync is turned off
    mpFPSSelection->enable(!mUsersConf.mVSync);

    mpFPSSelection->setSelection( iFPS );
    mpFrameSkip->enable( (iFPS> 0.0) ? true : false );

    mpShowCursorSwitch->enable( mUsersConf.mShowCursor );
    mpTiltScreenSwitch->enable( mUsersConf.mTiltedScreen );

    // TODO: find a way to indicate a color
    mpBorderColorSwitch->enable( mUsersConf.mBorderColorsEnabled );

    mpHorizBordersSelection->setSelection( mUsersConf.mHorizBorders );

    const auto gamesResSet = gVideoDriver.getGameResStrSet();
    mpGameResSelection->setList( gamesResSet );

    std::string resStr;
    resStr = itoa(mUsersConf.mGameRect.dim.x);
    resStr += "x";
    resStr += itoa(mUsersConf.mGameRect.dim.y);
    mpGameResSelection->setSelection(resStr);

}


void CVideoSettings::release()
{
	// Save up the changed stuff
    mUsersConf.mShowCursor = mpShowCursorSwitch->isEnabled();
    mUsersConf.mTiltedScreen = mpTiltScreenSwitch->isEnabled();

    const auto fpsf = float(mpFPSSelection->getSelection());
    gTimer.setFPS( fpsf );

    if(!mpFrameSkip->isEnabled())
    {
        gTimer.setFPS( 0.0 );
    }

    mUsersConf.mHorizBorders = mpHorizBordersSelection->getSelection();

    mUsersConf.mBorderColorsEnabled = mpBorderColorSwitch->isEnabled();
	
    const std::string GameResStr = mpGameResSelection->getSelection();
    sscanf( GameResStr.c_str(), "%hux%hux",
            &mUsersConf.mGameRect.dim.x, &mUsersConf.mGameRect.dim.y );


#if defined(CAANOO) || defined(WIZ) || defined(DINGOO) || defined(NANONOTE) || defined(ANDROID)
    mUsersConf.mDisplayRect.dim.x = 320;
    mUsersConf.mDisplayRect.dim.y = 200;
#endif

    // TODO: Find a better way to setup colors in the menu
    if(mpBorderColorSwitch->isEnabled())
    {
        mUsersConf.mBorderColors.r = 0x00;
        mUsersConf.mBorderColors.g = 0xAA;
        mUsersConf.mBorderColors.b = 0xAA;
    }

    mUsersConf.mBorderColorsEnabled = mpBorderColorSwitch->isEnabled();

    mUsersConf.mHorizBorders = mpHorizBordersSelection->getSelection();

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
    gVideoDriver.setRefreshSignal(true);
}
