/*
 * CMusicPlayer.cpp
 *
 *  Created on: 17.02.2011
 *      Author: gerhard
 */

#include "CMusicPlayer.h"

CMusicPlayer::CMusicPlayer() :
m_playing(false),
mLoadedTune(false)
{}

CMusicPlayer::~CMusicPlayer()
{}

void CMusicPlayer::reload()
{
	close();
	open();
}

void CMusicPlayer::play(const bool value)
{
	if(mLoadedTune)
		m_playing = value;
}

