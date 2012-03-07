/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "common/CSettings.h"
#include "sdl/input/CInput.h"
#include "sdl/CTimer.h"
#include "sdl/resolutionlist.h"
#include "CVideoSettings.h"
#include "CSettingsMenu.h"
#include "StringUtils.h"
#include "Utils.h"
#include "CMenuController.h"



class toggleFullscreenFunctor : public InvokeFunctorEvent
{
public:
	toggleFullscreenFunctor( CVideoSettings& VSettings ) :
		mVSettings(VSettings) {}

private:

	void operator()()
	{
		mVSettings.mUserVidConf.Fullscreen = !mVSettings.mUserVidConf.Fullscreen;
		mVSettings.release();
		mVSettings.init();
	}

	CVideoSettings& mVSettings;
};


CVideoSettings::CVideoSettings(const Uint8 dlg_theme) :
CBaseMenu(dlg_theme, CRect<float>(0.15f, 0.24f, 0.7f, 0.5f) )
{
	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	// Create the fps config selection control
	std::list<std::string>	List;
	for( int i = 10 ; i <= 120 ; i += 10 )
		List.push_back( itoa (i) );

	mpFPSSelection = new CGUINumberControl( "FPS",
											 10, 120, 10, 60,
											 CGUINumberControl::VORTICON );
	mpMenuDialog->addControl( mpFPSSelection );


#ifdef USE_OPENGL

	mpOpenGLSwitch = new CGUISwitch( "OpenGL",
									  CGUISwitch::VORTICON );
	mpMenuDialog->addControl( mpOpenGLSwitch );
#endif

	mpOGLFilterSelection = new CGUIComboSelection( "OGL Filter",
											filledStrList( 2, "nearest", "linear" ),
	 	 	 	 	 	 	 	 	 	 	 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpOGLFilterSelection );


	mpScalerSelection = new CGUIComboSelection( "Scaler",
												filledStrList( 4, "none", "2x", "3x", "4x" ),
												CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpScalerSelection );


	mpShowFPSSwitch = new CGUISwitch( "Show FPS",
									  	 CGUISwitch::VORTICON );
	mpMenuDialog->addControl( mpShowFPSSwitch );


	mpSFXSwitch = new CGUISwitch( "Special FX",
								  CGUISwitch::VORTICON );
	mpMenuDialog->addControl( mpSFXSwitch );


	mpCameraButton = new CGUIButton( "Camera",
									new OpenMenuEvent( new CCameraSettings(dlg_theme) ),
									CGUIButton::VORTICON );
	mpMenuDialog->addControl( mpCameraButton );



	mpFullScreenSwitch = new CGUIButton( "Unknown mode",
										new toggleFullscreenFunctor(*this),
										CGUIButton::VORTICON );
	mpMenuDialog->addControl( mpFullScreenSwitch );

	mpResolutionSelection = new CGUIComboSelection( "Mode",
													filledStrList(1, "?x?"),
													CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpResolutionSelection );

}

void CVideoSettings::init()
{
	std::string OGLFilterStr;
	mUserVidConf = g_pVideoDriver->getVidConfig();

	// Load the config into the GUI
	mpOGLFilterSelection->setSelection( mUserVidConf.m_opengl_filter==1 ? "nearest" : "linear" );
	mpFPSSelection->setSelection( g_pTimer->getFrameRate() );
	mpOpenGLSwitch->enable( mUserVidConf.m_opengl );
	mpScalerSelection->setSelection( mUserVidConf.m_ScaleXFilter==1 ? "none" : itoa(mUserVidConf.m_ScaleXFilter) + "x" );
	mpShowFPSSwitch->enable( mUserVidConf.showfps );
	mpSFXSwitch->enable( mUserVidConf.m_special_fx );
	mpFullScreenSwitch->setText( mUserVidConf.Fullscreen ? "Go Windowed" : "Go Fullscreen" );

	// only show resolutions if we are in Fullscreen!
	mpResolutionSelection->setList( ResolutionsList, NUM_MAIN_RESOLUTIONS );
	std::string resStr;
	resStr = itoa(mUserVidConf.m_DisplayRect.w);
	resStr += "x";
	resStr += itoa(mUserVidConf.m_DisplayRect.h);
	mpResolutionSelection->setSelection(resStr);
}


void CVideoSettings::release()
{
	// Save up the changed stuff

	g_pTimer->setFPS( mpFPSSelection->getSelection() );

#ifdef USE_OPENGL
	g_pVideoDriver->setOGLFilter( mpOGLFilterSelection->getSelection() == "nearest" ? 0 : 1 );
#endif
	g_pVideoDriver->enableOpenGL( mpOpenGLSwitch->isEnabled() );

	std::string scalerStr = mpScalerSelection->getSelection();
	if( scalerStr != "none" )
		mUserVidConf.m_ScaleXFilter = scalerStr.at(0)-'0';
	else
		mUserVidConf.m_ScaleXFilter = 1;

	mUserVidConf.showfps = mpShowFPSSwitch->isEnabled();
	mUserVidConf.m_special_fx = mpSFXSwitch->isEnabled();


	CVidConfig oldVidConf = g_pVideoDriver->getVidConfig();
	g_pVideoDriver->setVidConfig(mUserVidConf);

	// At this point we also must apply and save the settings
	if( !g_pVideoDriver->applyMode() )
	{
		g_pSettings->loadDrvCfg();
		return;
	}

	g_pVideoDriver->stop();

	if( !g_pVideoDriver->start() )
	{
		g_pVideoDriver->setVidConfig(oldVidConf);
		g_pVideoDriver->stop();
		g_pVideoDriver->start();
	}

	g_pSettings->saveDrvCfg();

}
