/**
 * Audio.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 *
 * This slass describes the Audio Driver we use in CG.
 * It manages the Soundslots, Music, etc. everything audio related to Commander Genius
 */

#ifndef AUDIO_H
#define AUDIO_H

#include <base/Singleton.h>

#define gSound Audio::get()

#include <SDL.h>
#include <map>
#include <vector>
#include <list>
#include <memory>

#include "sound/CSoundChannel.h"
#include "CAudioResources.h"

class Audio : public GsSingleton<Audio>
{
public:
    Audio();
    ~Audio();

	bool init();
	void stop();

	void stopAllSounds();
    bool forcedisPlaying();
	void callback(void *unused, Uint8 *stream, int len);
	void pauseAudio(void);
	void resumeAudio(void);


    /**
     * @brief playSound If priorities allow, plays the sound "snd".
     * @param snd
     * @param mode
     * @return nonzero return value indicates a higher priority sound is playing.
     */
	void playSound(	const GameSound snd,
                    const SoundPlayMode mode = SoundPlayMode::PLAY_NOW );

    /**
     * @brief playStereofromCoord  Play one sound
     * @param snd       sound number to play
     * @param mode      how to play
     * @param xCoord    X Coordinate in relative to the listening object
     */
    void playStereofromCoord(const GameSound snd,
                             const SoundPlayMode mode,
                             const int xCoord);

    /**
     * @brief playStereosound Play with indication of balance
     * @param snd   number of sound to play
     * @param mode  How to play it
     * @param balance between -127(left) to 127(right)
     */
    void playStereosound(const GameSound snd,
                         const SoundPlayMode mode,
                         const short balance);

    /**
     * @brief playStereosoundSlot
     * @param slotplay
     * @param mode
     * @param balance
     */
    void playStereosoundSlot(const unsigned char slotplay,
                             const SoundPlayMode mode,
                             const short balance);

	bool isPlaying(const GameSound snd);
	void stopSound(const GameSound snd);
	void destroy();

    /**
     * @brief setSoundVolume    Set Sound effects volume
     * @param volume
     * @param updateMixer       if true the SDL Mixer gets the volume updated directly
     *                          This should not be done when the Audio Device has not been yet initialized
     */
    void setSoundVolume(const Uint8 volume,
                        const bool updateMixer = true);

    /**
     * @brief setMusicVolume
     * @param volume
     * @param updateMixer       if true the SDL Mixer gets the volume updated directly
     *                          This should not be done when the Audio Device has not been yet initialized
     */
    void setMusicVolume(const Uint8 volume,
                        const bool updateMixer = true);

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

protected:
    bool mCallbackRunning = false;
	SDL_AudioSpec mAudioSpec;

    std::vector< void (SDLCALL *)(void *, Uint8 *, int) > mSubCallbackVec;

private:

    // Channels of sound which can be played at the same time.

    std::vector<CSoundChannel>	mSndChnlVec;

    std::unique_ptr<CAudioResources> mpAudioRessources;

	Uint8 m_MusicVolume;
	Uint8 m_SoundVolume;

    // Boolean that holds if the Soundblaster is to be used. If set false it will use the PC Speaker emulation
    bool mUseSoundBlaster = false;

    std::map<GameSound, int> sndSlotMap;

    COPLEmulator m_OPL_Player;
    bool mPauseGameplay = false;
};

#endif /* __AUDIO_H__ */
