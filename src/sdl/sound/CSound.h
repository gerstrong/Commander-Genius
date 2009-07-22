/*
 * CSound.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUND_H_
#define CSOUND_H_

#include "../../CSingleton.h"
#define g_pSound CSound::Get()

#include <SDL.h>
#include "CSoundChannel.h"
#include "CSoundSlot.h"

class CSound : public CSingleton<CSound>
{
public:
	CSound();
	virtual ~CSound();

	bool init(void);
	void stop(void);
	char loadSoundData(unsigned short Episode, char *DataDirectory);
	void stopAllSounds(void);

	bool forcedisPlaying(void);
	char sound_load_all(const char *path);
	void transform_into_logaritmic_sound(int *pcmstream, int len);
	void callback(void *unused, Uint8 *stream, int len);
	void pauseSound(void);
	void resumeSounds(void);
	void playSound(int snd, char mode);
	void playStereofromCoord(int snd, char mode, unsigned int xcoordinate);
	void playStereosound(int snd, char mode, short balance);
	bool isPlaying(int snd);
	void stopSound(int snd);
	void destroy(void);

	SDL_AudioSpec	getAudioSpec(void) { return AudioSpec; }
	unsigned short 	getMixingchannels(void) { return m_mixing_channels; };

	void setSoundmode(int freq, bool stereo);

	char extractOfExeFile(char *inputpath, int episode); // This is a special funktion. It doesn't belong here!

private:
	CSoundChannel	*m_soundchannel;
	CSoundSlot		*m_soundslot;

	SDL_AudioSpec AudioSpec;

	bool m_active;
	unsigned short m_mixing_channels;

    Uint8 *m_MixedForm;			// Mainly used by the callback function. Declared once and allocated
};

#endif /* CSOUND_H_ */
