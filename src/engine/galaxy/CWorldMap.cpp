/*
 * CWorldMap.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CWorldMap.h"
#include "CMapLoaderGalaxy.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"
#include "../../StringUtils.h"

namespace galaxy {

CWorldMap::CWorldMap(CExeFile &ExeFile):
m_active(false),
m_ExeFile(ExeFile),
m_newLevel(false)
{}

bool CWorldMap::isActive()
{	return m_active;	}

void CWorldMap::setActive(bool value)
{	m_active = value;	}

void CWorldMap::init()
{
	// Load the World map level.
	CMapLoaderGalaxy MapLoader(m_ExeFile, m_ObjectPtr);

	m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());
	MapLoader.loadMap(m_Map, 0); // Map Level?

	m_Map.drawAll();
}

void CWorldMap::process()
{
	processInput();

	// Animate the tiles of the map
	m_Map.animateAllTiles();

	for(size_t i=0 ; i<m_ObjectPtr.size() ; i++)
	{
		CObject* p_Object = m_ObjectPtr[i];

		p_Object->process();
	}

	g_pVideoDriver->blitScrollSurface();

	for( std::vector<CObject*>::iterator obj=m_ObjectPtr.begin() ;
			obj!=m_ObjectPtr.end() ; obj++ )
	{
		(*obj)->draw();
	}
}

/**
 * \brief This is called every game cycle. Here we check if the Player has chosen a new level
 * \param new_level the new level that has been detected in case of
 * \return return true, if player tries opening a level or false if not
 */
bool CWorldMap::PollNewLevel(Uint8 &new_level)
{
	if(m_newLevel)
	{

	}
}

/*
 *	makes the player finish the level
 */
void CWorldMap::finishLevel(Uint16 object)
{
	// if a door or other blocker was found remove it
	int x, y;
	Uint16 door = (object - 0xC000) + 0xD000;
	while(mp_Map->findTile(door, &x, &y, 2))
	{
		// Open blocks in case there are
		mp_Map->setTile( x, y, 0, true, 1);
		mp_Map->setTile( x, y, 0, true, 2);
		mp_Map->redrawAt( x, y);
	}

	Uint16 flag_dest = (object - 0xC000) + 0xF000;
	if(mp_Map->findTile(flag_dest, &x, &y, 2))
	{
		// spawn the flag
		VectorD2<Uint32> src(this->x, this->y);
		VectorD2<Uint32> dst((x<<CSF), (y<<CSF));

		CFlag *pFlag = new CFlag(mp_Map, src, dst);
		m_ObjectPtrs.push_back(pFlag);

		// Mark the tileinfo on the map as marked!!
		mp_Map->setTile( x, y, 0, true, 2);
	}
}

void CWorldMap::processInput()
{

}

CWorldMap::~CWorldMap() {
	while(!m_ObjectPtr.empty())
	{
		delete m_ObjectPtr.back();
		m_ObjectPtr.pop_back();
	}
}

}
