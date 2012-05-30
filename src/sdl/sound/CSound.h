/*
 * CSound.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 *
 *  This Class describes the Audio Driver we use in CG
 */

#ifndef CSOUND_H_
#define CSOUND_H_

#include "CSingleton.h"
#define g_pSound CSound::Get()

#include <string>
#include <SDL.h>
#include <map>
#include <vector>
#include <list>

#include "CSoundChannel.h"
#include "fileio/CExeFile.h"
#include "common/CAudioResources.h"
#include "SmartPointer.h"

class CSound : public CSingleton<CSound>
{
public:
	CSound();
	~CSound();

	bool init();
	void stop();

	void stopAllSounds();
	bool forcedisPlaying(void);
	char sound_load_all(const std::string& path);
	void callback(void *unused, Uint8 *stream, int len);
	void pauseSound(void);
	void resumeSounds(void);
	void playSound(	const GameSound snd,
					const SoundPlayMode mode = PLAY_NOW );
	void playStereofromCoord(const GameSound snd,
							 const SoundPlayMode mode,
							 const unsigned int xcoordinate);
	void playStereosound(const GameSound snd, const char mode, const short balance);
	bool isPlaying(const GameSound snd);
	void stopSound(const GameSound snd);
	void destroy();

	void setSoundVolume(const Uint8 volume) { m_SoundVolume = volume; }
	void setMusicVolume(const Uint8 volume) { m_MusicVolume = volume; }
	Uint8 getSoundVolume() { return m_SoundVolume; }
	Uint8 getMusicVolume() { return m_MusicVolume; }


	const SDL_AudioSpec	&getAudioSpec() const  { return const_cast<const SDL_AudioSpec&>(mAudioSpec); }
	unsigned short 	getMixingchannels() const { return m_mixing_channels; }
	bool getSoundBlasterMode() {	return m_sound_blaster_mode;	}
	COPLEmulator *getOPLEmulatorPtr() { return &m_OPL_Player; }

	void setSettings( const int rate,
							  const int channels,
							  const int format,
		 	  	  	  	  	  const bool useSB );

	void setSettings( const SDL_AudioSpec& audioSpec,
			 	 	  const bool useSB );

	bool loadSoundData();
	void unloadSoundData();

	// Tell whether a sound is played which has to stop the gameplay
	bool pauseGamePlay();

	// Returns as list of strings the frequencies
	std::list<std::string> getAvailableRateList() const;

protected:
	std::vector<CSoundChannel>	m_soundchannel;
	SmartPointer<CAudioResources> m_pAudioRessources;
	bool m_callback_running;
	SDL_AudioSpec mAudioSpec;

private:
	unsigned short m_mixing_channels;
	Uint8 m_MusicVolume;
	Uint8 m_SoundVolume;
	bool m_sound_blaster_mode;

	std::vector<Uint8> m_MixedForm;	// Mainly used by the callback function. Declared once and allocated
    									// for the whole program

    unsigned char *mp_SndSlotMap;

    COPLEmulator m_OPL_Player;
    bool m_pause_gameplay;
};

#endif /* CSOUND_H_ */
