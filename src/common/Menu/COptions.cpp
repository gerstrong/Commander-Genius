/*
 * COptions.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "COptions.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "common/CSettings.h"


COptions::COptions( const Uint8 dlg_theme ) :
CBaseMenu( dlg_theme, CRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{

	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	/*mpMenuDialog->addControl(new CGUIButton( "Video",
									new OpenMenuEvent( new CVideoSettings(dlgtheme) ),
									CGUIButton::VORTICON ),
								CRect<float>(0.05f, 0.08f, 0.9f, 0.07f) );*/


	/*int i;
	std::string buf;

	mp_Dialog = new CDialog(27, NUM_OPTIONS+2, INPUT_MODE_UP_DOWN, dlg_theme);
	
	for( i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i+1, "");
		buf = mp_option[i].menuname + " " + getSwitchString(mp_option[i].value);
		mp_Dialog->m_dlgobject.at(i)->m_Option->m_FontMapID = 1;
		mp_Dialog->setObjectText(i, buf);
	}*/
}

//void COptions::processSpecific()
//{
	/*std::string buf;

	if(m_mustclose) // If menu is about to close save the options
		g_pSettings->saveGameCfg();

	if( m_selection != NO_SELECTION)
	{
		//
		mp_option[m_selection].value = !mp_option[m_selection].value;
		buf = mp_option[m_selection].menuname + " " + getSwitchString(mp_option[m_selection].value);

		mp_Dialog->setObjectText(m_selection, buf);
		m_selection = NO_SELECTION;
	}*/
//}
