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

// Map for the vorticon sound slots
const unsigned char SndSlotMapVort[]=
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

class CAudioVorticon : public CAudioResources {
public:
	CAudioVorticon(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec);

	bool loadSoundData();
	void unloadSound();

private:
	Uint8* loadSoundStream(Uint32 &buffer_size, Uint8* exedata);

	template <typename T>
	bool loadPCSpeakerSound(Uint8 *buffer, const Uint32 buf_size,
			std::vector<T> &waveform, const std::string& searchname,
			bool IsSigned, Uint16& Priority);

	bool loadSound(Uint8 *buffer, const Uint32 buf_size, const std::string& path, const std::string& searchname, unsigned int loadnum);

	const CExeFile &m_ExeFile;
};

#endif /* CAUDIOVORTICON_H_ */
