/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "sdl/input/CInput.h"
#include "sdl/CTimer.h"
#include "StringUtils.h"
#include "CVideoSettings.h"
#include "common/CSettings.h"


#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CVideoSettings::CVideoSettings(Uint8 dlg_theme, bool &restartVideo) :
CBaseMenu(dlg_theme),
m_Vidconfig(g_pVideoDriver->getVidConfig()),
m_Resolutionlist(g_pVideoDriver->m_Resolutionlist),
m_restartVideo(restartVideo),
mp_CameraSettings(NULL)
{
	m_current = -1;
	m_changed = false;
	
	g_pVideoDriver->initResolutionList();
	
	m_Resolution_pos = m_Resolutionlist.begin();

	const unsigned short BUFFER_SIZE=256;
	char Buffer[BUFFER_SIZE];
	
	// Obtain the video driver name
	if (!SDL_VideoDriverName(Buffer, BUFFER_SIZE))
		m_usedSoftwareVideoDriver = Buffer;
	else
		m_usedSoftwareVideoDriver = "SDL";
	
	std::string buf;
	mp_Dialog = new CDialog(29, 11, INPUT_MODE_OPTION, m_dlg_theme);
	
	CRect<Uint16> &Res = m_Vidconfig.m_DisplayRect;

	buf = "Resolution: " + itoa(Res.w) + "x" + itoa(Res.h) + "x" + itoa(32);
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);
	
	buf = m_Vidconfig.Fullscreen ? "Fullscreen mode" : "Windowed mode";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);
	
	buf = "Mode: ";
	buf += (m_Vidconfig.m_opengl) ? "OpenGL" : m_usedSoftwareVideoDriver;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);
	
#ifdef USE_OPENGL
	if(m_Vidconfig.m_opengl) {
		buf = "GL Filter: ";
		buf += (m_Vidconfig.m_opengl_filter==1) ? "Linear" : "Nearest";
	}
	else
#endif
	{
		buf = "Zoom: ";
		buf += (m_Vidconfig.Zoom == 1) ? "None" : itoa(m_Vidconfig.Zoom) + "x";
	}

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, buf);
	mp_Dialog->m_dlgobject.at(3)->m_Option->m_value = m_Vidconfig.Zoom;
	
	buf = "Filter: ";
	if(m_Vidconfig.m_ScaleXFilter <= 4)
		buf += (!m_Vidconfig.m_ScaleXFilter) ? "None" : itoa(m_Vidconfig.m_ScaleXFilter)+"x";
	else
		buf += "Unknown Filter";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, buf);
	mp_Dialog->m_dlgobject.at(4)->m_Option->m_value = m_Vidconfig.m_ScaleXFilter;
	
	m_Autoframeskip = g_pTimer->getFrameRate();
	buf = "Frameskip: " + itoa(m_Autoframeskip) + " fps";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, buf);
	mp_Dialog->m_dlgobject.at(5)->m_Option->m_value = m_Autoframeskip;
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "");
	buf = "Special FX " + getSwitchString(m_Vidconfig.m_special_fx);
	mp_Dialog->m_dlgobject.at(6)->m_Option->m_FontMapID = 1;
	mp_Dialog->setObjectText(6, buf);
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "");
	buf = "Show FPS " + getSwitchString(m_Vidconfig.showfps);
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
			if( m_Vidconfig !=  g_pVideoDriver->getVidConfig() )
			{
				g_pVideoDriver->stop();
				g_pVideoDriver->setVidConfig(m_Vidconfig);
				g_pTimer->setFrameRate(DEFAULT_LPS, m_Autoframeskip, DEFAULT_SYNC);
				g_pVideoDriver->start();
				
				g_pSettings->saveDrvCfg();
				
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
			if(m_current == 0)
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = g_pVideoDriver->m_Resolutionlist.size();

				CRect<Uint16> &Res = m_Vidconfig.m_DisplayRect;
				buf = "Resolution: " + itoa(Res.w) + "x" + itoa(Res.h);
				mp_Dialog->setObjectText(0,buf);
			}
			else if(m_current == 3)
			{

#ifdef USE_OPENGL
				if(m_Vidconfig.m_opengl)
				{
					mp_Dialog->m_min = 1;
					mp_Dialog->m_max = 2;
						buf = "GL Filter: ";
						buf += (m_Vidconfig.m_opengl_filter==1) ? "Linear" : "Nearest";
				}
				else
#endif
				{

					mp_Dialog->m_min = 1;
					mp_Dialog->m_max = 4;
					m_Vidconfig.Zoom = mp_Dialog->m_dlgobject.at(3)->m_Option->m_value;

					buf = "Zoom: ";
					buf += (m_Vidconfig.Zoom == 1) ? "None" : itoa(m_Vidconfig.Zoom) + "x";
				}

				m_Vidconfig.Zoom = mp_Dialog->m_dlgobject.at(3)->m_Option->m_value;
				mp_Dialog->setObjectText(3,buf);
			}
			else if(m_current == 4)
			{
				mp_Dialog->m_min = 1;
				mp_Dialog->m_max = 4;
				m_Vidconfig.m_ScaleXFilter = mp_Dialog->m_dlgobject.at(4)->m_Option->m_value;
				buf = "Filter: ";
				if(m_Vidconfig.m_ScaleXFilter <= 4)
					buf += (!m_Vidconfig.m_ScaleXFilter) ? "None" : itoa(m_Vidconfig.m_ScaleXFilter)+"x";
				else
				{
					m_Vidconfig.m_ScaleXFilter = 1;
					buf += (!m_Vidconfig.m_ScaleXFilter) ? "None" : itoa(m_Vidconfig.m_ScaleXFilter)+"x";
				}
				mp_Dialog->setObjectText(4, buf);
			}
			else if(m_current == 5)
			{
				mp_Dialog->m_min = 10;
				mp_Dialog->m_max = 120;
				m_Autoframeskip = mp_Dialog->m_dlgobject.at(5)->m_Option->m_value;
				buf = "Frameskip: " + itoa(m_Autoframeskip) + " fps";
				mp_Dialog->setObjectText(5, buf);
			}
		}
		
		if( m_selection != NO_SELECTION)
		{
			if(m_selection == 0)
			{
				// Now the part of the resolution list
				mp_Dialog->m_dlgobject.at(0)->m_Option->m_value++;
					if(mp_Dialog->m_dlgobject.at(0)->m_Option->m_value>mp_Dialog->m_max)
						mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 1;

				m_Resolution_pos = m_Resolutionlist.begin();
				int value = mp_Dialog->m_dlgobject.at(0)->m_Option->m_value;
				for( ; value>0 ; value-- )	m_Resolution_pos++;
				m_Vidconfig.m_DisplayRect = *m_Resolution_pos;
			}
			else if(m_selection == 1)
			{
				m_Vidconfig.Fullscreen = !m_Vidconfig.Fullscreen;
				mp_Dialog->setObjectText(1,(m_Vidconfig.Fullscreen) ? "Fullscreen mode" : "Windowed mode");
			}
			else if(m_selection == 2)
			{
#ifdef USE_OPENGL
				m_Vidconfig.m_opengl = !m_Vidconfig.m_opengl;
#else
				m_Vidconfig.m_opengl = false;
#endif
				buf = "Mode: ";
				buf += (m_Vidconfig.m_opengl) ? "OpenGL            " : m_usedSoftwareVideoDriver;
				mp_Dialog->setObjectText(2,buf);
				

#ifdef USE_OPENGL
				if(m_Vidconfig.m_opengl)
				{
					buf = "GL Filter: ";
					buf += (m_Vidconfig.m_opengl_filter==1) ? "Linear" : "Nearest";
					mp_Dialog->setObjectType(3, DLG_OBJ_OPTION_TEXT);
				}
				else
#endif
				{

					buf = "Zoom: ";
					buf += (m_Vidconfig.Zoom == 1) ? "None" : itoa(m_Vidconfig.Zoom) + "x";
					mp_Dialog->setObjectType(3, DLG_OBJ_OPTION_TEXT);
				}
				mp_Dialog->setObjectText(3,buf);
			}
			else if(m_selection == 3)
			{
#ifdef USE_OPENGL
				if(m_Vidconfig.m_opengl)
				{
					m_Vidconfig.m_opengl_filter = !m_Vidconfig.m_opengl_filter;
				}
				else
#endif
				{

					mp_Dialog->m_dlgobject.at(3)->m_Option->m_value++;
					if(mp_Dialog->m_dlgobject.at(3)->m_Option->m_value>4)
						mp_Dialog->m_dlgobject.at(3)->m_Option->m_value = 1;
					m_Vidconfig.Zoom = mp_Dialog->m_dlgobject.at(3)->m_Option->m_value;

				}
			}
			else if(m_selection == 4)
			{
				mp_Dialog->m_dlgobject.at(4)->m_Option->m_value++;
					if(mp_Dialog->m_dlgobject.at(4)->m_Option->m_value>4)
						mp_Dialog->m_dlgobject.at(4)->m_Option->m_value = 1;
					m_Vidconfig.m_ScaleXFilter = mp_Dialog->m_dlgobject.at(4)->m_Option->m_value;
			}
			else if(m_selection == 5)
			{
				mp_Dialog->m_dlgobject.at(5)->m_Option->m_value+=10;
					if(mp_Dialog->m_dlgobject.at(5)->m_Option->m_value>120) mp_Dialog->m_dlgobject.at(5)->m_Option->m_value = 10;
					m_Autoframeskip = mp_Dialog->m_dlgobject.at(5)->m_Option->m_value;
			}
			else if(m_selection == 6)
			{
				bool &fx = m_Vidconfig.m_special_fx;
				fx = !fx;
				buf = "Special FX " + getSwitchString(fx);
				mp_Dialog->m_dlgobject.at(6)->m_Option->m_FontMapID = 1;
				mp_Dialog->setObjectText(6, buf);
			}
			else if(m_selection == 7)
			{
				m_Vidconfig.showfps = !m_Vidconfig.showfps;
				buf = "Show FPS " + getSwitchString(m_Vidconfig.showfps);
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
