/**
 * Audio.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 *
 * This slass describes the Audio Driver we use in CG.
 * It manages the Soundslots, Music, etc. everything audio related to Commander Genius
 */

#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "CSingleton.h"

#define g_pSound Audio::Get()

#include <SDL.h>
#include <map>
#include <vector>
#include <list>
#include <memory>

#include "sound/CSoundChannel.h"
#include "CAudioResources.h"

class Audio : public CSingleton<Audio>
{
public:
    Audio();
    ~Audio();

	bool init();
	void stop();

	void stopAllSounds();
    bool forcedisPlaying();
	void callback(void *unused, Uint8 *stream, int len);
	void pauseSound(void);
	void resumeSounds(void);
	void playSound(	const GameSound snd,
					const SoundPlayMode mode = PLAY_NOW );
    void playStereofromCoord(const GameSound snd,
                             const SoundPlayMode mode,
                             const int xCoord);
	void playStereosound(const GameSound snd, const char mode, const short balance);
	void playStereosoundSlot(unsigned char slotplay, const char mode, const short balance);
	bool isPlaying(const GameSound snd);
	void stopSound(const GameSound snd);
	void destroy();

	void setSoundVolume(const Uint8 volume) { m_SoundVolume = volume; }
	void setMusicVolume(const Uint8 volume) { m_MusicVolume = volume; }
	Uint8 getSoundVolume() { return m_SoundVolume; }
	Uint8 getMusicVolume() { return m_MusicVolume; }


	const SDL_AudioSpec	&getAudioSpec() const  { return const_cast<const SDL_AudioSpec&>(mAudioSpec); }
    bool getSoundBlasterMode() {	return mUseSoundBlaster;	}
    COPLEmulator &getOPLEmulatorRef() { return m_OPL_Player; }

	void setSettings( const int rate,
							  const int channels,
							  const int format,
		 	  	  	  	  	  const bool useSB );

	void setSettings( const SDL_AudioSpec& audioSpec,
			 	 	  const bool useSB );


    /**
     * @brief updateFuncPtrs Depending on the audio setup it will update the mixAudio function pointer.
     */
    void updateFuncPtrs();

	bool loadSoundData();

    void setupSoundData(const std::map<GameSound, int> &slotMap,
                               CAudioResources *audioResPtr);

	void unloadSoundData();

	// Tell whether a sound is played which has to stop the gameplay
	bool pauseGamePlay();

	// Returns as list of strings the frequencies
	std::list<std::string> getAvailableRateList() const;

    void addSubCallback( void (SDLCALL *callback)(void *, Uint8 *, int) )
    {
        mSubCallbackVec.push_back(callback);
    }

protected:
	bool m_callback_running;
	SDL_AudioSpec mAudioSpec;

    std::vector< void (SDLCALL *)(void *, Uint8 *, int) > mSubCallbackVec;

private:

    // Channels of sound which can be played at the same time.
    std::vector<CSoundChannel>	mSndChnlVec;
    std::unique_ptr<CAudioResources> mpAudioRessources;
	Uint8 m_MusicVolume;
	Uint8 m_SoundVolume;

    // Boolean that holds if the Soundblaster is to be used. If set false it will use the PC Speaker emulation
    bool mUseSoundBlaster;

	std::vector<Uint8> m_MixedForm;	// Mainly used by the callback function. Declared once and allocated
                                    // for the whole runtime

    std::map<GameSound, int> sndSlotMap;

    COPLEmulator m_OPL_Player;
    bool m_pause_gameplay;
};

#endif /* __AUDIO_H__ */
