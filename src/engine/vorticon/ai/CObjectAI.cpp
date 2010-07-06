/*
 * CObjectAI.cpp
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#include "CObjectAI.h"
#include "../../../sdl/CVideoDriver.h"
#include "../../../CLogFile.h"

CObjectAI::CObjectAI(CMap *p_map, std::vector<CObject*> &objvect, std::vector<CPlayer> &Player,
					 stOption *p_options, int NumPlayers, int episode, int level,
					 char difficulty, bool &dark) :
m_Objvect(objvect),
m_Player(Player),
m_difficulty(difficulty),
m_dark(dark)
{
	mp_Map = p_map;
	mp_Options = p_options;
	m_Level = level;
	m_Episode = episode;
	m_gunfiretimer = 0;
	sparks_left = 0;
	PlatExtending = false;
}

//////////////////
// AI Processes //
//////////////////
void CObjectAI::process()
{

	for( size_t i=0 ; i < m_Objvect.size() ; i++ )
	{
		CObject &object = *m_Objvect.at(i);
		if( checkforAIObject(object) )
		{
			object.processFalling();

		    // hit detection with players
			object.touchPlayer = false;
			std::vector<CPlayer>::iterator it_player = m_Player.begin();
			for( ; it_player != m_Player.end() ; it_player++ )
		    {
				if (!it_player->pdie)
				{
					if ( object.hitdetect(*it_player) )
					{
						object.touchPlayer = true;
						object.touchedBy = it_player->m_index;

						break;
					}
				}

		    }

			object.process();
		}
	}

	if(m_gunfiretimer < ((m_Episode==3) ? 180 : 50 )) m_gunfiretimer++;
	else m_gunfiretimer=0;
}

///
// do object and enemy AI
///
bool CObjectAI::checkforAIObject( CObject &object )
{
	unsigned int type = object.m_type;

	if ( !object.exists || type==OBJ_PLAYER ) return false;

	if( type==OBJ_EXPLOSION || type==OBJ_EARTHCHUNK ) return true;

	// Check if enemy is near enough. If he isn't, don't make him perform. Exception is on the map
	if(m_Level != 80)
	{
		bool is_near_enough=false;
		int x, y;

		std::vector<CPlayer>::iterator it_player = m_Player.begin();
		for( ; it_player != m_Player.end() ; it_player++ )
		{
			x = it_player->getXPosition();
			y = it_player->getYPosition();
			is_near_enough |= object.calcVisibility(x, y);
		}

		if(!is_near_enough) return false;
	}

   	object.onscreen = true;

   	if (object.hasbeenonscreen || object.zapped ||
		type==OBJ_RAY ||
		type==OBJ_ICECHUNK || type==OBJ_PLATFORM ||
		type==OBJ_PLATVERT || type==OBJ_YORP ||
		type==OBJ_FOOB || type==OBJ_SCRUB)

	{
		return true;
    }

   	return false;
}

void CObjectAI::SetAllCanSupportPlayer(CObject &object, bool state)
{
	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		object.cansupportplayer = state;
		if(!state && it_player->supportedbyobject)
		{
			it_player->pfalling=true;
			it_player->moveDown(1);
			it_player->blockedd=false;
		}
	}
}

void CObjectAI::killplayer(int theplayer)
{
	m_Player[theplayer].kill();
}

// anything (players/enemies) occupying the map tile at [mpx,mpy] is killed
void CObjectAI::kill_all_intersecting_tile(int mpx, int mpy)
{
	 unsigned int xpix,ypix;
	 unsigned int x, y;
	 xpix = mpx<<CSF;
	 ypix = mpy<<CSF;

	 std::vector<CObject*>::iterator object;
	 for( object=m_Objvect.begin() ; object!=m_Objvect.end() ; object++ )
	 {
		 x = (*object)->getXMidPos();
		 y = (*object)->getYUpPos();
		 if ((*object)->exists)
			 if (xpix-(1<<CSF) <= x && xpix+(1<<CSF) >= x)
				 if (ypix <= y && ypix+(1<<CSF) >= y)
					 (*object)->kill();
	 }
}

///
// Cleanup Routine
///
void CObjectAI::deleteAllObjects()
{
	// The real delete happens, when all the AI is done
	// If the last object was deleted, throw it out of the list
	if(!m_Objvect.empty())
		m_Objvect.clear();
}

CObjectAI::~CObjectAI() {
	deleteAllObjects();
}
