/*
 * CAudioVorticon.h
 *
 *  Created on: 22.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIOVORTICON_H_
#define CAUDIOVORTICON_H_

#include "sdl/audio/CAudioResources.h"
#include "fileio/CExeFile.h"


class CAudioVorticon : public CAudioResources
{
public:

	bool loadSoundData();
	void unloadSound();
	
	std::map<GameSound, int> sndSlotMap;

private:
	void setupAudioMap();
    
    void loadSoundStream(Uint8* exedata);

	template <typename T>
    bool loadPCSpeakerSound(std::vector<T> &waveform, const std::string& searchname,
			bool IsSigned, Uint16& Priority);

    bool loadSound(const std::string& path, const std::string& searchname, unsigned int loadnum);

    std::vector<byte> mFileBuffer;
};

#endif /* CAUDIOVORTICON_H_ */
