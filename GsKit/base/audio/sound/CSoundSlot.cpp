/*
 * CSoundSlot.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <base/GsLogging.h>
#include <fileio/fileio.h>
#include <fileio/ResourceMgmt.h>
#include <base/TypeDefinitions.h>
#include <base/audio/base/Sampling.h>
#include <base/audio/Audio.h>
#include <base/interface/FindFile.h>

#include "CSoundSlot.h"

CSoundSlot::CSoundSlot()
{}

CSoundSlot::~CSoundSlot()
{
    unload();
}

void CSoundSlot::setupWaveForm( Uint8 *buf, Uint32 len )
{
    unload();

	m_soundlength = len;
    mSounddata.resize(m_soundlength);
    memcpy(mSounddata.data(), buf, m_soundlength);    

    if(!(mpWaveChunk = Mix_QuickLoad_RAW(mSounddata.data(), mSounddata.size())))
    {
        gLogging.ftextOut("Mix_QuickLoad_WAV: %s\n", Mix_GetError());
        // handle error
        return;
    }    
}

void CSoundSlot::setupWaveForm( const std::vector<Uint8>& waveform )
{
    setupWaveForm( (Uint8*)(&waveform[0]),
                   Uint32(waveform.size()) );
}

bool CSoundSlot::HQSndLoad(const std::string& gamepath,
                           const std::string& soundname)
{
    // load sample.wav in to sample
    std::string buf;

    buf = getResourceFilename("snd/" + soundname + ".OGG", gamepath, false, true); // Start with OGG

    if(buf != "")
    {
        buf = getResourceFilename("snd/" + soundname + ".WAV", gamepath, false); // Start with OGG
    }

    if(buf != "")
    {
        return false;
    }

    unload();

    if(!(mpWaveChunk=Mix_LoadWAV(buf.c_str())))
    {
        gLogging.ftextOut("Mix_LoadWAV: %s\n", Mix_GetError());
        // handle error
        return false;
    }

    return true;
}

void CSoundSlot::unload()
{
    if(!mSounddata.empty())
    {
        mSounddata.clear();
    }        

    if(mpWaveChunk)
    {
        Mix_FreeChunk(mpWaveChunk);
        mpWaveChunk = nullptr;
    }
}


