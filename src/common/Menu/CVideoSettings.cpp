/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "common/CSettings.h"
#include "sdl/input/CInput.h"
#include "sdl/CTimer.h"
#include "CVideoSettings.h"
#include "CSettingsMenu.h"
#include "StringUtils.h"
#include "Utils.h"
#include "CMenuController.h"


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


	mpOGLFilterSelection = new CGUIComboSelection( "OGL Filter",
											filledStrList( 2, "nearest", "linear" ),
	 	 	 	 	 	 	 	 	 	 	 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpOGLFilterSelection );



#ifdef USE_OPENGL

	mpOpenGLSwitch = new CGUISwitch( "OpenGL",
									  CGUISwitch::VORTICON );
	mpMenuDialog->addControl( mpOpenGLSwitch );
#endif


	mpScalerSelection = new CGUIComboSelection( "Scaler",
												filledStrList( 4, "none", "2x", "3x", "4x" ),
												CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpScalerSelection );



	mpFullScreenSwitch = new CGUISwitch( "Fullscreen",
									  	 CGUISwitch::VORTICON );
	mpMenuDialog->addControl( mpFullScreenSwitch );


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

}

void CVideoSettings::init()
{
	CVidConfig &VidConf = g_pVideoDriver->getVidConfig();
	std::string OGLFilterStr;

	// Load the config into the GUI
	mpOGLFilterSelection->setSelection( VidConf.m_opengl_filter==1 ? "nearest" : "linear" );
	mpFPSSelection->setSelection( g_pTimer->getFrameRate() );
	mpOpenGLSwitch->enable( VidConf.m_opengl );
	mpScalerSelection->setSelection( VidConf.m_ScaleXFilter==1 ? "none" : itoa(VidConf.m_ScaleXFilter) + "x" );
	mpFullScreenSwitch->enable( VidConf.Fullscreen );
	mpShowFPSSwitch->enable( VidConf.showfps );
	mpSFXSwitch->enable( VidConf.m_special_fx );
}


void CVideoSettings::release()
{
	// Save up the changed stuff
#ifdef USE_OPENGL
	g_pVideoDriver->setOGLFilter( mpOGLFilterSelection->getSelection() == "nearest" ? 0 : 1 );
#endif
	g_pTimer->setFPS( mpFPSSelection->getSelection() );
	g_pVideoDriver->enableOpenGL( mpOpenGLSwitch->isEnabled() );

	CVidConfig &VidConf = g_pVideoDriver->getVidConfig();
	std::string scalerStr = mpScalerSelection->getSelection();
	if( scalerStr != "none" )
		VidConf.m_ScaleXFilter = scalerStr.at(0)-'0';
	else
		VidConf.m_ScaleXFilter = 1;

	VidConf.Fullscreen = mpFullScreenSwitch->isEnabled();
	VidConf.showfps = mpShowFPSSwitch->isEnabled();
	VidConf.m_special_fx = mpSFXSwitch->isEnabled();


	// At this point we also must apply and save the settings
	if (g_pVideoDriver->applyMode())
	{
		g_pSettings->saveDrvCfg();
	}
	else
	{	// if applying fails reject the changes
		g_pSettings->loadDrvCfg();
	}
}
