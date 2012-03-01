/*
 * CVideoSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "sdl/input/CInput.h"
#include "sdl/CTimer.h"
#include "CVideoSettings.h"
#include "common/CSettings.h"
#include "StringUtils.h"
#include "Utils.h"


CVideoSettings::CVideoSettings(const Uint8 dlg_theme) :
CBaseMenu(dlg_theme, CRect<float>(0.15f, 0.24f, 0.7f, 0.5f) )
{
	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	// Create the fps config selection control
	std::list<std::string>	List;
	for( int i = 10 ; i <= 120 ; i += 10 )
		List.push_back( itoa (i) );

	mpFPSSelection = new CGUIComboSelection( "FPS",
	 	 	 	 	 	 	 	 	 	 	 List,
	 	 	 	 	 	 	 	 	 	 	 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpFPSSelection, CRect<float>(0.05f, 0.08f, 0.9f, 0.07f) );


	mpOGLFilterSelection = new CGUIComboSelection( "OGL Filter",
											filledStrList( 2, "NEAREST", "LINEAR" ),
	 	 	 	 	 	 	 	 	 	 	 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpOGLFilterSelection, CRect<float>(0.05f, 0.15f, 0.9f, 0.07f) );



	/*
OGLfilter = 1
OpenGL = true
autoframeskip = 60
filter = 2
fullscreen = false
height = 1000
scale = 1
showfps = 0
specialfx = true
width = 1000
	 * */


	/*m_current = -1;
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

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, "Adjust Camera Bounds");*/
}

void CVideoSettings::init()
{
	// Load the config into the GUI
	mpFPSSelection->setSelection( itoa( g_pTimer->getFrameRate() ) );
}


void CVideoSettings::release()
{
	// Save up the changed stuff
	g_pTimer->setFPS( atoi(mpFPSSelection->getSelection().c_str() ));

	// TODO: At this point we also must save the settings
}
