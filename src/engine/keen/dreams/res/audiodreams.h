#ifndef AUDIODREAMS_H
#define AUDIODREAMS_H


#include <sdl/audio/CAudioResources.h>
#include "fileio/CExeFile.h"


class AudioDreams : public CAudioResources
{

public:


    /**
     * @brief loadSoundData     Load the dreams data file for sound effects
     * @return true if everything went fine, otherwise false
     */
    bool loadSoundData();

    /**
     * @brief unloadSound TODO: Need code for this class or nothing will be cleaned up
     */
    void unloadSound() {}


    std::map<GameSound, int> sndSlotMapDreams;

private:



    /**
     * @brief readPCSpeakerSoundintoWaveForm
     * @param soundslot
     * @param pcsdata
     * @param bytesize
     * @param formatsize
     * @return
     */
    bool readPCSpeakerSoundintoWaveForm(CSoundSlot &soundslot, const byte *pcsdata, const Uint8 formatsize);


    /**
     * @brief CacheAudioChunk   Reads in a single sound chunk
     * @param chunk
     */
    void CacheAudioChunk(int32_t chunk);


    /**
     * @brief LoadAudioFiles    Load the files required for audio transition
     * @return true if everything went fine, otherwise false
     */
    bool LoadAudioFiles();


};

#endif // AUDIODREAMS_H
