/*
 * CSettings.cpp
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong
 */

#include "../common/options.h"
#include "../engine/vorticon/CCamera.h"
#include "../fileio/CParser.h"
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
 * 			Note: See also CParser to understand better the concept of saving...
 *
 * \return	If the configuration has been saved successfully, it return true, else it's false.
 */
bool CSettings::saveDrvCfg()
{
	CParser Parser;
	Parser.loadParseFile();
	
	{
		int i = 1;
		for(searchpathlist::const_iterator p = tSearchPaths.begin(); p != tSearchPaths.end(); p++, i++)
			Parser.saveValue("SearchPath" + itoa(i), "FileHandling", *p);
	}
	
	Parser.saveIntValue("bpp","Video",g_pVideoDriver->getDepth());
	
	if(g_pVideoDriver->getFullscreen())
		Parser.saveIntValue("fullscreen","Video",1);
	else
		Parser.saveIntValue("fullscreen","Video",0);
	
	if(g_pVideoDriver->isOpenGL())
		Parser.saveIntValue("OpenGL","Video",1);
	else
		Parser.saveIntValue("OpenGL","Video",0);
	
	Parser.saveIntValue("width","Video",g_pVideoDriver->getWidth());
	Parser.saveIntValue("height","Video",g_pVideoDriver->getHeight());
	Parser.saveIntValue("scale","Video",g_pVideoDriver->getZoomValue());
	Parser.saveIntValue("OGLfilter","Video",g_pVideoDriver->getOGLFilter());
	Parser.saveIntValue("filter","Video",g_pVideoDriver->getFiltermode());
	Parser.saveIntValue("specialfx","Video",g_pVideoDriver->getSpecialFXConfig());
	Parser.saveIntValue("autoframeskip","Video",g_pTimer->getFrameRate());
	
	Parser.saveIntValue("left","Bound",g_pCamera->getScrollLeft());
	Parser.saveIntValue("right","Bound",g_pCamera->getScrollRight());
	Parser.saveIntValue("up","Bound",g_pCamera->getScrollUp());
	Parser.saveIntValue("down","Bound",g_pCamera->getScrollDown());
	Parser.saveIntValue("speed","Bound",g_pCamera->getScrollSpeed());
	
	Parser.saveIntValue("channels","Audio",(g_pSound->getAudioSpec()).channels);
	Parser.saveIntValue("format","Audio",(g_pSound->getAudioSpec()).format);
	Parser.saveIntValue("rate","Audio",(g_pSound->getAudioSpec()).freq);
	Parser.saveIntValue("mixerch","Audio",(g_pSound->getMixingchannels()));
	Parser.saveIntValue("musicvol","Audio",(g_pSound->getMusicVolume()/8));
	Parser.saveIntValue("soundvol","Audio",(g_pSound->getSoundVolume()/8));
	
	return Parser.saveParseFile();
}

/**
 * \brief	It loads the whole configuration from the settings file. NOTE: Also take a look at CParser.
 *
 * \return		true if successful, false if not.
 */
bool CSettings::loadDrvCfg()
{
	CParser Parser;
	
	if(!Parser.loadParseFile()) return false;
	else
	{
		int width, height, depth, boundl, boundr, boundu, boundd, sspeed;
		
		depth  = Parser.getIntValue("bpp","Video");
		width  = Parser.getIntValue("width","Video");
		height = Parser.getIntValue("height","Video");
		
		boundl  = Parser.getIntValue("left","Bound");
		boundr  = Parser.getIntValue("right","Bound");
		boundu	= Parser.getIntValue("up","Bound");
		boundd  = Parser.getIntValue("down","Bound");
		sspeed  = Parser.getIntValue("speed","Bound");

		if(depth*width*height <= 0)
		{
			g_pLogFile->ftextOut(RED,"Error reading the configuration file!<br>");
			return false;
		}
		
		g_pVideoDriver->setMode(width,height,depth);
		g_pVideoDriver->setMode(width,height,depth);
		g_pVideoDriver->isFullscreen(((Parser.getIntValue("fullscreen","Video")) == 1));
		g_pVideoDriver->setOGLFilter(Parser.getIntValue("OGLfilter","Video"));
		g_pVideoDriver->setZoom(Parser.getIntValue("scale","Video"));
		g_pVideoDriver->setSpecialFXMode(Parser.getIntValue("specialfx","Video"));

		g_pTimer->setFrameRate(DEFAULT_LPS, Parser.getIntValue("autoframeskip","Video"), DEFAULT_SYNC);
		
		g_pCamera->setScrollTriggers(boundl,boundu,boundr,boundd,sspeed);
		
		g_pVideoDriver->setFilter(Parser.getIntValue("filter","Video"));
		
		g_pVideoDriver->enableOpenGL(Parser.getIntValue("OpenGL","Video") == 1);
		
		g_pSound->setSoundmode(Parser.getIntValue("rate","Audio"),
							   Parser.getIntValue("channels","Audio") == 2, Parser.getIntValue("format","Audio"));

		g_pSound->setMusicVolume(Parser.getIntValue("musicvol","Audio")*8);
		g_pSound->setSoundVolume(Parser.getIntValue("soundvol","Audio")*8);
	}
	return true;
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
void CSettings::setOption( int opt, const std::string &menuname, const std::string &name, char value)
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
	setOption( OPT_CHEATS,			"Extra Cheats     ", "allcheats", 0 );
	setOption( OPT_ANALOGJOYSTICK,	"Analog Joystick  ", "analog-joystick", 1 );
	setOption( OPT_LVLREPLAYABILITY,"Replay Levels    ", "level replayability", 0 );
	setOption( OPT_RISEBONUS,		"Rising Bonus     ", "rise bonus", 1 );
	setOption( OPT_SWITCHSCORES,	"Score Fix (EP3)  ", "switch scores", 0 );
}

/**
 * \brief  This loads the options of the settings
 *
 * \return			true if options could be loaded, else false
 */
bool CSettings::loadGameCfg()
{
	int i;
	CParser Parser;
	
	if(!checkOptionPtr()) return false;

	loadDefaultGameCfg();

	if(!Parser.loadParseFile())
		return false;

	for (i = 0; i < NUM_OPTIONS; i++)
	{
		char newvalue = Parser.getIntValue(mp_option[i].name,"Game");
		if(newvalue == 1 || newvalue == 0) // can only be true or false!!
			mp_option[i].value = newvalue;
	}
	
	g_pLogFile->ftextOut("<br>Your personal settings were loaded successfully...<br>");
	return true;
}

/**
 * \brief  Saves the options in the settings
 */
void CSettings::saveGameCfg()
{
	CParser Parser;
	Parser.loadParseFile();
	
	if(!checkOptionPtr()) return;

	for (int i = 0; i < NUM_OPTIONS; i++)
		Parser.saveIntValue(mp_option[i].name,"Game",mp_option[i].value);
	
	Parser.saveParseFile();
}

CSettings::~CSettings() {}
