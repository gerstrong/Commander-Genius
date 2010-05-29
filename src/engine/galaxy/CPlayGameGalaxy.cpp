/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPlayGameGalaxy.h"

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(char episode, char level,
		 char numplayers, char difficulty,
		 std::string &gamepath) :
CPlayGame(episode, level, numplayers, difficulty, gamepath)
{}

bool CPlayGameGalaxy::loadGameState()
{return false;}

// Setup for the ingame
bool CPlayGameGalaxy::init()
{
	// Load the Level map. We have two modes. Inlevel and game map

	// TODO: Lets load the main map for now and create process for this

	return false;
}


// The main ingame process cycle when keen galaxy is up and running
void CPlayGameGalaxy::process()
{
	// TODO: Process code for the main map
}

void CPlayGameGalaxy::cleanup()
{}

CPlayGameGalaxy::~CPlayGameGalaxy()
{
	// TODO Auto-generated destructor stub
}

}
