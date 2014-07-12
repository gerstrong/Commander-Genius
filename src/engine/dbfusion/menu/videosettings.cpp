


#include <base/CInput.h>
#include <base/GsTimer.h>
#include <base/video/resolutionlist.h>
#include <base/video/CVideoDriver.h>
#include <base/utils/StringUtils.h>
#include <widgets/GsMenuController.h>

#include "engine/core/CBehaviorEngine.h"
#include "videosettings.h"
#include <base/utils/Utils.h>

#include "engine/core/CSettings.h"
#include "engine/core/videoAspect.h"



namespace dbfusion
{


class toggleFullscreenFunctor : public InvokeFunctorEvent
{
public:
    toggleFullscreenFunctor( VideoSettings& VSettings ) :
        mVSettings(VSettings) {}

private:

    void operator()() const
    {
        mVSettings.mUserVidConf.Fullscreen = !mVSettings.mUserVidConf.Fullscreen;
        mVSettings.release();
        mVSettings.refresh();
    }

    VideoSettings& mVSettings;
};



VideoSettings::VideoSettings() :
FusionMenu( GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f) )
{
    // Create the fps config selection control
    std::list<std::string>	List;
    for( int i = 10 ; i <= 120 ; i += 10 )
        List.push_back( itoa (i) );

    mpFPSSelection = new CGUINumberControl( "FPS", 10, 120, 10, 60, 0, false );
    mpMenuDialog->addControl( mpFPSSelection, GsRect<float>(0.2f, 0.2f, 0.6f, 0.05f) );

    /*mpGameResSelection = new CGUIComboSelection( "GameRes",
        filledStrList(1, "?x?") );
    mpMenuDialog->addControl( mpGameResSelection, GsRect<float>(0.2f, 0.25f, 0.6f, 0.05f) );*/

#if !defined(EMBEDDED)

    mpAspectSelection = new CGUIComboSelection( "Aspect",
        filledStrList(1, "disabled") );

    mpMenuDialog->addControl( mpAspectSelection, GsRect<float>(0.2f, 0.3f, 0.6f, 0.05f) );

    mpFilterSelection =
            new CGUIComboSelection( "Filter",
                filledStrList( 4, "none",
                                  "scale2x",
                                  "scale3x",
                                  "scale4x" ) );

    mpMenuDialog->addControl( mpFilterSelection, GsRect<float>(0.2f, 0.35f, 0.6f, 0.05f) );

    mpVSyncSwitch = new CGUISwitch( "VSync" );
    mpMenuDialog->addControl( mpVSyncSwitch, GsRect<float>(0.2f, 0.6f, 0.40f, 0.05f) );

    mpResolutionSelection = new CGUIComboSelection( "Size",
        filledStrList(1, "?x?") );
    mpMenuDialog->addControl( mpResolutionSelection, GsRect<float>(0.2f, 0.45f, 0.6f, 0.05f) );

    mpFullScreenSwitch = new GsButton( "Unknown mode",
                                        new toggleFullscreenFunctor(*this) );
    mpMenuDialog->addControl( mpFullScreenSwitch, GsRect<float>(0.2f, 0.5f, 0.6f, 0.05f) );
#endif

#if defined(USE_OPENGL)
    mpOpenGLSwitch = new CGUISwitch( "OpenGL" );
    mpMenuDialog->addControl( mpOpenGLSwitch, GsRect<float>(0.2f, 0.55f, 0.6f, 0.05f) );

    mpOGLFilterSelection = new CGUIComboSelection( "OGL Filter",
        filledStrList( 2, "nearest", "linear" ) );
    mpMenuDialog->addControl( mpOGLFilterSelection, GsRect<float>(0.2f, 0.6f, 0.6f, 0.05f) );
#endif


    mpSFXSwitch = new CGUISwitch( "Special FX" );
    mpMenuDialog->addControl( mpSFXSwitch, GsRect<float>(0.2f, 0.65f, 0.6f, 0.05f) );



    refresh();
}

void VideoSettings::refresh()
{
    mUserVidConf = gVideoDriver.getVidConfig();

    // Load the config into the GUI
    // TODO: Temporary. This must become a float later...
    mpFPSSelection->setSelection( static_cast<int>( gTimer.FPS() ) );

#if defined(USE_OPENGL)
    //std::string OGLFilterStr;
    mpOpenGLSwitch->enable( mUserVidConf.m_opengl );
    mpOGLFilterSelection->setSelection( mUserVidConf.m_opengl_filter==GL_LINEAR ? "linear" : "nearest" );
    mpOGLFilterSelection->enable( mUserVidConf.m_opengl );
#endif

    mpSFXSwitch->enable( mUserVidConf.m_special_fx );

#if !defined(EMBEDDED)
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


    /*mpGameResSelection->setList( GamesResList, NUM_GAME_RESOLUTIONS );

    resStr = itoa(mUserVidConf.m_GameRect.w);
    resStr += "x";
    resStr += itoa(mUserVidConf.m_GameRect.h);
    mpGameResSelection->setSelection(resStr);*/


#endif

}


void VideoSettings::release()
{
    // Save up the changed stuff

    gTimer.setFPS( mpFPSSelection->getSelection() );

#if defined(USE_OPENGL)
    mUserVidConf.m_opengl_filter = mpOGLFilterSelection->getSelection() == "linear" ? GL_LINEAR : GL_NEAREST;
    mUserVidConf.m_opengl = mpOpenGLSwitch->isEnabled();
#endif


#if !defined(EMBEDDED)
    //mUserVidConf.m_aspect_correction = mpAspectSwitch->isEnabled();
    mUserVidConf.vsync = mpVSyncSwitch->isEnabled();
    std::string scalerStr = mpFilterSelection->getSelection();

    const std::string res = mpResolutionSelection->getSelection();
    sscanf( res.c_str(), "%hux%hux", &mUserVidConf.m_DisplayRect.w, &mUserVidConf.m_DisplayRect.h );

    /*const std::string GameResStr = mpGameResSelection->getSelection();
    sscanf( GameResStr.c_str(), "%hux%hux", &mUserVidConf.m_GameRect.w, &mUserVidConf.m_GameRect.h );*/

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
}

}
