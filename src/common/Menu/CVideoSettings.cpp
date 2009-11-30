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

CVideoSettings::CVideoSettings(char &menu_type) :
CBaseMenu(menu_type) {

	m_Resolution.width = g_pVideoDriver->getWidth();
	m_Resolution.height = g_pVideoDriver->getHeight();
	m_Resolution.depth = g_pVideoDriver->getDepth();
	m_Zoom = g_pVideoDriver->getZoomValue();
	m_ScaleXFilter = g_pVideoDriver->getFiltermode();
	m_OGL_filter = g_pVideoDriver->getOGLFilter();
	m_Autoframeskip = g_pTimer->getFrameRate();
	m_FSmode =  g_pVideoDriver->getFullscreen();
	m_AspectCorrection = g_pVideoDriver->getAspectCorrection(),
	m_Opengl = g_pVideoDriver->isOpenGL();
	std::string buf;

	mp_Dialog = new CDialog(g_pVideoDriver->FGLayerSurface, 32, 13);
	mp_Dialog->setFrameTheme(DLG_THEME_OLDSCHOOL);

	buf = "Resolution: " + itoa(m_Resolution.width) + "x" + itoa(m_Resolution.height) + "x" + itoa(m_Resolution.depth);
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);

	buf = m_FSmode ? "Fullscreen mode" : "Windowed mode";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);

	if(!m_Opengl)
	{
		buf = (m_Zoom == 1) ? "No Scale" : "Scale: " + itoa(m_Zoom);
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);
	}
	else
	{
		buf = "OGL Filter: ";
		buf += (m_OGL_filter==1) ? "Linear" : "Nearest";
	}

	if(m_ScaleXFilter <= 3 && m_ScaleXFilter >= 0)
		buf = (!m_ScaleXFilter) ? "No Filter" : "Scale"+itoa(m_ScaleXFilter)+"x Filter";
	else
		buf = "Unknown Filter";

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, buf);

	buf = (m_Opengl) ? "OpenGL Acceleration" : "Software Rendering";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, buf);

	buf = "Frameskip : " + itoa(m_Autoframeskip) + " fps";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, buf);

	buf = "OGL Aspect Ratio ";
	buf += m_AspectCorrection ? "enabled" : "disabled";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);

	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 9, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, "Confirm");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 11, "Cancel");
}

void CVideoSettings::processSpecific(){
	std::string buf;

	if( g_pInput->getPressedCommand(IC_QUIT) )
	{
		m_mustclose = true;
		m_MenuType = MAIN;
	}

	if( m_selection != -1)
	{
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
			mp_Dialog->setObjectText(0,buf);
		}
		else if(m_selection == 2)
		{
			if(m_Opengl)
			{
				m_OGL_filter = (m_OGL_filter==1) ? 0 : 1;
				buf = (m_OGL_filter == 1) ? "OGL Filter: Linear" : "OGL Filter: Nearest";
				mp_Dialog->setObjectText(2,buf);
			}
			else
			{
				m_Zoom = (m_Zoom >= 4) ? 1 : m_Zoom+1;
				buf = (m_Zoom==1) ? "No scale" : "Scale: " + itoa(m_Zoom);
				if(m_ScaleXFilter>0)
					m_ScaleXFilter = m_Zoom;
			}
			mp_Dialog->setObjectText(2,buf);

			buf = (m_ScaleXFilter==0) ? "No Filter" : "Scale"+itoa(m_ScaleXFilter)+"x Filter";
			mp_Dialog->setObjectText(3, buf);
		}
		else if(m_selection == 3)
		{
			m_ScaleXFilter = (m_ScaleXFilter >= 4) ? 1 : m_ScaleXFilter+1;

			buf = (m_ScaleXFilter==0) ? "No Filter" : "Scale"+itoa(m_ScaleXFilter)+"x Filter";
			mp_Dialog->setObjectText(3, buf);
		}
		else if(m_selection == 4)
		{
			m_Opengl = !m_Opengl; // switch the mode!!

			buf = (m_Opengl) ? "OpenGL Acceleration" : "Software Rendering";
			mp_Dialog->setObjectText(4, buf);
		}
		else if(m_selection == 5)
		{
			if(m_Autoframeskip < 120) m_Autoframeskip += 10;
			else m_Autoframeskip = 10;

			buf = "Auto-Frameskip : " + itoa(m_Autoframeskip) + " fps";
			mp_Dialog->setObjectText(5, buf);
		}
		else if(m_selection == 6)
		{
			m_AspectCorrection = !m_AspectCorrection;

			buf = "OGL Aspect Ratio ";
			buf += (m_AspectCorrection) ? "Enabled" : "Disabled";
			mp_Dialog->setObjectText(6, buf);
		}
		else if(m_selection == 8) // Set the chosen settings! (Confirm)
		{
			g_pVideoDriver->stop();
			g_pVideoDriver->isFullscreen(m_FSmode);
			g_pVideoDriver->enableOpenGL(m_Opengl);
			g_pVideoDriver->setOGLFilter(m_OGL_filter);
			g_pVideoDriver->setZoom(m_Zoom);
			g_pVideoDriver->setFilter(m_ScaleXFilter);
			g_pTimer->setFrameRate(DEFAULT_LPS, m_Autoframeskip, DEFAULT_SYNC);
			g_pVideoDriver->setAspectCorrection(m_AspectCorrection);
			g_pVideoDriver->setMode(m_Resolution);
			g_pVideoDriver->start();

			CSettings Settings;
			Settings.saveDrvCfg();

			// And close this menu...
			m_mustclose = true;
			m_MenuType = CONFIGURE;
			m_restartVideo = true;
			m_mustclose = true;
			mp_Dialog->setSDLSurface(g_pVideoDriver->FGLayerSurface);
		}
		else if(m_selection == 9) // Cancel (don't save)
		{
			// And close this menu...
			m_mustclose = true;
			m_MenuType = CONFIGURE;
		}
		m_selection = -1;
	}
}

CVideoSettings::~CVideoSettings() {
	mp_Dialog->setSDLSurface(g_pVideoDriver->FGLayerSurface);
	delete mp_Dialog;
}
