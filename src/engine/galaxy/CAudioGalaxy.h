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

const unsigned char SndSlotMapGalaxy[]=
{
0, 1,
2, 3,
4, 5,
6,
7,
8,
9,
10,
11,
12,
13,
14,
15,
16,
17,
18,
19,
20,
21,
22,
23,
24,
25,
26,
27,
28,
29,

30,
31,
32,
33,
34,

35,
36,
37,
38
};

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
