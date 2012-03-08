/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CAudioSettings.h"
#include "StringUtils.h"

#include "gui/CGUIButton.h"

#include "core/CGameControl.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CAudioSettings::CAudioSettings(Uint8 dlg_theme) :
CBaseMenu(dlg_theme, CRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{

	mpMenuDialog->setBackground( CGUIDialog::VORTICON );

	/*mpRate = new CGUIComboSelection( "Rate",
									 ...,
									 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpRate );

	mpModeSelection = new CGUIComboSelection( "Mode",
									 ...,
									 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpModeSelection );

	mpDepth = new CGUIComboSelection( "Depth",
									 ...,
									 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpDepth );

	mpSBToggle = new CGUIComboSelection( "Soundcard",
									 ...,
									 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpSBToggle );

	mpSoundVolume = new CGUINumberControl( "Sound Volume",
			 	 	 	 	 	 	 	 	 ...,
			 	 	 	 	 	 	 	 	 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpSoundVolume );


	mpMusicVolume = new CGUINumberControl( "Music Volume",
	 	 	 	 ...,
	 	 	 	 CGUIComboSelection::VORTICON );
	mpMenuDialog->addControl( mpMusicVolume );*/

}


void CAudioSettings::init()
{
	mAudioSpec = g_pSound->getAudioSpec();
	mSoundblaster = g_pSound->getSoundBlasterMode();


}


void CAudioSettings::release()
{

}
