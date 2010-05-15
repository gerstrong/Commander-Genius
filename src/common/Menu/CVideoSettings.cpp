/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CVideoSettings.h"
#include "../../engine/vorticon/CCamera.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CSettings.h"
#include "../../StringUtils.h"

CVideoSettings::CVideoSettings(char &menu_type, Uint8 dlg_theme) :
CBaseMenu(menu_type, dlg_theme) {
	
	m_changed = false;
	
	if(menu_type == BOUNDS)
	{
		m_left = g_pCamera->getScrollLeft();
		m_up = g_pCamera->getScrollUp();
		m_right = g_pCamera->getScrollRight();
		m_down = g_pCamera->getScrollDown();
		m_speed = g_pCamera->getScrollSpeed();
		
		std::string buf;
		mp_Dialog = new CDialog(18, 13, INPUT_MODE_UP_DOWN, m_dlg_theme);
		
		mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 1, " Adjust Bounds: ");
		mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 2, " Left (50-270): ");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, " "+itoa(m_left));
		mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 4, " Right (50-270):");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, " "+itoa(m_right));
		mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 6, " Up (50-150):   ");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, " "+itoa(m_up));
		mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 8, " Down (50-150): ");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, " "+itoa(m_down));
		mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 10, " Speed (1-50):  ");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 11, " "+itoa(m_speed));
		
		
		mp_Dialog->setInputMode(INPUT_MODE_COUNTER);
	}
	else
	{
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
		mp_Dialog = new CDialog(29, 10, INPUT_MODE_UP_DOWN, m_dlg_theme);
		
		buf = "Resolution: " + itoa(m_Resolution.width) + "x" + itoa(m_Resolution.height) + "x" + itoa(m_Resolution.depth);
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);
		
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
		
		buf = "Filter: ";
		if(m_ScaleXFilter <= 4)
			buf += (!m_ScaleXFilter) ? "None" : itoa(m_ScaleXFilter)+"x";
		else
			buf += "Unknown Filter";
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, buf);
		
		buf = "Frameskip: " + itoa(m_Autoframeskip) + " fps";
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, buf);
		
		buf = "Special FX: ";
		buf += (m_SpecialFX) ? "on" : "off" ;
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);

		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Adjust Camera Bounds");
	}
}

void CVideoSettings::processSpecific(){
	std::string buf;
	
	if( g_pInput->getPressedCommand(IC_QUIT) )
	{
		if(m_changed)
		{
			if(m_MenuType == BOUNDS)
			{
				g_pCamera->setScrollTriggers(m_left, m_up, m_right, m_down, m_speed);
				
				CSettings Settings;
				Settings.saveDrvCfg();
				
				m_MenuType = GRAPHICS;
				m_mustclose = true;
			}
			else
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
				m_MenuType = CONFIGURE;
				m_restartVideo = true;
				m_mustclose = true;
			}
		}
		else
		{
			m_mustclose = true;
			if(m_MenuType == BOUNDS)
				m_MenuType = GRAPHICS;
			else
				m_MenuType = CONFIGURE;
		}
	}
	
	if(m_MenuType == BOUNDS)
	{
		if(mp_Dialog->getSelection() == 2)
		{
			mp_Dialog->m_min = 50;
			mp_Dialog->m_max = 270;
			mp_Dialog->m_length = 3;
		}
		else if(mp_Dialog->getSelection() == 4)
		{
			mp_Dialog->m_min = 50;
			mp_Dialog->m_max = 270;
			mp_Dialog->m_length = 3;
		}
		else if(mp_Dialog->getSelection() == 6)
		{
			mp_Dialog->m_min = 50;
			mp_Dialog->m_max = 150;
			mp_Dialog->m_length = 3;
		}
		else if(mp_Dialog->getSelection() == 8)
		{
			mp_Dialog->m_min = 50;
			mp_Dialog->m_max = 150;
			mp_Dialog->m_length = 3;
		}
		else if(mp_Dialog->getSelection() == 10)
		{
			mp_Dialog->m_min = 1;
			mp_Dialog->m_max = 50;
			mp_Dialog->m_length = 2;
		}
		m_changed = true;
		m_left = atoi(mp_Dialog->m_dlgobject.at(2)->m_Option->m_text);
		m_right = atoi(mp_Dialog->m_dlgobject.at(4)->m_Option->m_text);
		m_up = atoi(mp_Dialog->m_dlgobject.at(6)->m_Option->m_text);
		m_down = atoi(mp_Dialog->m_dlgobject.at(8)->m_Option->m_text);
		m_speed = atoi(mp_Dialog->m_dlgobject.at(10)->m_Option->m_text);
	}
	
	
	if( m_selection != -1)
	{
		if(m_MenuType == BOUNDS)
		{
			if(mp_Dialog->getInputMode(INPUT_MODE_COUNTER))
			{
				mp_Dialog->m_name = "";
				mp_Dialog->setInputMode(INPUT_MODE_INTEGER);
				m_selection = -1;
			}
			else if (mp_Dialog->getInputMode(INPUT_MODE_INTEGER))
			{
				if(mp_Dialog->m_name == "")
				{
					mp_Dialog->setObjectText(m_selection, " "+itoa(mp_Dialog->m_int));
					mp_Dialog->m_name = itoa(mp_Dialog->m_int);
				}
				else
				{
					if(atoi(mp_Dialog->m_name)>mp_Dialog->m_max)
						mp_Dialog->m_name = itoa(mp_Dialog->m_max);
					else if(atoi(mp_Dialog->m_name)<mp_Dialog->m_min)
						mp_Dialog->m_name = itoa(mp_Dialog->m_min);
					mp_Dialog->setObjectText(m_selection, " "+mp_Dialog->m_name);
				}
				mp_Dialog->setInputMode(INPUT_MODE_COUNTER);
				m_selection = -1;
			}
		}
		else
		{
			if(m_selection < 7)
				m_changed = true;
			
			if(m_selection == 0)
			{
				// Now the part of the resolution list
				m_Resolution = g_pVideoDriver->getNextResolution();
				
				buf = "Resolution: " + itoa(m_Resolution.width) + "x" + itoa(m_Resolution.height) + "x" + itoa(m_Resolution.depth);
				mp_Dialog->setObjectText(0,buf);
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
					m_Zoom++; if(m_Zoom>4) m_Zoom = 1;
					buf = "Zoom: ";
					buf += (m_Zoom == 1) ? "None" : itoa(m_Zoom) + "x";
				}
				else {
					m_OGL_filter = !m_OGL_filter;
					buf = "GL Filter: ";
					buf += (m_OGL_filter==1) ? "Linear" : "Nearest";
				}
				mp_Dialog->setObjectText(3,buf);
			}
			else if(m_selection == 4)
			{
				m_ScaleXFilter++; if(m_ScaleXFilter>4) m_ScaleXFilter=1;
				buf = "Filter: ";
				if(m_ScaleXFilter <= 4)
					buf += (!m_ScaleXFilter) ? "None" : itoa(m_ScaleXFilter)+"x";
				else
					buf += "Unknown Filter";
				mp_Dialog->setObjectText(4, buf);
			}
			else if(m_selection == 5)
			{
				if(m_Autoframeskip < 120) m_Autoframeskip += 10;
				else m_Autoframeskip = 10;
				
				buf = "Frameskip: " + itoa(m_Autoframeskip) + " fps";
				mp_Dialog->setObjectText(5, buf);
			}
			else if(m_selection == 6)
			{
				m_SpecialFX = !m_SpecialFX;
				buf = "Special FX: ";
				buf += (m_SpecialFX) ? "on" : "off" ;
				mp_Dialog->setObjectText(6, buf);
			}
			else if(m_selection == 7)
			{
				if(m_changed)
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
					m_MenuType = BOUNDS;
					//m_restartVideo = true;
					m_mustclose = true;
				}
				else
				{
					m_mustclose = true;
					m_MenuType = BOUNDS;
				}
			}
			m_selection = -1;
		}
	}
}

CVideoSettings::~CVideoSettings() {
	delete mp_Dialog;
}
