/*
 * CAudioResources.cpp
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#include "CAudioResources.h"

#include "fileio/ResourceMgmt.h"
#include "vorbis/oggsupport.h"
#include "FindFile.h"
#include "CLogFile.h"
#include <SDL.h>

CAudioResources::CAudioResources(const SDL_AudioSpec &AudioSpec) :
m_AudioSpec(AudioSpec)
{
	for(int i=0 ; i<MAX_SOUNDS ; i++)
		m_soundslot[i].setpAudioSpec(&AudioSpec);
}
