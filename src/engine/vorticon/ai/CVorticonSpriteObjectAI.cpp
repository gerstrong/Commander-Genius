/*
 * CVorticonSpriteObjectAI.cpp
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#include "CVorticonSpriteObjectAI.h"
#include "sdl/CVideoDriver.h"
#include "CLogFile.h"

CVorticonSpriteObjectAI::CVorticonSpriteObjectAI(CMap *p_map, std::vector<CVorticonSpriteObject*> &objvect,
					 std::vector<CPlayer> &Player,
					 int NumPlayers, int episode,
					 int level, bool &dark) :
m_Objvect(objvect),
m_Player(Player),
m_dark(dark)
{
	mp_Map = p_map;
	mp_Options = g_pBehaviorEngine->m_option;
	m_Level = level;
	m_Episode = episode;
	m_gunfiretimer = 0;
}

//////////////////
// AI Processes //
//////////////////
void CVorticonSpriteObjectAI::process()
{
	for( size_t i=0 ; i < m_Objvect.size() ; i++ )
	{
		CVorticonSpriteObject &object = *m_Objvect.at(i);

		if( object.checkforScenario() )
		{
			object.performCollisions();
			object.processFalling();

			if(!object.dead) // Only do that if not dead
			{
				// hit detection with players
				object.touchPlayer = false;
				std::vector<CPlayer>::iterator it_player = m_Player.begin();
				for( ; it_player != m_Player.end() ; it_player++ )
				{
					if (!it_player->pdie)
					{
						if ( object.hitdetect(*it_player) )
						{
							object.getTouchedBy(*it_player);

							object.touchPlayer = true;
							object.touchedBy = it_player->m_index;
							break;
						}
					}

				}

				object.process();

				std::vector<CVorticonSpriteObject*>::iterator theOther = m_Objvect.begin();
				for( ; theOther != m_Objvect.end() ; theOther++ )
				{
					if( *theOther != &object )
					{
						if( object.hitdetect(**theOther) )
						{
							object.getTouchedBy(**theOther);
						}
					}
				}
			}

			object.processEvents();
			object.InertiaAndFriction_X();
		}
	}
	
	
	CEventContainer &EventContainer = g_pBehaviorEngine->m_EventList;
	if( EventSpawnObject *ev =  EventContainer.occurredEvent<EventSpawnObject>() )
	{
		m_Objvect.push_back( static_cast<CVorticonSpriteObject*>
				    (const_cast<CSpriteObject*>(ev->pObject)) );
		EventContainer.pop_Event();
	}

	
	// Try always to remove the last objects if they aren't used anymore!
	CVorticonSpriteObject* p_object;
	while(m_Objvect.size()>0)
	{
		p_object = m_Objvect.at( m_Objvect.size()-1 );
		if(!p_object->exists)
		{
			delete p_object;
			m_Objvect.pop_back();
		}
		else
			break;
	}

	if(m_gunfiretimer < ((m_Episode==3) ? 180 : 50 )) m_gunfiretimer++;
	else m_gunfiretimer=0;
}

/*void CVorticonSpriteObjectAI::SetAllCanSupportPlayer(CVorticonSpriteObject &object, bool state)
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
}*/

///
// Cleanup Routine
///
void CVorticonSpriteObjectAI::deleteAllObjects()
{
	// The real delete happens, when all the AI is done
	// If the last object was deleted, throw it out of the list
	if(!m_Objvect.empty())
	{
		std::vector<CVorticonSpriteObject*>::iterator obj=m_Objvect.begin();
		for( ; obj != m_Objvect.end() ; obj++ )
			delete *obj;
		m_Objvect.clear();
	}
}

CVorticonSpriteObjectAI::~CVorticonSpriteObjectAI() {
	deleteAllObjects();
}
