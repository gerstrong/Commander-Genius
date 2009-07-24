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
#include "sound/CSound.h"
#include "../FindFile.h"
#include "../ConfigHandler.h"


void InitSearchPaths() {
	// have to set to find the config at some of the default places
	InitBaseSearchPaths();
	
	int i = 1;
	while(true) {
		std::string value;
		if(!ReadString(CONFIGFILENAME, "FileHandling", "SearchPath" + itoa(i), value, ""))
			break;
		
		AddToFileList(&tSearchPaths, value);
		i++;
	}
	
	// add the basesearchpaths to the searchpathlist as they should be saved in the end
	for(searchpathlist::const_iterator p1 = basesearchpaths.begin(); p1 != basesearchpaths.end(); i++,p1++)  {
		AddToFileList(&tSearchPaths, *p1);
	}
	
	// print the searchpaths, this may be very usefull for the user
	notes << "I have now the following searchpaths (in this order):\n";
	for(searchpathlist::const_iterator p2 = tSearchPaths.begin(); p2 != tSearchPaths.end(); p2++) {
		std::string path = *p2;
		ReplaceFileVariables(path);
		notes << "  " << path << "\n";
	}
	notes << " And that's all." << endl;
}



CSettings::CSettings() {
	// TODO Auto-generated constructor stub

}

CSettings::~CSettings() {
	// TODO Auto-generated destructor stub
}


short CSettings::saveDrvCfg(void)
{
	short retval = 0;

	CParser Parser;

	Parser.saveIntValue("bpp","Video",g_pVideoDriver->getDepth());
	Parser.saveIntValue("frameskip","Video",g_pVideoDriver->getFrameskip());

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
	Parser.saveIntValue("autoframeskip","Video",g_pVideoDriver->getTargetFPS());
	Parser.saveIntValue("aspect","Video",g_pVideoDriver->getAspectCorrection() ? 1 : 0);

	Parser.saveIntValue("channels","Audio",(g_pSound->getAudioSpec()).channels);
	Parser.saveIntValue("format","Audio",(g_pSound->getAudioSpec()).format);
	Parser.saveIntValue("rate","Audio",(g_pSound->getAudioSpec()).freq);
	Parser.saveIntValue("mixerch","Audio",(g_pSound->getMixingchannels()));

	Parser.saveParseFile();

	return retval;
}

short CSettings::loadDrvCfg(void)
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
		g_pVideoDriver->setFrameskip(Parser.getIntValue("frameskip","Video"));

		if((Parser.getIntValue("fullscreen","Video")) == 1)
			g_pVideoDriver->isFullscreen(true);

		g_pVideoDriver->setOGLFilter(Parser.getIntValue("OGLfilter","Video"));
		g_pVideoDriver->setZoom(Parser.getIntValue("scale","Video"));
		g_pVideoDriver->setTargetFPS(Parser.getIntValue("autoframeskip","Video"));

		g_pVideoDriver->setFilter(Parser.getIntValue("filter","Video"));

		if(Parser.getIntValue("OpenGL","Video") == 1)
			g_pVideoDriver->enableOpenGL(true);
		else
			g_pVideoDriver->enableOpenGL(false);

		if(Parser.getIntValue("channels","Audio") == 2)
			g_pSound->setSoundmode(Parser.getIntValue("rate","Audio"), true);
		else
			g_pSound->setSoundmode(Parser.getIntValue("rate","Audio"), false);
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
	setOption(Option,OPT_KEYCARDSTACK, "keycard-stacking", 0);
	setOption(Option,OPT_ANALOGJOYSTICK, "analog-joystick", 1);
	setOption(Option,OPT_LVLREPLAYABILITY, "level replayability", 0);
}

short CSettings::loadGameCfg(stOption *Option)
{
	short retval = 0;
	int i;
	CParser Parser;

	if(!Parser.loadParseFile())
		return 1;

	  for (i = 0; i < NUM_OPTIONS; i++)
	  {
		  Option[i].value = Parser.getIntValue(Option[i].name,"Game");
		  if(Option[i].value == -1)
		  {
			  loadDefaultGameCfg(Option);
			  break;
		  }
	  }

	return retval;

  g_pLogFile->ftextOut("<br>Your personal settings were loaded successfully...<br>");
  return 0;
}

void CSettings::saveGameCfg(stOption *Option)
{
	int i;
	CParser Parser;

	if(Parser.loadParseFile())
	{
		for (i = 0; i < NUM_OPTIONS; i++)
			Parser.saveIntValue(Option[i].name,"Game",Option[i].value);

		Parser.saveParseFile();
	}

	return;
}

