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

const Uint32 GalaxySongAssignments[] =
{
	0x02F206,
	0x03067A,
	0x03103E,
};


class CAudioGalaxy : public CAudioResources {
public:
	CAudioGalaxy(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec);
    
	bool readPCSpeakerSoundintoWaveForm(CSoundSlot &soundslot, const byte *pcsdata, const unsigned int bytesize, const Uint8 formatsize );
    
	bool LoadFromAudioCK(const CExeFile& ExeFile);
	bool loadSoundData();
	void unloadSound();
	
	std::map< unsigned int, std::map<GameSound, int> > sndSlotMapGalaxy;
	
private:
	void setupAudioMap();
    
	const CExeFile &m_ExeFile;
};

#endif /* CAUDIOGALAXY_H_ */
