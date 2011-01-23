/*
 * CAudioResources.cpp
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#include "CAudioResources.h"

CAudioResources::CAudioResources(const SDL_AudioSpec &AudioSpec) {
	for(int i=0 ; i<MAX_SOUNDS ; i++)
		m_soundslot[i].setpAudioSpec(&AudioSpec);
}

CAudioResources::~CAudioResources() {
	// TODO Auto-generated destructor stub
}
