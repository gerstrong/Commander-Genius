/*
 * CMusic.cpp
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#include "CMusic.h"
#include "../hqp/hq_sound.h"
#include "../CLogFile.h"
#include "../include/vorbis/oggsupport.h"
#include "../FindFile.h"

CMusic::CMusic() {
	playmode = PLAY_MODE_STOP;
	music_buffer = NULL;
	music_pos = 0;
	music_len = 0;
}

CMusic::~CMusic() {
	unload();
}

int CMusic::load(SDL_AudioSpec AudioSpec, char *musicfile)
{
	if(AudioSpec.format != 0)
	{

#ifdef BUILD_WITH_OGG
	SDL_AudioSpec AudioFileSpec;
	SDL_AudioCVT  Audio_cvt;

	stHQSound	pOggAudio;

	pOggAudio.sound_buffer=NULL;
	pOggAudio.sound_len=0;
	pOggAudio.sound_pos=0;

	FILE *fp;
	if((fp = OpenGameFile(musicfile,"rb")) == NULL)
	{
		g_pLogFile->textOut(PURPLE,"Music Driver(): \"%s\". File does not exist!<br>", musicfile);
		return -1;
	}

	if(openOGGSound(fp, &AudioFileSpec, AudioSpec.format, &pOggAudio) != 0)
	{
		g_pLogFile->textOut(PURPLE,"Music Driver(): OGG file could not be opened: \"%s\". File is damaged or something is wrong with your soundcard!<br>", musicfile);
		return 1;
	}

	g_pLogFile->ftextOut("Music Driver(): File \"%s\" opened successfully!<br>", musicfile);

    int ret;

   	ret = SDL_BuildAudioCVT(&Audio_cvt,
							  AudioFileSpec.format, AudioFileSpec.channels, AudioFileSpec.freq,
	                          AudioSpec.format, AudioSpec.channels, AudioSpec.freq);

    music_pos = 0;

		if(ret == -1){
			g_pLogFile->textOut(PURPLE,"Music Driver(): Couldn't convert the audio for an appropriate format!<br>");
		   free(pOggAudio.sound_buffer);
		   pOggAudio.sound_len = 0;
		   pOggAudio.sound_pos = 0;
		   return 1;
		}

		music_len = pOggAudio.sound_len;
		Audio_cvt.buf = (Uint8*) malloc(music_len * (Audio_cvt.len_mult) * sizeof(Uint8));
		Audio_cvt.len = music_len;
		memcpy(Audio_cvt.buf, pOggAudio.sound_buffer, music_len*sizeof(Uint8));

		free(pOggAudio.sound_buffer);

		SDL_ConvertAudio(&Audio_cvt);

		music_len = Audio_cvt.len_cvt;
		music_buffer = new Uint8 [music_len];
		memcpy(music_buffer, Audio_cvt.buf, music_len);

		// Structure Audio_cvt must be freed!
		free(Audio_cvt.buf);

		return 0;

#endif
	}
	else
		g_pLogFile->textOut(PURPLE,"Music Driver(): I would like to open the music for you. But your Soundcard is disabled!!<br>");

	return 0;
}

void CMusic::unload(void)
{
	if(music_buffer){ delete[] music_buffer; music_buffer = NULL; }

	music_len = 0;
	music_pos = 0;
	playmode = PLAY_MODE_STOP;
}

void CMusic::play(void)
{
	if(music_buffer)
		playmode = PLAY_MODE_PLAY;
}

void CMusic::stop(void)
{
	playmode = PLAY_MODE_STOP;
}

Uint8 *CMusic::passBuffer(int length) // length only refers to the part(buffer) that has to be played
{
	if(!music_buffer)
		return NULL;
	
	if(length < music_len - music_pos)
	{
		music_pos += length;
		return music_buffer + music_pos - length;
	}
	else
	{
		music_pos = 0;
		return music_buffer;
	}
}
