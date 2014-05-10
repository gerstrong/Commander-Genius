/*
 * CBridges.cpp
 *
 *  Created on: 10.07.2010
 *      Author: gerstrong
 */

#include "CBridges.h"
#include "engine/core/spritedefines.h"
#include <base/GsLogging.h>

unsigned int CBridges::m_bgtile = 0;

const int TILE_EXTENDING_PLATFORM = 270;

const int PLAT_EXTEND_RATE = 3;

CBridges::CBridges(CMap *pmap, Uint32 x, Uint32 y, Uint32 platx, Uint32 platy):
CVorticonSpriteObject(pmap, x, y, OBJ_BRIDGE),
m_state(EXTEND),
m_platx(platx),
m_platy(platy),
m_dir(LEFT)
{
	timer = 0;
	inhibitfall = true;
	canbezapped = false;
	sprite = BLANKSPRITE;

	// if the platform is already extended, turn ourselves
	// into an se_retract_plat()
	if ( mp_Map->at(m_platx, m_platy) == TILE_EXTENDING_PLATFORM )
	{
		// figure out which direction the bridge is supposed to go
		m_state = RETRACT;
		if (mp_Map->at(m_platx-1, m_platy) != TILE_EXTENDING_PLATFORM)
			m_dir = LEFT;
		else if(mp_Map->at(m_platx+1, m_platy) != TILE_EXTENDING_PLATFORM)
			m_dir = RIGHT;
		else
			m_dir = LEFT;

		// scan across until we find the end of the platform--that will
		// be where we will start (remove platform in oppisote direction
		// it was extended)
		do
		{
			if (mp_Map->at(m_platx, m_platy) != TILE_EXTENDING_PLATFORM)
			{ // we've found the end of the platform
				break;
			}
			if (m_dir==LEFT)
			{
				if (m_platx==mp_Map->m_width)
				{
					gLogging.ftextOut("SE_RETRACT_PLATFORM: Failed to find end of platform when scanning right.");
					return;
				}
				m_platx++;
			}
			else
			{ // platform will be removed in a right-going direction
				if (m_platx==0)
				{
					gLogging.ftextOut("SE_RETRACT_PLATFORM: Failed to find end of platform when scanning left.");
					return;
				}
				m_platx--;
			}
		} while(1);

		// when we were scanning we went one tile too far, go back one
		if (m_dir==LEFT) m_platx--;
		else m_platx++;
		return;
	}

	// figure out which direction the bridge is supposed to go
	CTileProperties &TileProperties = g_pBehaviorEngine->getTileProperties(1).at(mp_Map->at(m_platx+1, m_platy));
	if (!TileProperties.bleft)
		m_dir = RIGHT;
	else
		m_dir = LEFT;

	// get the background tile from the tile above the starting point
	if(m_dir == RIGHT)
		m_bgtile = mp_Map->at(m_platx+1, m_platy);
	else
		m_bgtile = mp_Map->at(m_platx-1, m_platy);		
}

void CBridges::process()
{
	if(m_state == EXTEND)
		extend();
	else
		retract();
}

void CBridges::extend()
{
	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
	
	if (!timer)
	{
		auto &tileProp = TileProperties[mp_Map->at(m_platx, m_platy)];
		
		if (m_dir==RIGHT && !tileProp.bleft)
		{
			mp_Map->changeTile(m_platx, m_platy, TILE_EXTENDING_PLATFORM);
			m_platx++;
			timer = PLAT_EXTEND_RATE;
		}
		else if(m_dir==LEFT && !tileProp.bright)
		{
			mp_Map->changeTile(m_platx, m_platy, TILE_EXTENDING_PLATFORM);
			m_platx--;
			timer = PLAT_EXTEND_RATE;
		}
		else
		{
			exists = false;
			return;
		}
	}
	else timer--;
}

void CBridges::retract()
{
	if (!timer)
	{
		if (mp_Map->at(m_platx, m_platy) == TILE_EXTENDING_PLATFORM)
		{
			mp_Map->setTile(m_platx, m_platy, m_bgtile, true);

			if (m_dir==RIGHT)
				m_platx++;
			else
				m_platx--;

			timer = PLAT_EXTEND_RATE;
		}
		else
		{
			exists = false;
		}
	}
	else timer--;
}
