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
	void setGameData(CExeFile &ExeFile);
	bool loadSoundData(CExeFile &ExeFile);
};

#endif /* CAUDIOVORTICON_H_ */
