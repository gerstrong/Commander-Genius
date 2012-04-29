/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "common/CSettings.h"
#include "CAudioSettings.h"
#include "StringUtils.h"

#include "gui/CGUIButton.h"

#include "core/CGameControl.h"

CAudioSettings::CAudioSettings() :
CBaseMenu(CRect<float>(0.1f, 0.24f, 0.8f, 0.4f) )
{

	mpRate = new CGUIComboSelection( "rate",
									 g_pSound->getAvailableRateList());
	mpMenuDialog->addControl( mpRate );

	mpStereo = new CGUISwitch( "Stereo" );
	mpMenuDialog->addControl( mpStereo );

	mpDepth = new CGUIComboSelection( "Depth",
									  filledStrList( 2, "8-bit", "16-bit" ) );
	mpMenuDialog->addControl( mpDepth );

	mpSBToggle = new CGUIComboSelection( "Soundcard",
										 filledStrList( 2, "pc speaker", "soundblaster" ) );
	mpMenuDialog->addControl( mpSBToggle );

	mpSoundVolume = new CGUINumberControl( "Sound Volume",
											0, SDL_MIX_MAXVOLUME, 8,
											g_pSound->getSoundVolume() );
	mpMenuDialog->addControl( mpSoundVolume );


	mpMusicVolume = new CGUINumberControl( "Music Volume",
											0, SDL_MIX_MAXVOLUME, 8,
				 	 	 	 	 	 	 	g_pSound->getMusicVolume() );
	mpMenuDialog->addControl( mpMusicVolume );

	setMenuLabel("SNDEFFMENULABEL");

}


void CAudioSettings::init()
{
	mAudioSpec = g_pSound->getAudioSpec();
	mSoundblaster = g_pSound->getSoundBlasterMode();

	mpRate->setSelection( itoa(mAudioSpec.freq) );
	mpStereo->enable( mAudioSpec.channels == 2 );
	mpDepth->setSelection( mAudioSpec.format == AUDIO_S8 ? "8-bit" : "16-bit" );
	mpSBToggle->setSelection( mSoundblaster ? "soundblaster" : "pc speaker" );
}


void CAudioSettings::process()
{
	CBaseMenu::process();

	g_pSound->setSoundVolume( mpSoundVolume->getSelection() );
	g_pSound->setMusicVolume( mpMusicVolume->getSelection() );
}


void CAudioSettings::release()
{
	mAudioSpec.freq = atoi( mpRate->getSelection().c_str() );
	mAudioSpec.channels = mpStereo->isEnabled() ? 2 : 1;
	mAudioSpec.format = mpDepth->getSelection() == "8-bit" ? AUDIO_S8 : AUDIO_S16;

	mSoundblaster = ( mpSBToggle->getSelection() == "soundblaster" ? true : false );

	g_pSound->setSettings(mAudioSpec, mSoundblaster);

	g_pSettings->saveDrvCfg();
}
