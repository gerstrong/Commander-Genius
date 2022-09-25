/*
 * CAudioVorticon.h
 *
 *  Created on: 22.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIOVORTICON_H_
#define CAUDIOVORTICON_H_

#include <base/audio/CAudioResources.h>
#include <map>

enum class GameSound;

class CAudioVorticon : public CAudioResources
{
public:

    bool loadSoundData(const unsigned int dictOffset);
	void unloadSound();
	
    std::map<int, int> sndSlotMap;

private:
	void setupAudioMap();
    
    void loadSoundStream(Uint8* exedata);

	template <typename T>
    bool loadPCSpeakerSound(std::vector<T> &waveform,
                            const std::string& searchname,
                            bool IsSigned,
                            Uint16& Priority);

    bool loadSound(const std::string& path,
                   const std::string& searchname,
                   const GameSound loadnum);

    std::vector<gs_byte> mFileBuffer;
};

#endif /* CAUDIOVORTICON_H_ */
