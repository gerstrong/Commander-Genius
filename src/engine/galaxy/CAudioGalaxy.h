/*
 * CAudioGalaxy.h
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIOGALAXY_H_
#define CAUDIOGALAXY_H_

#include "common/CAudioResources.h"
#include "fileio/CExeFile.h"

class CAudioGalaxy : public CAudioResources {
public:
	CAudioGalaxy(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec);

	bool LoadFromAudioCK(const CExeFile& ExeFile);
	bool loadSoundData();
	void unloadSound();

private:
	const CExeFile &m_ExeFile;
};

#endif /* CAUDIOGALAXY_H_ */
