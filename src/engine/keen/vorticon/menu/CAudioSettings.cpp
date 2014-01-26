/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

/*
#include "CAudioSettings.h"

#include "widgets/Button.h"
#include "common/CSettings.h"
#include "sdl/music/CMusic.h"
#include "sdl/sound/CSound.h"

#include <base/utils/StringUtils.h>


namespace vorticon
{

CAudioSettings::CAudioSettings() :
VorticonMenu(GsRect<float>(0.075f, 0.24f, 0.85f, 0.4f) )
{
*/
   /* mpRate = new ComboSelection( "Rate", g_pSound->getAvailableRateList());
	mpMenuDialog->addControl( mpRate );

    mpStereo = new Switch( "Stereo" );
	mpMenuDialog->addControl( mpStereo );

    mpDepth = new ComboSelection( "Depth", filledStrList( 2, "8-bit", "16-bit" ) );
	mpMenuDialog->addControl( mpDepth );

    mpSBToggle = new ComboSelection( "Card", filledStrList( 2, "PC Speaker", "Soundblaster" ) );
	mpMenuDialog->addControl( mpSBToggle );

    mpSoundVolume = new NumberControl( "Sound Vol", 0, SDL_MIX_MAXVOLUME, 8, g_pSound->getSoundVolume() );
	mpMenuDialog->addControl( mpSoundVolume );


    mpMusicVolume = new NumberControl( "Music Vol", 0, SDL_MIX_MAXVOLUME, 8, g_pSound->getMusicVolume() );
    mpMenuDialog->addControl( mpMusicVolume );*/

/*	setMenuLabel("SNDEFFMENULABEL");

}


void CAudioSettings::init()
{*/
    /*mAudioSpec = g_pSound->getAudioSpec();
	mSoundblaster = g_pSound->getSoundBlasterMode();
	mSoundVolume = mpSoundVolume->getSelection();

    mpRate->setSelection( itoa(mAudioSpec.freq) );
	mpStereo->enable( mAudioSpec.channels == 2 );
	mpDepth->setSelection( mAudioSpec.format == AUDIO_U8 ? "8-bit" : "16-bit" );
	mpSBToggle->setSelection( mSoundblaster ? "Soundblaster" : "PC Speaker" );
    g_pMusicPlayer->play();*/
/*}


void CAudioSettings::ponder(const float deltaT)
{*/
    /*CBaseMenu::ponder(0);

	if( mSoundVolume != mpSoundVolume->getSelection() )
		g_pSound->playSound(SOUND_GET_ITEM);

	mSoundVolume = mpSoundVolume->getSelection();

	g_pSound->setSoundVolume( mSoundVolume );
    g_pSound->setMusicVolume( mpMusicVolume->getSelection() );*/
/*}


void CAudioSettings::release()
{*/
    /*mAudioSpec.freq = atoi( mpRate->getSelection().c_str() );
	mAudioSpec.channels = mpStereo->isEnabled() ? 2 : 1;
	mAudioSpec.format = mpDepth->getSelection() == "8-bit" ? AUDIO_U8 : AUDIO_S16;

	mSoundblaster = ( mpSBToggle->getSelection() == "Soundblaster" ? true : false );

	g_pSound->unloadSoundData();
	g_pSound->destroy();
	g_pSound->setSettings(mAudioSpec, mSoundblaster);
	g_pSound->init();
	g_pSound->loadSoundData();
	g_pMusicPlayer->reload();

    g_pSettings->saveDrvCfg();*/
/*}

}*/
