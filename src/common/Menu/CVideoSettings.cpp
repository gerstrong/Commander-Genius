/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "hardware/Configurator.h"
#include "common/CSettings.h"
#include "CCameraSettings.h"
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


CVideoSettings::CVideoSettings() :
#if defined(EMBEDDED)
CBaseMenu(CRect<float>(0.15f, 0.24f, 0.65f, 0.25f) )
#else
CBaseMenu(CRect<float>(0.15f, 0.24f, 0.65f, 0.55f) )
#endif
{
	// Create the fps config selection control
	std::list<std::string>	List;
	for( int i = 10 ; i <= 120 ; i += 10 )
		List.push_back( itoa (i) );

	mpFPSSelection = new CGUINumberControl( "FPS", 10, 120, 10, 60, false );
	mpMenuDialog->addControl( mpFPSSelection );


#if defined(USE_OPENGL)
	mpOpenGLSwitch = new CGUISwitch( "OpenGL" );
	mpMenuDialog->addControl( mpOpenGLSwitch );

	mpOGLFilterSelection = new CGUIComboSelection( "OGL Filter",
		filledStrList( 2, "nearest", "linear" ) );
	mpMenuDialog->addControl( mpOGLFilterSelection );
#endif

	mpShowFPSSwitch = new CGUISwitch( "Show FPS" );
	mpMenuDialog->addControl( mpShowFPSSwitch );


	mpSFXSwitch = new CGUISwitch( "Special FX" );
	mpMenuDialog->addControl( mpSFXSwitch );

	mpAspectSwitch = new CGUISwitch( "Aspect Correct" );
	mpMenuDialog->addControl( mpAspectSwitch );

#if !defined(EMBEDDED)
	mpScalerSelection = new CGUIComboSelection( "Scaler",
		filledStrList( 7, "none", "normal2x", "normal3x", "normal4x", "scale2x", "scale3x", "scale4x" ) );
	mpMenuDialog->addControl( mpScalerSelection );



	mpVSyncSwitch = new CGUISwitch( "VSync" );
	mpMenuDialog->addControl( mpVSyncSwitch );

	mpResolutionSelection = new CGUIComboSelection( "Mode",
		filledStrList(1, "?x?") );
	mpMenuDialog->addControl( mpResolutionSelection );



	mpFullScreenSwitch = new CGUIButton( "Unknown mode",
										new toggleFullscreenFunctor(*this) );
	mpMenuDialog->addControl( mpFullScreenSwitch );
#endif

	setMenuLabel("CONFIGMENULABEL");
}

void CVideoSettings::init()
{
	mUserVidConf = g_pVideoDriver->getVidConfig();

	// Load the config into the GUI
	// TODO: Temporary. This must become a float later...
	mpFPSSelection->setSelection( static_cast<int>( g_pTimer->FPS()) );

#if defined(USE_OPENGL)
	std::string OGLFilterStr;
	mpOpenGLSwitch->enable( mUserVidConf.m_opengl );
	mpOGLFilterSelection->setSelection( mUserVidConf.m_opengl_filter==GL_LINEAR ? "linear" : "nearest" );
	mpOGLFilterSelection->enable( mUserVidConf.m_opengl );
#endif

	mpShowFPSSwitch->enable( mUserVidConf.showfps );
	mpSFXSwitch->enable( mUserVidConf.m_special_fx );

	mpAspectSwitch->enable( mUserVidConf.m_aspect_correction );

#if !defined(EMBEDDED)
	mpScalerSelection->setSelection( mUserVidConf.m_ScaleXFilter==1 ? "none" : (mUserVidConf.m_normal_scale ? "normal" : "scale") + itoa(mUserVidConf.m_ScaleXFilter) + "x" );
	mpVSyncSwitch->enable( mUserVidConf.vsync );
	mpFullScreenSwitch->setText( mUserVidConf.Fullscreen ? "Go Windowed" : "Go Fullscreen" );

	mpResolutionSelection->setList( ResolutionsList, NUM_MAIN_RESOLUTIONS );
	std::string resStr;
	resStr = itoa(mUserVidConf.m_DisplayRect.w);
	resStr += "x";
	resStr += itoa(mUserVidConf.m_DisplayRect.h);
	mpResolutionSelection->setSelection(resStr);
#endif

}


void CVideoSettings::release()
{
	// Save up the changed stuff

	g_pTimer->setFPS( mpFPSSelection->getSelection() );

#if defined(USE_OPENGL)
	mUserVidConf.m_opengl_filter = mpOGLFilterSelection->getSelection() == "linear" ? GL_LINEAR : GL_NEAREST;
	mUserVidConf.m_opengl = mpOpenGLSwitch->isEnabled();
#endif

#if !defined(EMBEDDED)
	mUserVidConf.m_aspect_correction = mpAspectSwitch->isEnabled();
	mUserVidConf.vsync = mpVSyncSwitch->isEnabled();
	std::string scalerStr = mpScalerSelection->getSelection();

	const std::string res = mpResolutionSelection->getSelection();
	sscanf( res.c_str(), "%hux%hux", &mUserVidConf.m_DisplayRect.w, &mUserVidConf.m_DisplayRect.h );

	if( scalerStr != "none" )
	{
		mUserVidConf.m_normal_scale = (scalerStr.at(0) == 'n');
		if (mUserVidConf.m_normal_scale)
			mUserVidConf.m_ScaleXFilter = scalerStr.at(6)-'0';
		else
			mUserVidConf.m_ScaleXFilter = scalerStr.at(5)-'0';
	}
	else
		mUserVidConf.m_ScaleXFilter = 1;
#endif

#if defined(CAANOO) || defined(WIZ) || defined(DINGOO) || defined(NANONOTE) || defined(ANDROID)
	mUserVidConf.m_DisplayRect.w = 320;
	mUserVidConf.m_DisplayRect.h = 200;
#endif

	mUserVidConf.showfps = mpShowFPSSwitch->isEnabled();
	mUserVidConf.m_special_fx = mpSFXSwitch->isEnabled();

	// In case the user changed something in the camera settings, reload that.
	mUserVidConf.m_CameraBounds = g_pVideoDriver->getCameraBounds();

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
