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

CSettings::CSettings(stOption *p_option) {
	notes << "Reading game options from " << GetFullFileName(CONFIGFILENAME) << endl;
	notes << "Will write game options to " << GetWriteFullFileName(CONFIGFILENAME, true) << endl;
	mp_option = p_option;
}

short CSettings::saveDrvCfg()
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
	
	Parser.saveIntValue("left","Bound",g_pCamera->getScrollLeft());
	Parser.saveIntValue("right","Bound",g_pCamera->getScrollRight());
	Parser.saveIntValue("up","Bound",g_pCamera->getScrollUp());
	Parser.saveIntValue("down","Bound",g_pCamera->getScrollDown());
	Parser.saveIntValue("speed","Bound",g_pCamera->getScrollSpeed());
	
	Parser.saveIntValue("channels","Audio",(g_pSound->getAudioSpec()).channels);
	Parser.saveIntValue("format","Audio",(g_pSound->getAudioSpec()).format);
	Parser.saveIntValue("rate","Audio",(g_pSound->getAudioSpec()).freq);
	Parser.saveIntValue("mixerch","Audio",(g_pSound->getMixingchannels()));
	
	Parser.saveParseFile();
	
	return retval;
}

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
		g_pTimer->setFrameRate(DEFAULT_LPS, Parser.getIntValue("autoframeskip","Video"), DEFAULT_SYNC);
		
		g_pCamera->setScrollTriggers(boundl,boundu,boundr,boundd,sspeed);
		
		g_pVideoDriver->setFilter(Parser.getIntValue("filter","Video"));
		
		g_pVideoDriver->enableOpenGL(Parser.getIntValue("OpenGL","Video") == 1);
		
		g_pSound->setSoundmode(Parser.getIntValue("rate","Audio"),
							   Parser.getIntValue("channels","Audio") == 2, Parser.getIntValue("format","Audio"));
	}
	return true;
}

bool CSettings::checkOptionPtr() {
	if(mp_option) return true;
	g_pLogFile->textOut("ERROR in Code implementation! There is an error in the source code. Pointer mp_option cannot be used here!\n");
	return false;
}

void CSettings::setOption( int opt, const std::string &menuname, const std::string &name, char value)
{
	if(!checkOptionPtr()) return;
	mp_option[opt].menuname = menuname;
	mp_option[opt].name = name;
	mp_option[opt].value = value;
}

void CSettings::loadDefaultGameCfg()
{
	if(!checkOptionPtr()) return;
	setOption( OPT_FULLYAUTOMATIC, "Machine Gun", "autogun", 0 );
	setOption( OPT_SUPERPOGO, "Super Pogo Stick", "superpogo", 0 );
	setOption( OPT_ALLOWPKING, "Friendly Fire", "pking", 1 );
	setOption( OPT_CHEATS, "Extra Cheats", "allcheats", 0 );
	setOption( OPT_ANALOGJOYSTICK, "Analog Joystick", "analog-joystick", 1 );
	setOption( OPT_LVLREPLAYABILITY, "Replay Levels", "level replayability", 0 );
	setOption( OPT_RISEBONUS, "Rising Bonus", "rise bonus", 1 );
	setOption( OPT_SWITCHSCORES, "Score Fix (EP3)", "switch scores", 0 );
}

bool CSettings::loadGameCfg()
{
	int i;
	CParser Parser;
	
	if(!checkOptionPtr()) return false;

	if(!Parser.loadParseFile()) {
		loadDefaultGameCfg();
		return false;
	}
	
	loadDefaultGameCfg();
	
	for (i = 0; i < NUM_OPTIONS; i++)
	{
		mp_option[i].value = Parser.getIntValue(mp_option[i].name,"Game");
		if(mp_option[i].value == -1)
		{
			//loadDefaultGameCfg();
			return 1;
		}
	}
	
	g_pLogFile->ftextOut("<br>Your personal settings were loaded successfully...<br>");
	return true;
}

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
