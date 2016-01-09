/*
 * CAudioGalaxy.h
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIOGALAXY_H_
#define CAUDIOGALAXY_H_

#include <sdl/audio/CAudioResources.h>
#include "fileio/CExeFile.h"

const Uint32 GalaxySongAssignments[] =
{
	0x02F206,
	0x03067A,
	0x03103E,
};


class CAudioGalaxy : public CAudioResources
{
public:


    bool readPCSpeakerSoundintoWaveForm(CSoundSlot &soundslot, const byte *pcsdata, const Uint8 formatsize );

    /**
     * @brief LoadFromAudioCK   Load audio from the CK files
     * @return true if everything went finde otherwise false
     */
    bool LoadFromAudioCK(const uint dictOffset);

    bool loadSoundData(const uint dictOffset);
	void unloadSound();
	
	std::map< unsigned int, std::map<GameSound, int> > sndSlotMapGalaxy;
	
private:
	void setupAudioMap();
    
    //const CExeFile &m_ExeFile;
};

#endif /* CAUDIOGALAXY_H_ */
