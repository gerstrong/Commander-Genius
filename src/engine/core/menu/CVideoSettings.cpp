/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
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


CVideoSettings::CVideoSettings(const GsControl::Style style) :
#if defined(EMBEDDED)
GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.25f), style )
#else
GameMenu(GsRect<float>(0.15f, 0.20f, 0.65f, 0.55f), style )
#endif
{
    mpShowCursorSwitch = new Switch("Cursor", style);
    mpMenuDialog->addControl( mpShowCursorSwitch );

    mpTiltScreenSwitch = new Switch("TiltedScreen", style);
    mpMenuDialog->addControl( mpTiltScreenSwitch );
    
	// Create the fps config selection control
	std::list<std::string>	List;
	for( int i = 10 ; i <= 120 ; i += 10 )
    {
		List.push_back( itoa (i) );
    }

    mpFPSSelection = new NumberControl( "FPS", 10, 120, 10, 60,
                                        false,
                                        style);
    mpMenuDialog->addControl( mpFPSSelection, style );

    mpFrameSkip = new Switch( "FrameSkip", style );
    mpMenuDialog->addControl( mpFrameSkip );


#if !defined(EMBEDDED)

    mpGameResSelection = new ComboSelection( "GameRes",
                                             filledStrList(1, "?x?"),
                                             style );
    mpMenuDialog->addControl( mpGameResSelection );
	
#endif

#ifdef VIRTUALPAD
    mpVPadSwitch  = new Switch( "VirtPad", style );
    mpMenuDialog->addControl( mpVPadSwitch, style );
#endif

    mpBorderColorSwitch = new Switch( "Border Color", style );
    mpMenuDialog->addControl( mpBorderColorSwitch );

    mpHorizBordersSelection = new NumberControl( "H-Borders",
                                                 0, 80, 5, 0,
                                                 false,
                                                 style);
    mpMenuDialog->addControl( mpHorizBordersSelection );

	setMenuLabel("OPTIONSMENULABEL");

    refresh();
}

void CVideoSettings::refresh()
{
    mUsersConf = gVideoDriver.getVidConfig();
    
	// Load the config into the GUI
	// TODO: Temporary. This must become a float later...
    const auto iFPS = static_cast<int>( gTimer.FPS() );
    mpFPSSelection->setSelection( iFPS );
    mpFrameSkip->enable( (iFPS> 0.0) ? true : false );

    mpShowCursorSwitch->enable( mUsersConf.mShowCursor );
    mpTiltScreenSwitch->enable( mUsersConf.mTiltedScreen );

#ifdef VIRTUALPAD
    mpVPadSwitch->enable( mUsersConf.mVPad );
#endif

    // TODO: find a way to indicate a color
    mpBorderColorSwitch->enable( mUsersConf.mBorderColorsEnabled );

    mpHorizBordersSelection->setSelection( mUsersConf.mHorizBorders );

#if !defined(EMBEDDED)

    mpGameResSelection->setList( GamesResList, NUM_GAME_RESOLUTIONS );

    std::string resStr;
    resStr = itoa(mUsersConf.mGameRect.w);
    resStr += "x";
    resStr += itoa(mUsersConf.mGameRect.h);
    mpGameResSelection->setSelection(resStr);

#endif

}


void CVideoSettings::release()
{
	// Save up the changed stuff
    mUsersConf.mShowCursor = mpShowCursorSwitch->isEnabled();
    mUsersConf.mTiltedScreen = mpTiltScreenSwitch->isEnabled();

	gTimer.setFPS( mpFPSSelection->getSelection() );

    if(!mpFrameSkip->isEnabled())
    {
        gTimer.setFPS( 0.0 );
    }

#ifdef VIRTUALPAD
    mUsersConf.mVPad = mpVPadSwitch->isEnabled();
#endif


    mUsersConf.mHorizBorders = mpHorizBordersSelection->getSelection();

    mUsersConf.mBorderColorsEnabled = mpBorderColorSwitch->isEnabled();
	
#if !defined(EMBEDDED)	

#ifdef VIRTUALPAD
    mUsersConf.mVPad = mpVPadSwitch->isEnabled();

    // Disable OpenGL when using Virtual GL
    if(mUsersConf.mVPad)
    {
        mUsersConf.mOpengl = false;
    }
#endif

    const std::string GameResStr = mpGameResSelection->getSelection();
    sscanf( GameResStr.c_str(), "%hux%hux", &mUsersConf.mGameRect.w, &mUsersConf.mGameRect.h );

#endif

#if defined(CAANOO) || defined(WIZ) || defined(DINGOO) || defined(NANONOTE) || defined(ANDROID)
    mUsersConf.mDisplayRect.w = 320;
    mUsersConf.mDisplayRect.h = 200;
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
