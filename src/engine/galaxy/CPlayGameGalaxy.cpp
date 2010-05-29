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

void CPlayGameGalaxy::process()
{

}

bool CPlayGameGalaxy::init()
{return false;}

void CPlayGameGalaxy::cleanup()
{}

CPlayGameGalaxy::~CPlayGameGalaxy()
{
	// TODO Auto-generated destructor stub
}

}
