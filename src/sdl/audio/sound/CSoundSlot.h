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

#include <SDL_mixer.h>

class CSoundSlot
{
public:

	CSoundSlot();

    virtual ~CSoundSlot();

	
	void setupWaveForm( Uint8 *buf, Uint32 len );
	void setupWaveForm( const std::vector<Uint8>& waveform );
	bool HQSndLoad(const std::string& gamepath, const std::string& soundname);
	void unload();
		
	unsigned int getSoundlength() const { return m_soundlength; }

    byte *getSoundData()
    {
        return mSounddata.data();
    }

    auto WaveChunk()
    {
        return mpWaveChunk;
    }
	
	word priority;

private:
    std::vector<byte> mSounddata;
    unsigned int m_soundlength = 0;
    Mix_Chunk *mpWaveChunk = nullptr;
};

#endif /* CSOUNDSLOT_H_ */
