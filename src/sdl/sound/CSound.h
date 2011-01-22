/*
 * CSound.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
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
#include "CSoundSlot.h"
#include "sounds.h"
#include "fileio/CExeFile.h"

class CSound : public CSingleton<CSound>
{
public:
	CSound();

	bool init(void);
	void stop(void);

	void stopAllSounds();
	bool forcedisPlaying(void);
	char sound_load_all(const std::string& path);
	void transform_into_logaritmic_sound(int *pcmstream, int len);
	void callback(void *unused, Uint8 *stream, int len);
	void pauseSound(void);
	void resumeSounds(void);
	void playSound(GameSound snd, char mode);
	void playStereofromCoord(GameSound snd, char mode, unsigned int xcoordinate);
	void playStereosound(GameSound snd, char mode, short balance);
	bool isPlaying(GameSound snd);
	void stopSound(GameSound snd);
	void destroy(void);

	void setSoundVolume(Uint8 volume) { m_SoundVolume = volume; }
	void setMusicVolume(Uint8 volume) { m_MusicVolume = volume; }
	Uint8 getSoundVolume() { return m_SoundVolume; }
	Uint8 getMusicVolume() { return m_MusicVolume; }

	SDL_AudioSpec	getAudioSpec(void) { return AudioSpec; }
	unsigned short 	getMixingchannels(void) { return m_mixing_channels; }

	void setSoundmode(int freq, bool stereo, Uint16 format);

	void setGameData(CExeFile &ExeFile);
	virtual bool loadSoundData(CExeFile &ExeFile);

	char extractOfExeFile(CExeFile &ExeFile);

protected:
	std::vector<CSoundChannel>	m_soundchannel;
	std::map<int, CSoundSlot>	m_soundslot;

	static const int MAX_SOUNDS = 50;

	SDL_AudioSpec AudioSpec;

	bool m_active;
    unsigned short m_Episode;
    std::string m_DataDirectory;

private:
	unsigned short m_mixing_channels;
	Uint8 m_MusicVolume;
	Uint8 m_SoundVolume;

    Uint8 *m_pMixedForm;			// Mainly used by the callback function. Declared once and allocated

};

#endif /* CSOUND_H_ */
