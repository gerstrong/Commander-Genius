/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CVideoSettings.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CSettings.h"
#include "../../StringUtils.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CVideoSettings::CVideoSettings(Uint8 dlg_theme, bool &restartVideo) :
CBaseMenu(dlg_theme),
m_showFPS(g_pVideoDriver->showfps),
m_restartVideo(restartVideo),
mp_CameraSettings(NULL)
{
	m_current = -1;
	m_changed = false;
	
	m_Resolution.width = g_pVideoDriver->getWidth();
	m_Resolution.height = g_pVideoDriver->getHeight();
	m_Resolution.depth = g_pVideoDriver->getDepth();
	m_FSmode =  g_pVideoDriver->getFullscreen();
	g_pVideoDriver->initResolutionList();
	
	const unsigned short BUFFER_SIZE=256;
	char Buffer[BUFFER_SIZE];
	
	// Obtain the video driver name
	if (!SDL_VideoDriverName(Buffer, BUFFER_SIZE))
		m_usedSoftwareVideoDriver = Buffer;
	else
		m_usedSoftwareVideoDriver = "SDL";
	
	m_Opengl = g_pVideoDriver->isOpenGL();
	
	m_Zoom = g_pVideoDriver->getZoomValue();
	m_ScaleXFilter = g_pVideoDriver->getFiltermode();
	m_OGL_filter = g_pVideoDriver->getOGLFilter();
	m_Autoframeskip = g_pTimer->getFrameRate();
	m_SpecialFX = g_pVideoDriver->getSpecialFXConfig();
	
	std::string buf;
	mp_Dialog = new CDialog(29, 11, INPUT_MODE_OPTION, m_dlg_theme);
	
	buf = "Resolution: " + itoa(m_Resolution.width) + "x" + itoa(m_Resolution.height) + "x" + itoa(m_Resolution.depth);
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);
	std::list<st_resolution> :: iterator i;
			for( i = g_pVideoDriver->m_Resolutionlist.begin() ; i != g_pVideoDriver->m_Resolutionlist.end() ; i++ )
			{
				if(i->width  == m_Resolution.width  &&
				   i->height == m_Resolution.height &&
				   i->depth  == m_Resolution.depth)
				{
					mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = i->value;
					break;
				}
			}
	
	buf = m_FSmode ? "Fullscreen mode" : "Windowed mode";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);
	
	buf = "Mode: ";
	buf += (m_Opengl) ? "OpenGL" : m_usedSoftwareVideoDriver;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);
	
	if(!m_Opengl) {
		buf = "Zoom: ";
		buf += (m_Zoom == 1) ? "None" : itoa(m_Zoom) + "x";
	}
	else {
		buf = "GL Filter: ";
		buf += (m_OGL_filter==1) ? "Linear" : "Nearest";
	}
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, buf);
	mp_Dialog->m_dlgobject.at(3)->m_Option->m_value = m_Zoom;
	
	buf = "Filter: ";
	if(m_ScaleXFilter <= 4)
		buf += (!m_ScaleXFilter) ? "None" : itoa(m_ScaleXFilter)+"x";
	else
		buf += "Unknown Filter";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, buf);
	mp_Dialog->m_dlgobject.at(4)->m_Option->m_value = m_ScaleXFilter;
	
	buf = "Frameskip: " + itoa(m_Autoframeskip) + " fps";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, buf);
	mp_Dialog->m_dlgobject.at(5)->m_Option->m_value = m_Autoframeskip/10;
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "");
	buf = "Special FX ";
	buf += (m_SpecialFX) ? 28 : 20; //This adds in the switch seen in the options menu, which I think is fitting for any on/off option
	buf += (m_SpecialFX) ? 29 : 21;
	buf += (m_SpecialFX) ? 30 : 22;
	buf += (m_SpecialFX) ? 31 : 23;
	mp_Dialog->m_dlgobject.at(6)->m_Option->m_FontMapID = 1;
	mp_Dialog->setObjectText(6, buf);
	
	m_showFPS = g_pVideoDriver->showfps;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "");
	buf = "Show FPS ";
	buf += (m_showFPS) ? 28 : 20; //This adds in the switch seen in the options menu, which I think is fitting for any on/off option
	buf += (m_showFPS) ? 29 : 21;
	buf += (m_showFPS) ? 30 : 22;
	buf += (m_showFPS) ? 31 : 23;
	mp_Dialog->m_dlgobject.at(7)->m_Option->m_FontMapID = 1;
	mp_Dialog->setObjectText(7, buf);


	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, "Adjust Camera Bounds");
}

void CVideoSettings::processSpecific(){
	
	if(!mp_CameraSettings)
	{
		std::string buf;
		m_changed = true;
		
		if( m_mustclose )
		{
			if(m_Resolution.width != g_pVideoDriver->getWidth() or m_Resolution.height != g_pVideoDriver->getHeight() or m_Resolution.depth != g_pVideoDriver->getDepth() or m_FSmode !=  g_pVideoDriver->getFullscreen() or m_Opengl != g_pVideoDriver->isOpenGL() or m_Zoom != g_pVideoDriver->getZoomValue() or m_ScaleXFilter != g_pVideoDriver->getFiltermode() or m_OGL_filter != g_pVideoDriver->getOGLFilter() or m_Autoframeskip != g_pTimer->getFrameRate() or m_SpecialFX != g_pVideoDriver->getSpecialFXConfig())
			{
				g_pVideoDriver->stop();
				g_pVideoDriver->isFullscreen(m_FSmode);
				g_pVideoDriver->enableOpenGL(m_Opengl);
				g_pVideoDriver->setOGLFilter(m_OGL_filter);
				g_pVideoDriver->setZoom(m_Zoom);
				g_pVideoDriver->setFilter(m_ScaleXFilter);
				g_pTimer->setFrameRate(DEFAULT_LPS, m_Autoframeskip, DEFAULT_SYNC);
				g_pVideoDriver->setMode(m_Resolution);
				g_pVideoDriver->setSpecialFXMode(m_SpecialFX);
				g_pVideoDriver->start();
				
				CSettings Settings;
				Settings.saveDrvCfg();
				
				// And close this menu...
				m_restartVideo = true;
				m_mustclose = true;
			}
		}
		
		if( m_current != mp_Dialog->getSelection())
		{
			m_current = mp_Dialog->getSelection();
		}
		else if( m_current != NO_SELECTION)
		{
			if(m_current == 0) //This is the main reason I implemented this to begin with, as I find it much easier to find the resolution I want if I can navigate both directions.
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = g_pVideoDriver->m_Resolutionlist.size();
				setValues(0, mp_Dialog->m_dlgobject.at(0)->m_Option->m_value);
				buf = "Resolution: " + itoa(m_Resolution.width) + "x" + itoa(m_Resolution.height) + "x" + itoa(m_Resolution.depth);
				mp_Dialog->setObjectText(0,buf);
			}
			else if(m_current == 3)
			{
				if(!m_Opengl) {
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 4;
				setValues(3, mp_Dialog->m_dlgobject.at(3)->m_Option->m_value);
					buf = "Zoom: ";
					buf += (m_Zoom == 1) ? "None" : itoa(m_Zoom) + "x";
				}
				else {
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 1;
					buf = "GL Filter: ";
					buf += (m_OGL_filter==1) ? "Linear" : "Nearest";
				}
				setValues(3, mp_Dialog->m_dlgobject.at(3)->m_Option->m_value);
				mp_Dialog->setObjectText(3,buf);
			}
			else if(m_current == 4)
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 4;
				setValues(4, mp_Dialog->m_dlgobject.at(4)->m_Option->m_value);
				buf = "Filter: ";
				if(m_ScaleXFilter <= 4)
					buf += (!m_ScaleXFilter) ? "None" : itoa(m_ScaleXFilter)+"x";
				else
				{
					m_ScaleXFilter = 1;
					buf += (!m_ScaleXFilter) ? "None" : itoa(m_ScaleXFilter)+"x";
				}
				mp_Dialog->setObjectText(4, buf);
			}
			else if(m_current == 5)
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 12;
				setValues(5, mp_Dialog->m_dlgobject.at(5)->m_Option->m_value);
				buf = "Frameskip: " + itoa(m_Autoframeskip) + " fps";
				mp_Dialog->setObjectText(5, buf);
			}
		}
		
		if( m_selection != NO_SELECTION)  //I left this in so that people won't be confused and believe something is wrong
		{
			if(m_selection == 0)
			{
				// Now the part of the resolution list
				mp_Dialog->m_dlgobject.at(0)->m_Option->m_value++;
					if(mp_Dialog->m_dlgobject.at(0)->m_Option->m_value>mp_Dialog->m_max) mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 1;
					setValues(0,mp_Dialog->m_dlgobject.at(0)->m_Option->m_value);
			}
			else if(m_selection == 1)
			{
				m_FSmode = !m_FSmode;
				mp_Dialog->setObjectText(1,(m_FSmode) ? "Fullscreen mode" : "Windowed mode");
			}
			else if(m_selection == 2)
			{
#ifdef USE_OPENGL
				m_Opengl = !m_Opengl;
#else
				m_Opengl = false;
#endif
				buf = "Mode: ";
				buf += (m_Opengl) ? "OpenGL            " : m_usedSoftwareVideoDriver;
				mp_Dialog->setObjectText(2,buf);
				
				if(!m_Opengl) {
					buf = "Zoom: ";
					buf += (m_Zoom == 1) ? "None" : itoa(m_Zoom) + "x";
					mp_Dialog->setObjectType(3, DLG_OBJ_OPTION_TEXT);
				}
				else {
					buf = "GL Filter: ";
					buf += (m_OGL_filter==1) ? "Linear" : "Nearest";
					mp_Dialog->setObjectType(3, DLG_OBJ_OPTION_TEXT);
				}
				mp_Dialog->setObjectText(3,buf);
			}
			else if(m_selection == 3)
			{
				if(!m_Opengl) {
					mp_Dialog->m_dlgobject.at(3)->m_Option->m_value++;
					if(mp_Dialog->m_dlgobject.at(3)->m_Option->m_value>4) mp_Dialog->m_dlgobject.at(3)->m_Option->m_value = 1;
					setValues(3,mp_Dialog->m_dlgobject.at(3)->m_Option->m_value);
				}
				else {
					m_OGL_filter = !m_OGL_filter;
				}
			}
			else if(m_selection == 4)
			{
				mp_Dialog->m_dlgobject.at(4)->m_Option->m_value++;
					if(mp_Dialog->m_dlgobject.at(4)->m_Option->m_value>4) mp_Dialog->m_dlgobject.at(4)->m_Option->m_value = 1;
					setValues(4,mp_Dialog->m_dlgobject.at(4)->m_Option->m_value);
			}
			else if(m_selection == 5)
			{
				mp_Dialog->m_dlgobject.at(5)->m_Option->m_value++;
					if(mp_Dialog->m_dlgobject.at(5)->m_Option->m_value>12) mp_Dialog->m_dlgobject.at(5)->m_Option->m_value = 1;
					setValues(5,mp_Dialog->m_dlgobject.at(5)->m_Option->m_value);
			}
			else if(m_selection == 6)
			{
				m_SpecialFX = !m_SpecialFX;
				buf = "Special FX ";
				buf += (m_SpecialFX) ? 28 : 20;
				buf += (m_SpecialFX) ? 29 : 21;
				buf += (m_SpecialFX) ? 30 : 22;
				buf += (m_SpecialFX) ? 31 : 23;
				mp_Dialog->m_dlgobject.at(6)->m_Option->m_FontMapID = 1;
				mp_Dialog->setObjectText(6, buf);
			}
			else if(m_selection == 7)
			{
				m_showFPS = !m_showFPS;
				buf = "Show FPS ";
				buf += (m_showFPS) ? 28 : 20;
				buf += (m_showFPS) ? 29 : 21;
				buf += (m_showFPS) ? 30 : 22;
				buf += (m_showFPS) ? 31 : 23;
				mp_Dialog->m_dlgobject.at(7)->m_Option->m_FontMapID = 1;
				mp_Dialog->setObjectText(7, buf);
			}
			else if(m_selection == 8)
			{
				mp_CameraSettings = new CCameraSettings(m_dlg_theme);
				m_suspended = true;
			}
			m_selection = NO_SELECTION;
		}
	}
	else
	{
		mp_CameraSettings->processCommon();
		mp_CameraSettings->processSpecific();
		mp_CameraSettings->postProcess();
		
		if(mp_CameraSettings->mustClose())
		{
			SAFE_DELETE(mp_CameraSettings);
			m_suspended = false;
		}
	}
}

void CVideoSettings::setValues(int item, int value)
{
	if(item == 0)
	{
		m_Resolution = g_pVideoDriver->setResolution(value);
	}
	else if(item == 3)
	{
		switch(value)
		{
			case 1: m_Zoom = 1; break;
			case 2: m_Zoom = 2; break;
			case 3: m_Zoom = 3; break;
			case 4: m_Zoom = 4; break;
		}
	}
	else if(item == 4)
	{
		switch(value)
		{
			case 1: m_ScaleXFilter = 1; break;
			case 2: m_ScaleXFilter = 2; break;
			case 3: m_ScaleXFilter = 3; break;
			case 4: m_ScaleXFilter = 4; break;
		}
	}
	else if(item == 5)
	{
		switch(value)
		{
			case 1: m_Autoframeskip = 10; break;
			case 2: m_Autoframeskip = 20; break;
			case 3: m_Autoframeskip = 30; break;
			case 4: m_Autoframeskip = 40; break;
			case 5: m_Autoframeskip = 50; break;
			case 6: m_Autoframeskip = 60; break;
			case 7: m_Autoframeskip = 70; break;
			case 8: m_Autoframeskip = 80; break;
			case 9: m_Autoframeskip = 90; break;
			case 10: m_Autoframeskip = 100; break;
			case 11: m_Autoframeskip = 110; break;
			case 12: m_Autoframeskip = 120; break;
		}
	}
}
