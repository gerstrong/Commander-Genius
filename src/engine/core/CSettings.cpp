/*
 * CSettings.cpp
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong
 */

#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include <base/utils/ConfigHandler.h>
#include <base/CInput.h>
#include "CSettings.h"
#include <base/GsTimer.h>
#include "sdl/audio/Audio.h"
#include <base/video/CVideoDriver.h>
#include "options.h"
#include "CBehaviorEngine.h"
#include "fileio/CConfiguration.h"
#include "fileio/KeenFiles.h"
#include "engine/core/VGamepads/vgamepadsimple.h"



/**
 * \brief	The CSettings class handles the saving and loading of all the settings that are saved in
 * 			the game. Those are, video, audio, options and input.
 *
 * \param	p_option	pointer to an array that stores the options settings of the game
 */
CSettings::CSettings()
{
	notes << "Reading game options from " << GetFullFileName(CONFIGFILENAME) << endl;
	notes << "Will write game options to " << GetWriteFullFileName(CONFIGFILENAME, true) << endl;
}


/**
 * \brief	Only saves the last used resolution or window size.
 * \return	If the configuration has been saved successfully, it return true, else it's false.
 */
bool CSettings::saveDispCfg()
{
    CConfiguration Configuration(CONFIGFILENAME);
    Configuration.Parse();

    CVidConfig &VidConf = gVideoDriver.getVidConfig();
    Configuration.WriteInt("Video", "width", VidConf.mDisplayRect.dim.x);
    Configuration.WriteInt("Video", "height", VidConf.mDisplayRect.dim.y);

    return Configuration.saveCfgFile();
}

/**
 * \brief	Write the whole configuration of the settings.
 * 			Note: See also CConfiguration to understand better the concept of saving...
 *
 * \return	If the configuration has been saved successfully, it return true, else it's false.
 */
bool CSettings::saveDrvCfg()
{
    CConfiguration Configuration(CONFIGFILENAME);

    try
    {
        Configuration.Parse();

        int i = 1;

        for(searchpathlist::const_iterator p = tSearchPaths.begin(); p != tSearchPaths.end(); p++, i++)
            Configuration.WriteString("FileHandling", "SearchPath" + itoa(i), *p);

        CVidConfig &VidConf = gVideoDriver.getVidConfig();
        Configuration.SetKeyword("Video", "fullscreen", VidConf.mFullscreen);
        Configuration.SetKeyword("Video", "OpenGL", VidConf.mOpengl);
#ifdef VIRTUALPAD
        Configuration.SetKeyword("Video", "VirtPad", VidConf.mVPad);
        Configuration.WriteInt("Video", "VirtPadSize", VidConf.mVPadSize);
#endif
        Configuration.SetKeyword("Video", "ShowCursor", VidConf.mShowCursor);
        Configuration.SetKeyword("Video", "TiltedScreen", VidConf.mTiltedScreen);


        Configuration.WriteInt("Video", "width", VidConf.mDisplayRect.dim.x);
        Configuration.WriteInt("Video", "height", VidConf.mDisplayRect.dim.y);

        Configuration.WriteInt("Video", "gameWidth", VidConf.mGameRect.dim.x);
        Configuration.WriteInt("Video", "gameHeight", VidConf.mGameRect.dim.y);

        Configuration.WriteInt("Video", "scale", VidConf.Zoom);


        const std::string oglFilter =
                (VidConf.mRenderScQuality == CVidConfig::RenderQuality::LINEAR) ?
                "linear" : "nearest";

        Configuration.WriteString("Video", "OGLfilter", oglFilter );


        Configuration.WriteInt("Video", "filter", VidConf.m_ScaleXFilter);
        Configuration.WriteString("Video", "scaletype", VidConf.m_normal_scale ? "normal" : "scalex" );        

        const auto fpsi = int(gTimer.FPS());
        Configuration.WriteInt("Video", "fps", fpsi);

        Configuration.SetKeyword("Video", "vsync", VidConf.mVSync);

        const std::string arc_str =
                itoa(VidConf.mAspectCorrection.dim.x) + ":" +
                itoa(VidConf.mAspectCorrection.dim.y);
        Configuration.WriteString("Video", "aspect", arc_str);

        st_camera_bounds &CameraBounds = VidConf.m_CameraBounds;
        Configuration.WriteInt("Bound", "left", CameraBounds.left);
        Configuration.WriteInt("Bound", "right", CameraBounds.right);
        Configuration.WriteInt("Bound", "up", CameraBounds.up);
        Configuration.WriteInt("Bound", "down", CameraBounds.down);
        Configuration.WriteInt("Bound", "speed", CameraBounds.speed);

        Configuration.WriteInt("Audio", "channels", (gSound.getAudioSpec()).channels);
        Configuration.WriteInt("Audio", "format", (gSound.getAudioSpec()).format);
        Configuration.WriteInt("Audio", "rate", (gSound.getAudioSpec()).freq);
        Configuration.SetKeyword("Audio", "sndblaster", gSound.getSoundBlasterMode());
        Configuration.WriteInt("Audio", "musicvol", (gSound.getMusicVolume()));
        Configuration.WriteInt("Audio", "soundvol", (gSound.getSoundVolume()));

    }
    catch(...)
    {
        gLogging.textOut(FONTCOLORS::RED,"General error writing the configuration file...\n");
    }

    bool ok = Configuration.saveCfgFile();

    gLogging.textOut(FONTCOLORS::GREEN,"Saving game options...");

    if(ok)
    {
        gLogging.textOut(FONTCOLORS::GREEN,"ok.");
    }
    else
    {
        gLogging.textOut(FONTCOLORS::RED,"error.");
    }

    return ok;
}

/**
 * \brief	It loads the whole configuration from the settings file.
 * 			NOTE: Also take a look at CConfiguration.
 *
 * \return		true if successful, false if not.
 */
bool CSettings::loadDrvCfg()
{
    CConfiguration config(CONFIGFILENAME);

    CVidConfig VidConf;

    if(!config.Parse())
    {
        return false;
    }
	else
	{		
        GsRect<Uint16> &res = VidConf.mDisplayRect;
        GsRect<Uint16> &gamesRes = VidConf.mGameRect;
        int value = 0;
        config.ReadInteger("Video", "width", &value, 320);
        res.dim.x = static_cast<unsigned short>(value);
        config.ReadInteger("Video", "height", &value, 200);
        res.dim.y = static_cast<unsigned short>(value);

        config.ReadInteger("Video", "gameWidth", &value, 320);
        gamesRes.dim.x = static_cast<unsigned short>(value);
        config.ReadInteger("Video", "gameHeight", &value, 200);
        gamesRes.dim.y = static_cast<unsigned short>(value);

        if(res.dim.x*res.dim.y <= 0)
		{
			gLogging.ftextOut(FONTCOLORS::RED,"Error reading the configuration file!<br>");
			return false;
		}

        config.ReadKeyword("Video", "fullscreen", &VidConf.mFullscreen, false);
        config.ReadInteger("Video", "scale", &value, 1);
        VidConf.Zoom = static_cast<unsigned short>(value);


		std::string arcStr;
        config.ReadString("Video", "aspect", arcStr, "none");
        VidConf.mAspectCorrection.dim = 0;
        sscanf( arcStr.c_str(), "%i:%i", &VidConf.mAspectCorrection.dim.x,
                                         &VidConf.mAspectCorrection.dim.y );

        config.ReadKeyword("Video", "vsync", &VidConf.mVSync, true);
        config.ReadInteger("Video", "filter", &value, 1);
        VidConf.m_ScaleXFilter = static_cast<filterOptionType>(value);

		std::string scaleType;
        config.ReadString("Video", "scaletype", scaleType, "normal");
		VidConf.m_normal_scale = (scaleType == "normal");

		// if ScaleX is one and scaletype is not at normal, this is wrong.
		// we will change that and force it to normal
		if(scaleType == "normal")
		{
		  VidConf.m_normal_scale = true;
		}


        config.ReadKeyword("Video", "OpenGL", &VidConf.mOpengl, true);

        std::string oglFilter;
        config.ReadString("Video", "OGLfilter", oglFilter, "nearest");

        VidConf.mRenderScQuality =
                (oglFilter == "linear") ?
                    CVidConfig::RenderQuality::LINEAR :
                    CVidConfig::RenderQuality::NEAREST;

#ifdef VIRTUALPAD
        config.ReadKeyword("Video", "VirtPad", &VidConf.mVPad, VidConf.mVPad);
        config.ReadInteger("Video", "VirtPadSize", &VidConf.mVPadSize, VidConf.mVPadSize);
#endif
        config.ReadKeyword("Video", "ShowCursor", &VidConf.mShowCursor, true);
        config.ReadKeyword("Video", "TiltedScreen", &VidConf.mTiltedScreen, false);


		st_camera_bounds &CameraBounds = VidConf.m_CameraBounds;
        config.ReadInteger("Bound", "left", &CameraBounds.left, 152);
        config.ReadInteger("Bound", "right", &CameraBounds.right, 168);
        config.ReadInteger("Bound", "up", &CameraBounds.up, 92);
        config.ReadInteger("Bound", "down", &CameraBounds.down, 108);
        config.ReadInteger("Bound", "speed", &CameraBounds.speed, 20);
		gVideoDriver.setVidConfig(VidConf);

		int framerate;
        config.ReadInteger("Video", "fps", &framerate, 60);
        gTimer.setFPS( float(framerate) );


		int audio_rate, audio_channels, audio_format;
		bool audio_sndblaster;
        config.ReadInteger("Audio", "rate", &audio_rate, 44000);
        config.ReadInteger("Audio", "channels", &audio_channels, 2);
        config.ReadInteger("Audio", "format", &audio_format, AUDIO_U8);
        config.ReadKeyword("Audio", "sndblaster", &audio_sndblaster, false);
        gSound.setSettings(audio_rate, audio_channels, audio_format, audio_sndblaster);


		int sound_vol, music_vol;
        config.ReadInteger("Audio", "musicvol", &music_vol, SDL_MIX_MAXVOLUME);
        config.ReadInteger("Audio", "soundvol", &sound_vol, SDL_MIX_MAXVOLUME);

        gSound.setMusicVolume(Uint8(music_vol), false);
        gSound.setSoundVolume(Uint8(sound_vol), false);


	}

	return true;
}

void CSettings::loadDefaultGraphicsCfg() //Loads default graphics
{
    gVideoDriver.setMode(320,200);
	gVideoDriver.isFullscreen(false);

#if defined(USE_OPENGL)
	gVideoDriver.enableOpenGL(false);           
    gVideoDriver.setRenderQuality(CVidConfig::RenderQuality::LINEAR);
#endif

	gVideoDriver.setZoom(1);
    gTimer.setFPS(60.0f);
#if defined(ANDROID)	
	gVideoDriver.setAspectCorrection(0,0);
#else
	gVideoDriver.setAspectCorrection(4,3);
#endif
    gVideoDriver.setFilter(NONE);
	gVideoDriver.setScaleType(true);

}

/**
 * \brief	Sets the option data to the option array.
 *
 * \param	opt			option ID
 * \param	menuname	Name of the option shown in the options menu
 * 						NOTE: This name may change, depending on what value is set
 * \param	name		Name of the option itself
 * 						NOTE: This string should be changed, since it names the topic
 * 						of the applied option
 * \param	value		Value that has to be set.
 */
void CSettings::setOption( const GameOption opt,
                           const std::string &menuname,
                           const std::string &name,
                           const char value)
{
    stOption &option = gBehaviorEngine.mOptions[opt];
	option.menuname = menuname;
	option.name = name;
	option.value = value;
}
/**
 * \brief  This is normally processed when the game is started. It sets the default options.
 */
void CSettings::loadDefaultGameCfg()
{
    setOption( GameOption::ALLOWPKING,		"Friendly Fire  ", "pking", 1 );
    setOption( GameOption::KEYSTACK,		"Keystacking    ", "keystack", 0 );
    setOption( GameOption::LVLREPLAYABILITY,"Replay Levels  ", "level_replayability", 0 );
    setOption( GameOption::RISEBONUS,		"Rising Bonus   ", "rise_bonus", 1 );
    setOption( GameOption::MODERN,          "Modern Style   ", "modern_style", 1 );
    setOption( GameOption::HUD,				"HUD Display    ", "hud", 1 );
    setOption( GameOption::SPECIALFX,		"Special Effects", "specialfx", 1 );
    setOption( GameOption::SHOWFPS,			"Show FPS       ", "showfps", 0 );
    setOption( GameOption::SANDWICHMENU,    "Menu Button    ", "sandwichbutton", 1 );
}

/**
 * \brief  This loads the options of the settings
 *
 * \return			true if options could be loaded, else false
 */
bool CSettings::loadGameOptions()
{
	CConfiguration Configuration(CONFIGFILENAME);

	if(!Configuration.Parse()) return false;

	loadDefaultGameCfg();

    for(auto &option : gBehaviorEngine.mOptions)
	{
        auto &second = option.second;
		bool newvalue;
        if ( Configuration.ReadKeyword("Game", option.second.name, &newvalue, false) )
        {
            second.value = (newvalue) ? 1 : 0;
        }
	}
	
	gLogging.ftextOut("<br>Your personal settings were loaded successfully...<br>");
	return true;
}

/**
 * \brief  Saves the options in the settings
 */
bool CSettings::saveGameOptions()
{
	CConfiguration Configuration(CONFIGFILENAME);

    if ( !Configuration.Parse() )
    {
        return false;
    }


    for(auto &option : gBehaviorEngine.mOptions)
    {
        Configuration.SetKeyword("Game", option.second.name, option.second.value);
    }

	Configuration.saveCfgFile();
	return true;
}

const std::string CSettings::getConfigFileName() const
{	return CONFIGFILENAME;	}

