/*
 * CMusicPlayer.cpp
 *
 *  Created on: 17.02.2011
 *      Author: gerhard
 */

#include "CMusicPlayer.h"


void CMusicPlayer::reload()
{
    close(true);
    open(true);
}

void CMusicPlayer::play(const bool value)
{
	m_playing = value;
}

