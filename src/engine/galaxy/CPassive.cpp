/*
 * CPassive.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPassive.h"

#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"

namespace galaxy
{

CPassiveGalaxy::CPassiveGalaxy(char Episode, std::string DataDirectory,
		CSavedGame &SavedGame, stOption *p_Option) :
CPassive(Episode, DataDirectory, SavedGame, p_Option)
{
	// TODO Auto-generated destructor stub
}

bool CPassiveGalaxy::init(char mode)
{
	return true;
}

void CPassiveGalaxy::process()
{
	CBitmap &Bitmap = g_pGfxEngine->getBitmap(103);

	// draw Bitmap here!
	Bitmap.draw(g_pVideoDriver->BlitSurface, 0, 0);
	
	if (g_pInput->getPressedAnyKey())
	{
		m_modeg = true;
	}
}

void CPassiveGalaxy::cleanup()
{

}

CPassiveGalaxy::~CPassiveGalaxy()
{
	// TODO Auto-generated destructor stub
}

}
