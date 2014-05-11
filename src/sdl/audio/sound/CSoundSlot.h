/*
 * CSoundSlot.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUNDSLOT_H_
#define CSOUNDSLOT_H_

#include <base/TypeDefinitions.h>
#include <string>
#include <vector>
#include <SDL.h>

class CSoundSlot {
public:

	CSoundSlot();
	
	void openOGGSound(const std::string& filename, SDL_AudioSpec *pspec, Uint8 *&SoundBuffer, Uint32 &SoundLen);
	void setupWaveForm( Uint8 *buf, Uint32 len );
	void setupWaveForm( const std::vector<Uint8>& waveform );
	bool HQSndLoad(const std::string& gamepath, const std::string& soundname);
	void unload();
		
	unsigned int getSoundlength() const { return m_soundlength; }

    byte *getSoundData()
    {
        return mSounddata.data();
    }
	
	word priority;

private:
    std::vector<byte> mSounddata;
	unsigned int m_soundlength;
    bool mHasCommonFreqBase;
    int mOggFreq;
};

#endif /* CSOUNDSLOT_H_ */
