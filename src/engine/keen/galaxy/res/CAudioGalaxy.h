/*
 * CAudioGalaxy.h
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIOGALAXY_H_
#define CAUDIOGALAXY_H_

#include <base/audio/CAudioResources.h>
#include "fileio/CExeFile.h"

#include <base/GsLua.h>

class CAudioGalaxy : public CAudioResources
{
public:


    bool readPCSpeakerSoundintoWaveForm(CSoundSlot &soundslot,
                                        const gs_byte *pcsdata,
                                        const Uint8 formatsize );

    /**
     * @brief LoadFromAudioCK   Load audio from the CK files
     * @return true if everything went finde otherwise false
     */
    bool LoadFromAudioCK(const unsigned int dictOffset);

    bool loadSoundData(const unsigned int dictOffset);


    void unloadSound() {}
	
	std::map< unsigned int, std::map<GameSound, int> > sndSlotMapGalaxy;
	
private:
	void setupAudioMap();

    GsLua mLua;
};

#endif /* CAUDIOGALAXY_H_ */
