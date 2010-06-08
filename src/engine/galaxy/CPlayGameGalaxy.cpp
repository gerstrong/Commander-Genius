/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPlayGameGalaxy.h"

#include "CMapLoaderGalaxy.h"

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(CExeFile &ExeFile, char level,
		 char numplayers, char difficulty, stOption *p_option) :
CPlayGame(ExeFile, level, numplayers, difficulty, p_option)
{}

bool CPlayGameGalaxy::loadGameState()
{return false;}

// Setup for the ingame
bool CPlayGameGalaxy::init()
{
	// Load the Level map. We have two modes. Inlevel and game map

	// TODO: Lets load the main map for now and create process for this
	CMapLoaderGalaxy MapLoader(m_ExeFile);

	// MapLoader.loadMap(m_Map, 1); // Map Level?

	// We should create this as base and two more classes. One will be for ingame and the other for
	// the map processes
	//CMapLoaderGalaxy MapLoaderGalaxy;

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
