/*
 * CAudioVorticon.h
 *
 *  Created on: 22.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIOVORTICON_H_
#define CAUDIOVORTICON_H_

#include "common/CAudioResources.h"
#include "fileio/CExeFile.h"

class CAudioVorticon : public CAudioResources {
public:
	CAudioVorticon(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec);

	bool loadSoundData();
	void unloadSound();

private:
	Uint8* loadSoundStream(Uint32 &buffer_size, Uint8* exedata);

	template <typename T>
	void generateWave(std::vector<T> &waveform, word sample, T &wave, Uint64 &freqtimer, bool IsSigned, const int& AMP);

	template <typename T>
	bool loadPCSpeakerSound(Uint8 *buffer, const Uint32 buf_size, std::vector<T> &waveform, const std::string& searchname, bool IsSigned);

	bool loadSound(Uint8 *buffer, const Uint32 buf_size, const std::string& path, const std::string& searchname, unsigned int loadnum);

	const CExeFile &m_ExeFile;
};

#endif /* CAUDIOVORTICON_H_ */
