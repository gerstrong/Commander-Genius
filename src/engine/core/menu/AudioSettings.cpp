/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "AudioSettings.h"
#include <base/utils/StringUtils.h>

#include "engine/core/CBehaviorEngine.h"
#include "engine/core/CSettings.h"
#include "sdl/audio/music/CMusic.h"
#include "sdl/audio/Audio.h"


CAudioSettings::CAudioSettings(const Style &style) :
GameMenu(GsRect<float>(0.075f, 0.24f, 0.85f, 0.4f), style )
{

#if !defined(EMBEDDED)

    mpRate =
        mpMenuDialog->add(
                new ComboSelection( "Rate", gSound.getAvailableRateList(), style) );

#endif

    mpStereo =
        mpMenuDialog->add( new Switch( "Stereo", style ) );


#if !defined(EMBEDDED)

    mpDepth =
        mpMenuDialog->add(
                new ComboSelection( "Depth",
                                    filledStrList( 2, "8-bit", "16-bit" ),
                                    style ) );

#endif

    mpSBToggle =
        mpMenuDialog->add(
                new ComboSelection( "Card",
                                    filledStrList( 2, "PC Speaker", "Soundblaster" ),
                                    style ) );

    mpSoundVolume =
        mpMenuDialog->add( new NumberControl( "Sound Vol", 0, 100, 4, false,
                                              gSound.getSoundVolume(), style ) );


    mpMusicVolume =
        mpMenuDialog->add( new NumberControl( "Music Vol", 0, 100, 4, false,
                                              gSound.getMusicVolume(), style ) );

	setMenuLabel("SNDEFFMENULABEL");

    mpMenuDialog->fit();
}


void CAudioSettings::refresh()
{
	mAudioSpec = gSound.getAudioSpec();
	mSoundblaster = gSound.getSoundBlasterMode();
	mSoundVolume = mpSoundVolume->getSelection();
    mMusicVolume = mpMusicVolume->getSelection();

    mpSoundVolume->setSelection(gSound.getSoundVolume());
    mpMusicVolume->setSelection(gSound.getMusicVolume());        

#if !defined(EMBEDDED)
	mpRate->setSelection( itoa(mAudioSpec.freq) );
#endif
	mpStereo->enable( mAudioSpec.channels == 2 );

#if !defined(EMBEDDED)
	mpDepth->setSelection( mAudioSpec.format == AUDIO_U8 ? "8-bit" : "16-bit" );
#endif
	mpSBToggle->setSelection( mSoundblaster ? "Soundblaster" : "PC Speaker" );
    gMusicPlayer.play();
}


void CAudioSettings::ponder(const float deltaT)
{
    GameMenu::ponder(0);

	if( mSoundVolume != mpSoundVolume->getSelection() )
    {
		gSound.playSound(SOUND_GET_ITEM);
    }

	mSoundVolume = mpSoundVolume->getSelection();
    mMusicVolume = mpMusicVolume->getSelection();

    gSound.setSoundVolume( mSoundVolume );
    gSound.setMusicVolume( mMusicVolume );
}


void CAudioSettings::release()
{
#if !defined(EMBEDDED)
	mAudioSpec.freq = atoi( mpRate->getSelection().c_str() );
#endif

	mAudioSpec.channels = mpStereo->isEnabled() ? 2 : 1;

#if !defined(EMBEDDED)
	mAudioSpec.format = mpDepth->getSelection() == "8-bit" ? AUDIO_U8 : AUDIO_S16;
#endif

	mSoundblaster = ( mpSBToggle->getSelection() == "Soundblaster" ? true : false );

	gSound.setSettings(mAudioSpec, mSoundblaster);
	gSound.init();

	gSettings.saveDrvCfg();
}

