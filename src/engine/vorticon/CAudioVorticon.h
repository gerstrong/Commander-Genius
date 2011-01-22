/*
 * CAudioVorticon.h
 *
 *  Created on: 22.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIOVORTICON_H_
#define CAUDIOVORTICON_H_

#include "sdl/sound/CSound.h"

class CAudioVorticon : public CSound {
public:

	CAudioVorticon() {};

	void setGameData(CExeFile &ExeFile);
	bool loadSoundData(CExeFile &ExeFile);
	void unloadSound();

	Uint8* loadSoundStream(Uint32 &buffer_size, Uint8* exedata);
};

#endif /* CAUDIOVORTICON_H_ */
