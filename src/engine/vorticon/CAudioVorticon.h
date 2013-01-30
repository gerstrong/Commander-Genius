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
	
	std::map<GameSound, int> sndSlotMap;
    
private:
	void setupAudioMap();
    
	Uint8* loadSoundStream(Uint32 &buffer_size, Uint8* exedata);
    
	template <typename T>
	bool loadPCSpeakerSound(Uint8 *buffer, const Uint32 buf_size,
                            std::vector<T> &waveform, const std::string& searchname,
                            bool IsSigned, Uint16& Priority);
    
	bool loadSound(Uint8 *buffer, const Uint32 buf_size, const std::string& path, const std::string& searchname, unsigned int loadnum);
    
	const CExeFile &m_ExeFile;
};

#endif /* CAUDIOVORTICON_H_ */
