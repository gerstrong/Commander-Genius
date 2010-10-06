/*
 * CSettings.cpp
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong
 */

#include "../common/options.h"
#include "../fileio/CConfiguration.h"
#include "../CLogFile.h"
#include "../FindFile.h"
#include "../ConfigHandler.h"
#include "CSettings.h"
#include "CVideoDriver.h"
#include "CTimer.h"
#include "sound/CSound.h"

/**
 * \brief	The CSettings class handles the saving and loading of all the settings that are saved in
 * 			the game. Those are, video, audio, options and input.
 *
 * \param	p_option	pointer to an array that stores the options settings of the game
 */
CSettings::CSettings(stOption *p_option) {
	notes << "Reading game options from " << GetFullFileName(CONFIGFILENAME) << endl;
	notes << "Will write game options to " << GetWriteFullFileName(CONFIGFILENAME, true) << endl;
	mp_option = p_option;
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
	
	Configuration.WriteInt("Video", "bpp",g_pVideoDriver->getDepth());
	Configuration.SetKeyword("Video", "fullscreen", g_pVideoDriver->getFullscreen());
	Configuration.SetKeyword("Video", "OpenGL", g_pVideoDriver->isOpenGL());

	Configuration.WriteInt("Video", "width", g_pVideoDriver->getWidth());
	Configuration.WriteInt("Video", "height", g_pVideoDriver->getHeight());
	Configuration.WriteInt("Video", "scale", g_pVideoDriver->getZoomValue());
	Configuration.WriteInt("Video", "OGLfilter", g_pVideoDriver->getOGLFilter());
	Configuration.WriteInt("Video", "filter", g_pVideoDriver->getFiltermode());
	Configuration.SetKeyword("Video", "specialfx", g_pVideoDriver->getSpecialFXConfig());
	Configuration.WriteInt("Video", "autoframeskip", g_pTimer->getFrameRate());
	Configuration.WriteInt("Video", "showfps", g_pVideoDriver->showfps);
	
	st_camera_bounds &CameraBounds = g_pVideoDriver->getCameraBounds();
	Configuration.WriteInt("Bound", "left", CameraBounds.left);
	Configuration.WriteInt("Bound", "right", CameraBounds.right);
	Configuration.WriteInt("Bound", "up", CameraBounds.up);
	Configuration.WriteInt("Bound", "down", CameraBounds.down);
	Configuration.WriteInt("Bound", "speed", CameraBounds.speed);
	
	Configuration.WriteInt("Audio", "channels", (g_pSound->getAudioSpec()).channels);
	Configuration.WriteInt("Audio", "format", (g_pSound->getAudioSpec()).format);
	Configuration.WriteInt("Audio", "rate", (g_pSound->getAudioSpec()).freq);
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
		int width, height, depth;
		Configuration.ReadInteger("Video", "bpp", &depth, 32);
		Configuration.ReadInteger("Video", "width", &width, 320);
		Configuration.ReadInteger("Video", "height", &height, 200);
		
		if(depth*width*height <= 0)
		{
			g_pLogFile->ftextOut(RED,"Error reading the configuration file!<br>");
			return false;
		}
		else
			g_pVideoDriver->setMode(width,height,depth);

		bool fullscreen;
		Configuration.ReadKeyword("Video", "fullscreen", &fullscreen, false);
		g_pVideoDriver->isFullscreen(fullscreen);
		
		bool glfilter;
		Configuration.ReadKeyword("Video", "OGLfilter", &glfilter, false);
		g_pVideoDriver->setOGLFilter(glfilter);

		int scale;
		Configuration.ReadInteger("Video", "scale", &scale, 1);
		g_pVideoDriver->setZoom(scale);

		bool special_fx;
		Configuration.ReadKeyword("Video", "specialfx", &special_fx, true);
		g_pVideoDriver->setSpecialFXMode(special_fx);

		Configuration.ReadKeyword("Video", "showfps", &g_pVideoDriver->showfps, false);

		int framerate;
		Configuration.ReadInteger("Video", "autoframeskip", &framerate, 60);
		g_pTimer->setFrameRate(DEFAULT_LPS, framerate, DEFAULT_SYNC);

		st_camera_bounds CameraBounds;
		#ifdef ANDROID
		// For some reason it's buggy
		Configuration.ReadInteger("Bound", "left", &CameraBounds.left, 0);
		Configuration.ReadInteger("Bound", "right", &CameraBounds.right, 0);
		Configuration.ReadInteger("Bound", "up", &CameraBounds.up, 0);
		Configuration.ReadInteger("Bound", "down", &CameraBounds.down, 0);
		Configuration.ReadInteger("Bound", "speed", &CameraBounds.speed, 20);
		#else
		Configuration.ReadInteger("Bound", "left", &CameraBounds.left, 152);
		Configuration.ReadInteger("Bound", "right", &CameraBounds.right, 168);
		Configuration.ReadInteger("Bound", "up", &CameraBounds.up, 92);
		Configuration.ReadInteger("Bound", "down", &CameraBounds.down, 108);
		Configuration.ReadInteger("Bound", "speed", &CameraBounds.speed, 20);
		#endif

		g_pVideoDriver->saveCameraBounds(CameraBounds);
		
		int filter;
		Configuration.ReadInteger("Video", "filter", &filter, 1);
		g_pVideoDriver->setFilter(filter);
		
		bool opengl;
		Configuration.ReadKeyword("Video", "OpenGL", &opengl, false);
		g_pVideoDriver->enableOpenGL(opengl);

		int audio_rate, audio_channels, audio_format;
		Configuration.ReadInteger("Audio", "rate", &audio_rate, 44000);
		Configuration.ReadInteger("Audio", "channels", &audio_channels, 2);
		Configuration.ReadInteger("Audio", "format", &audio_format, AUDIO_U8);
		g_pSound->setSoundmode(audio_rate, audio_channels, audio_format);

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
	g_pVideoDriver->setOGLFilter(false);
	g_pVideoDriver->setZoom(1);
	g_pTimer->setFrameRate(DEFAULT_LPS, 60, DEFAULT_SYNC);
	g_pVideoDriver->setFilter(1);
	g_pVideoDriver->enableOpenGL(false);
}

/**
 * \brief	checks if the pointer to the options array is set.
 *
 * \return			true, if pointer is properly set, false if not.
 */
bool CSettings::checkOptionPtr() {
	if(mp_option) return true;
	g_pLogFile->textOut("ERROR in Code implementation! There is an error in the source code. Pointer mp_option cannot be used here!\n");
	return false;
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
	if(!checkOptionPtr()) return;
	mp_option[opt].menuname = menuname;
	mp_option[opt].name = name;
	mp_option[opt].value = value;
}
/**
 * \brief  This is normally processed when the game is started. It sets the default options.
 */
void CSettings::loadDefaultGameCfg()
{
	if(!checkOptionPtr()) return;
	setOption( OPT_FULLYAUTOMATIC,	"Machine Gun      ", "autogun", 0 );
	setOption( OPT_SUPERPOGO,		"Super Pogo Stick ", "superpogo", 0 );
	setOption( OPT_ALLOWPKING,		"Friendly Fire    ", "pking", 1 );
	setOption( OPT_KEYSTACK,		"Keystacking      ", "keystack", 0 );
	setOption( OPT_ANALOGJOYSTICK,	"Analog Joystick  ", "analog_joystick", 1 );
	setOption( OPT_LVLREPLAYABILITY,"Replay Levels    ", "level_replayability", 0 );
	setOption( OPT_RISEBONUS,		"Rising Bonus     ", "rise_bonus", 1 );
	setOption( OPT_SWITCHSCORES,	"Score Fix (EP3)  ", "switch_scores", 0 );
	setOption( OPT_IMPPOGO,			"Impossible Pogo  ", "impossible_pogo", 0 );
	setOption( OPT_HUD,				"HUD Display      ", "hud", 1 );
	setOption( OPT_FLASHEFFECT,		"Flash Effects    ", "flashfx", 1 );
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

	if(!checkOptionPtr()) return false;

	loadDefaultGameCfg();

	for (i = 0; i < NUM_OPTIONS; i++)
	{
		bool newvalue;
		Configuration.ReadKeyword("Game", mp_option[i].name, &newvalue, false);
		mp_option[i].value = (newvalue) ? 1 : 0;
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
	
	if(!checkOptionPtr()) return false;

	for (int i = 0; i < NUM_OPTIONS; i++)
		Configuration.SetKeyword("Game", mp_option[i].name, mp_option[i].value);
	
	Configuration.saveCfgFile();
	return true;
}

CSettings::~CSettings() {}
