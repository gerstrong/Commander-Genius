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

	Uint8* loadSoundStream(Uint32 &buffer_size, Uint8* exedata);

private:
	const CExeFile &m_ExeFile;
};

#endif /* CAUDIOVORTICON_H_ */
