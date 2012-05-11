/*
 * CSettings.cpp
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong
 */

#include "CLogFile.h"
#include "FindFile.h"
#include "ConfigHandler.h"
#include "CSettings.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"
#include "sdl/CVideoDriver.h"
#include "common/options.h"
#include "common/CBehaviorEngine.h"
#include "fileio/CConfiguration.h"


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
 * \brief	Write the whole configuration of the settings.
 * 			Note: See also CConfiguration to understand better the concept of saving...
 *
 * \return	If the configuration has been saved successfully, it return true, else it's false.
 */
bool CSettings::saveDrvCfg()
{
	CConfiguration Configuration(CONFIGFILENAME);
	Configuration.Parse();
	
	int i = 1;
	for(searchpathlist::const_iterator p = tSearchPaths.begin(); p != tSearchPaths.end(); p++, i++)
		Configuration.WriteString("FileHandling", "SearchPath" + itoa(i), *p);
	
	CVidConfig &VidConf = g_pVideoDriver->getVidConfig();
	Configuration.SetKeyword("Video", "fullscreen", VidConf.Fullscreen);
	Configuration.SetKeyword("Video", "OpenGL", VidConf.m_opengl);

	Configuration.WriteInt("Video", "width", VidConf.m_DisplayRect.w);
	Configuration.WriteInt("Video", "height", VidConf.m_DisplayRect.h);
	Configuration.WriteInt("Video", "scale", VidConf.Zoom);
#ifdef USE_OPENGL
	if(VidConf.m_opengl)
		Configuration.WriteString("Video", "OGLfilter", VidConf.m_opengl_filter == GL_NEAREST ? "nearest" : "linear" );
#endif
	Configuration.WriteInt("Video", "filter", VidConf.m_ScaleXFilter);
	Configuration.SetKeyword("Video", "specialfx", VidConf.m_special_fx);
	Configuration.WriteInt("Video", "autoframeskip", g_pTimer->getFrameRate());
	Configuration.SetKeyword("Video", "showfps", VidConf.showfps);
	
	st_camera_bounds &CameraBounds = VidConf.m_CameraBounds;
	Configuration.WriteInt("Bound", "left", CameraBounds.left);
	Configuration.WriteInt("Bound", "right", CameraBounds.right);
	Configuration.WriteInt("Bound", "up", CameraBounds.up);
	Configuration.WriteInt("Bound", "down", CameraBounds.down);
	Configuration.WriteInt("Bound", "speed", CameraBounds.speed);
	
	Configuration.WriteInt("Audio", "channels", (g_pSound->getAudioSpec()).channels);
	Configuration.WriteInt("Audio", "format", (g_pSound->getAudioSpec()).format);
	Configuration.WriteInt("Audio", "rate", (g_pSound->getAudioSpec()).freq);
	Configuration.SetKeyword("Audio", "sndblaster", g_pSound->getSoundBlasterMode());
	Configuration.WriteInt("Audio", "mixerch", (g_pSound->getMixingchannels()));
	Configuration.WriteInt("Audio", "musicvol", (g_pSound->getMusicVolume()/8));
	Configuration.WriteInt("Audio", "soundvol", (g_pSound->getSoundVolume()/8));
	
	return Configuration.saveCfgFile();
}

/**
 * \brief	It loads the whole configuration from the settings file.
 * 			NOTE: Also take a look at CConfiguration.
 *
 * \return		true if successful, false if not.
 */
bool CSettings::loadDrvCfg()
{
	CConfiguration Configuration(CONFIGFILENAME);
	
	if(!Configuration.Parse()) return false;
	else
	{
		CVidConfig VidConf;
		CRect<Uint16> &res = VidConf.m_DisplayRect;
		//Configuration.ReadInteger("Video", "bpp", &res.depth, 32);
		int value = 0;
		Configuration.ReadInteger("Video", "width", &value, 320);
		res.w = value;
		Configuration.ReadInteger("Video", "height", &value, 200);
		res.h = value;
		
		if(res.w*res.h <= 0)
		{
			g_pLogFile->ftextOut(RED,"Error reading the configuration file!<br>");
			return false;
		}

		Configuration.ReadKeyword("Video", "fullscreen", &VidConf.Fullscreen, false);
		Configuration.ReadInteger("Video", "scale", &value, 1);
		VidConf.Zoom = value;
		Configuration.ReadKeyword("Video", "specialfx", &VidConf.m_special_fx, true);
		Configuration.ReadKeyword("Video", "showfps", &VidConf.showfps, false);
		Configuration.ReadInteger("Video", "filter", &value, 1);
		VidConf.m_ScaleXFilter = value;
		Configuration.ReadKeyword("Video", "OpenGL", &VidConf.m_opengl, false);
#ifdef USE_OPENGL
		if(VidConf.m_opengl)
		{
			std::string oglFilter;
			Configuration.ReadString("Video", "OGLfilter",  oglFilter, "nearest");
			VidConf.m_opengl_filter = (oglFilter == "nearest") ? GL_NEAREST : GL_LINEAR;
		}
#endif
		
		
		st_camera_bounds &CameraBounds = VidConf.m_CameraBounds;
		Configuration.ReadInteger("Bound", "left", &CameraBounds.left, 152);
		Configuration.ReadInteger("Bound", "right", &CameraBounds.right, 168);
		Configuration.ReadInteger("Bound", "up", &CameraBounds.up, 92);
		Configuration.ReadInteger("Bound", "down", &CameraBounds.down, 108);
		Configuration.ReadInteger("Bound", "speed", &CameraBounds.speed, 20);
		g_pVideoDriver->setVidConfig(VidConf);

		int framerate;
		Configuration.ReadInteger("Video", "autoframeskip", &framerate, 60);
		g_pTimer->setFPS( framerate );


		int audio_rate, audio_channels, audio_format;
		bool audio_sndblaster;
		Configuration.ReadInteger("Audio", "rate", &audio_rate, 44000);
		Configuration.ReadInteger("Audio", "channels", &audio_channels, 2);
		Configuration.ReadInteger("Audio", "format", &audio_format, AUDIO_U8);
		Configuration.ReadKeyword("Audio", "sndblaster", &audio_sndblaster, false);
		g_pSound->setSettings(audio_rate, audio_channels, audio_format, audio_sndblaster);


		int sound_vol, music_vol;
		Configuration.ReadInteger("Audio", "musicvol", &music_vol, SDL_MIX_MAXVOLUME);
		Configuration.ReadInteger("Audio", "soundvol", &sound_vol, SDL_MIX_MAXVOLUME);

		g_pSound->setMusicVolume(music_vol*8);
		g_pSound->setSoundVolume(sound_vol*8);
	}
	return true;
}

void CSettings::loadDefaultGraphicsCfg() //Loads default graphics
{
	g_pVideoDriver->setMode(320,200,32);
	g_pVideoDriver->isFullscreen(false);
	g_pVideoDriver->setOGLFilter(GL_NEAREST);
	g_pVideoDriver->setZoom(1);
	g_pTimer->setFPS(60);
	g_pVideoDriver->setFilter(1);
	g_pVideoDriver->enableOpenGL(false);
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
void CSettings::setOption( e_OptionKeyword opt, const std::string &menuname, const std::string &name, char value)
{
	stOption &option = g_pBehaviorEngine->m_option[opt];
	option.menuname = menuname;
	option.name = name;
	option.value = value;
}
/**
 * \brief  This is normally processed when the game is started. It sets the default options.
 */
void CSettings::loadDefaultGameCfg()
{
	setOption( OPT_FULLYAUTOMATIC,	"Machine Gun      ", "autogun", 0 );
	setOption( OPT_SUPERPOGO,		"Super Pogo Stick ", "superpogo", 0 );
	setOption( OPT_ALLOWPKING,		"Friendly Fire    ", "pking", 1 );
	setOption( OPT_KEYSTACK,			"Keystacking      ", "keystack", 0 );
	setOption( OPT_ANALOGJOYSTICK,	"Analog Joystick  ", "analog_joystick", 1 );
	setOption( OPT_LVLREPLAYABILITY,	"Replay Levels    ", "level_replayability", 0 );
	setOption( OPT_RISEBONUS,		"Rising Bonus     ", "rise_bonus", 1 );
	setOption( OPT_SWITCHSCORES,		"Score Fix (EP3)  ", "switch_scores", 0 );
	setOption( OPT_IMPPOGO,			"Impossible Pogo  ", "impossible_pogo", 0 );
	setOption( OPT_HUD,				"HUD Display      ", "hud", 1 );
	setOption( OPT_FLASHEFFECT,		"Flash Effects    ", "flashfx", 1 );
	setOption( OPT_FIXLEVELERRORS,	"Fix Level Errors ", "fix_level", 1 );
}

/**
 * \brief  This loads the options of the settings
 *
 * \return			true if options could be loaded, else false
 */
bool CSettings::loadGameCfg()
{
	int i;
	CConfiguration Configuration(CONFIGFILENAME);

	if(!Configuration.Parse()) return false;

	loadDefaultGameCfg();

	stOption *p_option = g_pBehaviorEngine->m_option;
	for (i = 0; i < NUM_OPTIONS; i++)
	{
		bool newvalue;
		Configuration.ReadKeyword("Game", p_option[i].name, &newvalue, false);
		p_option[i].value = (newvalue) ? 1 : 0;
	}
	
	g_pLogFile->ftextOut("<br>Your personal settings were loaded successfully...<br>");
	return true;
}

/**
 * \brief  Saves the options in the settings
 */
bool CSettings::saveGameCfg()
{
	CConfiguration Configuration(CONFIGFILENAME);

	if(!Configuration.Parse()) return false;
	
	stOption *p_option = g_pBehaviorEngine->m_option;
	for (int i = 0; i < NUM_OPTIONS; i++)
		Configuration.SetKeyword("Game", p_option[i].name, p_option[i].value);
	
	Configuration.saveCfgFile();
	return true;
}

const std::string CSettings::getConfigFileName() const
{	return CONFIGFILENAME;	}

CSettings::~CSettings() {}
