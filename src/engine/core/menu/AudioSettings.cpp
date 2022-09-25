/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "AudioSettings.h"
#include <base/utils/StringUtils.h>

//#include "engine/core/CBehaviorEngine.h"
#include "engine/core/CSettings.h"
#include <base/audio/music/CMusic.h>
#include <base/audio/Audio.h>


CAudioSettings::CAudioSettings(const Style &style) :
GameMenu(GsRect<float>(0.075f, 0.24f, 0.85f, 0.4f), style )
{

#if !defined(EMBEDDED)

    mpRate =
        mpMenuDialog->add(
                new ComboSelection( "Rate", gAudio.getAvailableRateList(), style) );

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
                                    filledStrList( 2, "PC Speaker", "SndBlaster" ),
                                    style ) );

    mpSoundVolume =
        mpMenuDialog->add( new NumberControl( "Sound Vol", 0, SDL_MIX_MAXVOLUME, 4, false,
                                              gAudio.getSoundVolume(), style ) );


    mpMusicVolume =
        mpMenuDialog->add( new NumberControl( "Music Vol", 0, SDL_MIX_MAXVOLUME, 4, false,
                                              gAudio.getMusicVolume(), style ) );

    mpOPLAmp =
        mpMenuDialog->add( new NumberControl( "OPL Amp", 100, 400, 10, false,
                                              gAudio.getOplAmp(), style ) );

    mpPCSpeakerAmp =
        mpMenuDialog->add( new NumberControl( "PC Spk Amp", 0, 100, 10, false,
                                              gAudio.getPCSpeakerVol(), style ) );

    mpBufferAmp =
        mpMenuDialog->add( new NumberControl( "Buffer Amp", 1, 10, 1, false,
                                              gAudio.getBufferAmp(), style ) );


	setMenuLabel("SNDEFFMENULABEL");

    mpMenuDialog->fit();
    select(1);
}


void CAudioSettings::refresh()
{
    mAudioSpec       = gAudio.getAudioSpec();
    mSoundblaster    = gAudio.getSoundBlasterMode();
    mSoundVolume     = mpSoundVolume->getSelection();
    mMusicVolume     = mpMusicVolume->getSelection();
    mOplBoost        = mpOPLAmp->getSelection();
    mPcSpeakerVolume = mpPCSpeakerAmp->getSelection();

    mpSoundVolume->setSelection(gAudio.getSoundVolume());
    mpMusicVolume->setSelection(gAudio.getMusicVolume());
    mpOPLAmp->setSelection(gAudio.getOplAmp());
    mpPCSpeakerAmp->setSelection(gAudio.getPCSpeakerVol());
    mpBufferAmp->setSelection(gAudio.getBufferAmp());

#if !defined(EMBEDDED)
	mpRate->setSelection( itoa(mAudioSpec.freq) );
#endif
	mpStereo->enable( mAudioSpec.channels == 2 );

#if !defined(EMBEDDED)
	mpDepth->setSelection( mAudioSpec.format == AUDIO_U8 ? "8-bit" : "16-bit" );
#endif
    mpSBToggle->setSelection( mSoundblaster ? "SndBlaster" : "PC Speaker" );
    gMusicPlayer.play();
}


void CAudioSettings::ponder(const float deltaT)
{
    GameMenu::ponder(0);

	if( mSoundVolume != mpSoundVolume->getSelection() )
    {
        gAudio.playSound(12);
    }

    mSoundVolume     = mpSoundVolume->getSelection();
    mMusicVolume     = mpMusicVolume->getSelection();
    mOplBoost        = mpOPLAmp->getSelection();
    mPcSpeakerVolume = mpPCSpeakerAmp->getSelection();
    mBufferAmp       = mpBufferAmp->getSelection();

    gAudio.setSoundVolume( mSoundVolume );
    gAudio.setMusicVolume( mMusicVolume );
    gAudio.setOplAmp( mOplBoost );
    gAudio.setPcSpeakerVol( mPcSpeakerVolume );
    gAudio.setBufferAmp( mBufferAmp );
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

    mSoundblaster = ( mpSBToggle->getSelection() == "SndBlaster" ? true : false );

	gAudio.setSettings(mAudioSpec, mSoundblaster);
	gAudio.init();

	gSettings.saveDrvCfg();
}

