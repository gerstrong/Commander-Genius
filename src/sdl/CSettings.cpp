/*
 * CSettings.cpp
 *
 *  Created on: 08.06.2009
 *      Author: gerstrong
 */

#include "../fileio/CParser.h"
#include "../CLogFile.h"
#include "CSettings.h"
#include "CVideoDriver.h"
#include "CTimer.h"
#include "sound/CSound.h"
#include "../FindFile.h"
#include "../ConfigHandler.h"


std::string CONFIGFILENAME = "genius.cfg";


CSettings::CSettings() {
	notes << "Reading game options from " << GetFullFileName(CONFIGFILENAME) << endl;
	notes << "Will write game options to " << GetWriteFullFileName(CONFIGFILENAME, true) << endl;
}

CSettings::~CSettings() {}


short CSettings::saveDrvCfg(void)
{
	short retval = 0;

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
	Parser.saveIntValue("autoframeskip","Video",g_pTimer->getFrameRate());
	Parser.saveIntValue("aspect","Video",g_pVideoDriver->getAspectCorrection() ? 1 : 0);

	Parser.saveIntValue("channels","Audio",(g_pSound->getAudioSpec()).channels);
	Parser.saveIntValue("format","Audio",(g_pSound->getAudioSpec()).format);
	Parser.saveIntValue("rate","Audio",(g_pSound->getAudioSpec()).freq);
	Parser.saveIntValue("mixerch","Audio",(g_pSound->getMixingchannels()));

	Parser.saveParseFile();

	return retval;
}

short CSettings::loadDrvCfg()
{	
	short retval = 0;
	CParser Parser;

	if(!Parser.loadParseFile())
	{
		retval = 1;
	}
	else
	{
		int width, height, depth;

		depth  = Parser.getIntValue("bpp","Video");
		width  = Parser.getIntValue("width","Video");
		height = Parser.getIntValue("height","Video");


		if(depth*width*height < 0)
			g_pLogFile->ftextOut(RED,"Error reading the configuration file. It appears to be damaged!");
		g_pVideoDriver->setMode(width,height,depth);
		g_pVideoDriver->isFullscreen(((Parser.getIntValue("fullscreen","Video")) == 1));
		g_pVideoDriver->setOGLFilter(Parser.getIntValue("OGLfilter","Video"));
		g_pVideoDriver->setZoom(Parser.getIntValue("scale","Video"));
		g_pTimer->setFrameRate(Parser.getIntValue("autoframeskip","Video"));

		g_pVideoDriver->setFilter(Parser.getIntValue("filter","Video"));

		g_pVideoDriver->enableOpenGL(Parser.getIntValue("OpenGL","Video") == 1);

		g_pVideoDriver->setAspectCorrection(Parser.getIntValue("aspect","Video") == 1);

		g_pSound->setSoundmode(Parser.getIntValue("rate","Audio"),
						Parser.getIntValue("channels","Audio") == 2);
	}

	return retval;
}

void CSettings::setOption(stOption *options, int opt, const char *name, char value)
{
  if (name != NULL)
    options[opt].name = (char*) name;

  options[opt].value = value;
}

void CSettings::loadDefaultGameCfg(stOption *Option)
{
	setOption(Option,OPT_FULLYAUTOMATIC, "autogun", 0);
	setOption(Option,OPT_SUPERPOGO, "superpogo", 0);
	setOption(Option,OPT_ALLOWPKING, "pking", 1);
	setOption(Option,OPT_CHEATS, "allcheats", 0);
	setOption(Option,OPT_TWOBUTTON, "two-button-firing", 0);
	setOption(Option,OPT_ANALOGJOYSTICK, "analog-joystick", 1);
	setOption(Option,OPT_LVLREPLAYABILITY, "level replayability", 0);
	setOption(Option,OPT_RISEBONUS, "rise bonus", 1);
}

short CSettings::loadGameCfg(stOption *Option)
{
	int i;
	CParser Parser;

	if(!Parser.loadParseFile()) {
		loadDefaultGameCfg(Option);
		return 1;
	}

	  for (i = 0; i < NUM_OPTIONS; i++)
	  {
		  Option[i].value = Parser.getIntValue(Option[i].name,"Game");
		  if(Option[i].value == -1)
		  {
			  loadDefaultGameCfg(Option);
			  return 1;
		  }
	  }


  g_pLogFile->ftextOut("<br>Your personal settings were loaded successfully...<br>");
  return 0;
}

void CSettings::saveGameCfg(stOption *Option)
{
	CParser Parser;
	Parser.loadParseFile();
	
	for (int i = 0; i < NUM_OPTIONS; i++)
		Parser.saveIntValue(Option[i].name,"Game",Option[i].value);

	Parser.saveParseFile();

	return;
}

