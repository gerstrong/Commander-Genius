/*
 * COGGPlayer.h
 *
 *  Created on: 17.02.2011
 *      Author: gerstrong
 */

#ifndef COGGPLAYER_H_
#define COGGPLAYER_H_

#if defined(OGG) || defined(TREMOR)

#include <cstdio>
#include <iostream>

#include "CMusicPlayer.h"

#ifdef OGG
#define OV_EXCLUDE_STATIC_CALLBACKS // Will reduce some nasty warning, since we don't use those callbacks
#include <vorbisfile.h>
#elif defined  TREMOR
#include <ivorbisfile.h>
#endif

#include <SDL.h>
#include <string>
#include <fileio/CExeFile.h>


class COGGPlayer : public CMusicPlayer
{
public:
    COGGPlayer();
    COGGPlayer(const std::string& filename);
	virtual ~COGGPlayer();

    bool loadMusicTrack(const int track);
    bool loadMusicFromFile(const std::string& filename);
	bool open();
	void readBuffer(Uint8* buffer, Uint32 length);
	void close();

private:
	bool readOGGStream( OggVorbis_File  &oggStream, char *buffer, const size_t &size, const SDL_AudioSpec &OGGAudioSpec );
    bool readOGGStreamAndResample(OggVorbis_File  &oggStream, Uint8 *buffer, const int output_size, const size_t input_size, const SDL_AudioSpec &OGGAudioSpec );

	OggVorbis_File  m_oggStream;
	std::string m_filename;
	SDL_AudioSpec m_AudioFileSpec;
	SDL_AudioCVT m_Audio_cvt;
	Uint32 m_pcm_size;
	Uint32 m_music_pos;
	int m_bitStream;
    vorbis_info*    mVorbisInfo;    // some formatting data
    bool mHasCommonFreqBase;

    std::vector<Uint8> mResampleBuf;
};

#if defined(TREMOR)
int ov_fopen(char *path,OggVorbis_File *vf);
#endif

#endif

#endif /* COGGPLAYER_H_ */
