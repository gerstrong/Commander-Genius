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


//#include "hardware/Configurator.h"
#include "engine/core/CBehaviorEngine.h"
#include "CCameraSettings.h"
#include "CVideoSettings.h"
#include "engine/core/CSettings.h"
//#include "Utils.h"
#include "engine/core/videoAspect.h"


namespace vorticon
{

class toggleFullscreenFunctor : public InvokeFunctorEvent
{
public:
	toggleFullscreenFunctor( CVideoSettings& VSettings ) :
		mVSettings(VSettings) {}

private:

    void operator()() const
	{
		mVSettings.mUserVidConf.Fullscreen = !mVSettings.mUserVidConf.Fullscreen;
		mVSettings.release();
        mVSettings.refresh();
	}

	CVideoSettings& mVSettings;
};


CVideoSettings::CVideoSettings() :
#if defined(EMBEDDED)
VorticonMenu(GsRect<float>(0.15f, 0.24f, 0.65f, 0.25f) )
#else
VorticonMenu(GsRect<float>(0.15f, 0.24f, 0.65f, 0.55f) )
#endif
{
	// Create the fps config selection control
	std::list<std::string>	List;
	for( int i = 10 ; i <= 120 ; i += 10 )
		List.push_back( itoa (i) );

    mpFPSSelection = new NumberControl( "FPS", 10, 120, 10, 60, false );
	mpMenuDialog->addControl( mpFPSSelection );

    mpGameResSelection = new ComboSelection( "GameRes",
        filledStrList(1, "?x?") );
    mpMenuDialog->addControl( mpGameResSelection );
	
#if !defined(EMBEDDED)	
	
    mpAspectSelection = new ComboSelection( "Aspect",
		filledStrList(1, "disabled") );
	
	mpMenuDialog->addControl( mpAspectSelection );

    mpFilterSelection =
            new ComboSelection( "Filter",
                filledStrList( 4, "none",
                                  "scale2x",
                                  "scale3x",
                                  "scale4x" ) );

    mpMenuDialog->addControl( mpFilterSelection );

    mpVSyncSwitch = new Switch( "VSync" );
	mpMenuDialog->addControl( mpVSyncSwitch );

    mpResolutionSelection = new ComboSelection( "Size",
        filledStrList(1, "?x?") );
    mpMenuDialog->addControl( mpResolutionSelection );

    mpFullScreenSwitch = new Button( "Unknown mode",
										new toggleFullscreenFunctor(*this) );
	mpMenuDialog->addControl( mpFullScreenSwitch );
#endif

#if defined(USE_OPENGL)
    mpOpenGLSwitch = new Switch( "OpenGL" );
    mpMenuDialog->addControl( mpOpenGLSwitch );

    mpRenderScaleQualitySel = new ComboSelection( "OGL Filter",
        filledStrList( 2, "nearest", "linear" ) );
    mpMenuDialog->addControl( mpRenderScaleQualitySel );
#endif


    mpSFXSwitch = new Switch( "Special FX" );
    mpMenuDialog->addControl( mpSFXSwitch );
}

void CVideoSettings::refresh()
{
	mUserVidConf = gVideoDriver.getVidConfig();

	// Load the config into the GUI
	// TODO: Temporary. This must become a float later...
	mpFPSSelection->setSelection( static_cast<int>( gTimer.FPS() ) );

#if defined(USE_OPENGL)
    mpOpenGLSwitch->enable( mUserVidConf.m_opengl );
#endif

    mpRenderScaleQualitySel->setSelection(mUserVidConf.mRenderScQuality);

	mpSFXSwitch->enable( mUserVidConf.m_special_fx );	

#if !defined(EMBEDDED)
	//mpAspectSwitch->enable( mUserVidConf.m_aspect_correction );
	mpAspectSelection->setList( aspectList, NUM_ASPECTS );		
	std::string arcStr;
	arcStr = itoa(mUserVidConf.mAspectCorrection.w);
	arcStr += ":";
	arcStr += itoa(mUserVidConf.mAspectCorrection.h);
	
	if( arcStr == "0:0")
	  arcStr = "disabled";
	
	mpAspectSelection->setSelection(arcStr);

	
    mpFilterSelection->setSelection( mUserVidConf.m_ScaleXFilter==1 ? "none" : (mUserVidConf.m_normal_scale ? "normal" : "scale") + itoa(mUserVidConf.m_ScaleXFilter) + "x" );
	mpVSyncSwitch->enable( mUserVidConf.vsync );
	mpFullScreenSwitch->setText( mUserVidConf.Fullscreen ? "Go Windowed" : "Go Fullscreen" );

    mpResolutionSelection->setList( ResolutionsList, NUM_MAIN_RESOLUTIONS );

	std::string resStr;

	resStr = itoa(mUserVidConf.m_DisplayRect.w);
	resStr += "x";
	resStr += itoa(mUserVidConf.m_DisplayRect.h);        
	mpResolutionSelection->setSelection(resStr);        


    mpGameResSelection->setList( GamesResList, NUM_GAME_RESOLUTIONS );

    resStr = itoa(mUserVidConf.m_GameRect.w);
    resStr += "x";
    resStr += itoa(mUserVidConf.m_GameRect.h);
    mpGameResSelection->setSelection(resStr);


#endif

}


void CVideoSettings::release()
{
	// Save up the changed stuff

	gTimer.setFPS( mpFPSSelection->getSelection() );

#if defined(USE_OPENGL)
    mUserVidConf.m_opengl = mpOpenGLSwitch->isEnabled();
#endif

    mUserVidConf.mRenderScQuality = mpRenderScaleQualitySel->getSelection();

	
#if !defined(EMBEDDED)	
	//mUserVidConf.m_aspect_correction = mpAspectSwitch->isEnabled();	
	mUserVidConf.vsync = mpVSyncSwitch->isEnabled();
    std::string scalerStr = mpFilterSelection->getSelection();

    const std::string res = mpResolutionSelection->getSelection();
    sscanf( res.c_str(), "%hux%hux", &mUserVidConf.m_DisplayRect.w, &mUserVidConf.m_DisplayRect.h );

    const std::string GameResStr = mpGameResSelection->getSelection();
    sscanf( GameResStr.c_str(), "%hux%hux", &mUserVidConf.m_GameRect.w, &mUserVidConf.m_GameRect.h );

	int w, h;
	const std::string aspect = mpAspectSelection->getSelection();
	if( sscanf( aspect.c_str(), "%i:%i", &w, &h ) == 2 )
	{
	    mUserVidConf.mAspectCorrection.w = w;
	    mUserVidConf.mAspectCorrection.h = h;
	}
	else
	{
	    mUserVidConf.mAspectCorrection.w = 0;
	    mUserVidConf.mAspectCorrection.h = 0;
	}

	if( scalerStr != "none" )
	{
		mUserVidConf.m_normal_scale = (scalerStr.at(0) == 'n');
		if (mUserVidConf.m_normal_scale)
            mUserVidConf.m_ScaleXFilter = (filterOptionType)(scalerStr.at(6)-'0');
		else
            mUserVidConf.m_ScaleXFilter = (filterOptionType)(scalerStr.at(5)-'0');
	}
	else
	{
        mUserVidConf.m_ScaleXFilter = NONE;
	}
#endif

#if defined(CAANOO) || defined(WIZ) || defined(DINGOO) || defined(NANONOTE) || defined(ANDROID)
	mUserVidConf.m_DisplayRect.w = 320;
	mUserVidConf.m_DisplayRect.h = 200;
#endif


	mUserVidConf.m_special_fx = mpSFXSwitch->isEnabled();

	// In case the user changed something in the camera settings, reload that.
	mUserVidConf.m_CameraBounds = gVideoDriver.getCameraBounds();

	CVidConfig oldVidConf = gVideoDriver.getVidConfig();
	gVideoDriver.setVidConfig(mUserVidConf);		

	// At this point we also must apply and save the settings
	if( !gVideoDriver.applyMode() )
	{
        g_pSettings->loadDrvCfg(); // If it fails load the old settings
		return;
	}		

    if( !gVideoDriver.start() ) // Here the same situation
	{
		gVideoDriver.setVidConfig(oldVidConf);
		gVideoDriver.start();
	}
	
	g_pSettings->saveDrvCfg();

    gMenuController.updateGraphics();

    gVideoDriver.setRefreshSignal(true);
}

}
