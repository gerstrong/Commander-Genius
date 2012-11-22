/*
 * CVorticonSpriteObjectAI.cpp
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#include "CVorticonSpriteObjectAI.h"
#include "sdl/CVideoDriver.h"
#include "CLogFile.h"

CVorticonSpriteObjectAI::CVorticonSpriteObjectAI(CMap *p_map, 
					 std::vector< std::unique_ptr<CVorticonSpriteObject> > &objvect,
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
		CVorticonSpriteObject &object = *m_Objvect[i];

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

				auto theOther = m_Objvect.begin(); theOther++;
				for( ; theOther != m_Objvect.end() ; theOther++ )
				{
				    bool near = false;
				    
				    near |= object.isNearby(**theOther);
				    near |= (*theOther)->isNearby(object);
				    if(near)
				    {									    
					if( object.hitdetect(**theOther) )
					{
						object.getTouchedBy(**theOther);
						(*theOther)->getTouchedBy(object);
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
	    CVorticonSpriteObject *ptr = (CVorticonSpriteObject*)(ev->pObject);
	    std::unique_ptr<CVorticonSpriteObject> obj( ptr );
	    m_Objvect.push_back( move(obj) );
	    EventContainer.pop_Event();
	}

	if( !m_Objvect.empty() )
	{	
	    // Try always to remove the last objects if they aren't used anymore!
	    if(!m_Objvect.back()->exists)
	    {
		m_Objvect.pop_back();
	    }
	}

	if(m_gunfiretimer < ((m_Episode==3) ? 180 : 50 )) m_gunfiretimer++;
	else m_gunfiretimer=0;
}
