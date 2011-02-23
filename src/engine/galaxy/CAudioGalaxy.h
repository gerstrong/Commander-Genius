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
52,52,
28,7,
23,20,
15,
7,
4,
21,
25,
11,
12,
13,
14,
15,
17,
17,
18,
19,
20,
21,
18,
23,
24,
25,
26,
27,
28,
8,

12,
31,
32,
33,
34,

35,
36,
37,
38,

14,
45,
26,
9,
10,
19,
31,
34,
35,
36,
43,
44,

22,
24,
30,
5,
32,
33,
39,
40,
41,
42,
46,

47,
48,
49,
50,
51
};

class CAudioGalaxy : public CAudioResources {
public:
	CAudioGalaxy(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec);

	bool readPCSpeakerSoundintoWaveForm(CSoundSlot &soundslot, const byte *pcsdata, const unsigned int bytesize, const Uint8 formatsize );

	bool LoadFromAudioCK(const CExeFile& ExeFile);
	bool loadSoundData();
	void unloadSound();

private:
	const CExeFile &m_ExeFile;
};

#endif /* CAUDIOGALAXY_H_ */
