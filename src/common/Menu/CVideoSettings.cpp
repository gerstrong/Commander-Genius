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
m_restartVideo(restartVideo),
mp_CameraSettings(NULL)
{
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

void CVideoSettings::processSpecific(){

	if(!mp_CameraSettings)
	{
	std::string buf;
	
	if( m_mustclose )
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
			m_restartVideo = true;
			m_mustclose = true;
		}
	}

	if( m_selection != NO_SELECTION)
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

CVideoSettings::~CVideoSettings() {
	delete mp_Dialog;
}
