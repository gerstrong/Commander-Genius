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
	void playSound(GameSound snd, char mode=PLAY_NOW);
	void playStereofromCoord(GameSound snd, char mode, unsigned int xcoordinate);
	void playStereosound(GameSound snd, char mode, short balance);
	bool isPlaying(const GameSound snd);
	void stopSound(const GameSound snd);
	void destroy(void);

	void setSoundVolume(const Uint8 volume) { m_SoundVolume = volume; }
	void setMusicVolume(const Uint8 volume) { m_MusicVolume = volume; }
	Uint8 getSoundVolume() { return m_SoundVolume; }
	Uint8 getMusicVolume() { return m_MusicVolume; }

	SDL_AudioSpec	getAudioSpec() const  { return AudioSpec; }
	unsigned short 	getMixingchannels() const { return m_mixing_channels; }
	bool getSoundBlasterMode() {	return m_sound_blaster_mode;	}
	COPLEmulator *getOPLEmulatorPtr() { return &m_OPL_Player; }

	void setSoundmode(int freq, bool stereo, Uint16 format);
	void setSoundBlasterMode(const bool value) {	m_sound_blaster_mode = value;	}

	bool loadSoundData(const CExeFile &ExeFile);
	void unloadSoundData();

protected:
	std::vector<CSoundChannel>	m_soundchannel;
	SmartPointer<CAudioResources> m_pAudioRessources;
	bool m_callback_running;
	SDL_AudioSpec AudioSpec;

private:
	unsigned short m_mixing_channels;
	Uint8 m_MusicVolume;
	Uint8 m_SoundVolume;
	bool m_sound_blaster_mode;

	SmartPointer<Uint8> m_pMixedForm;	// Mainly used by the callback function. Declared once and allocated
    									// for the whole program

    unsigned char *mp_SndSlotMap;

    COPLEmulator m_OPL_Player;
};

#endif /* CSOUND_H_ */
