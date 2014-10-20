/*
 * CVorticonSpriteObjectAI.cpp
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#include "CVorticonSpriteObjectAI.h"
//#include "sdl/CVideoDriver.h"
#include <base/GsLogging.h>

#include "CMeep.h"

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
// Pump Events  //
//////////////////

void CVorticonSpriteObjectAI::pumpEvent(const CEvent *evPtr)
{
    if( const EventSpawnObject *ev =  dynamic_cast<const EventSpawnObject*>(evPtr) )
    {
        CVorticonSpriteObject *ptr = (CVorticonSpriteObject*)(ev->pObject);
        std::unique_ptr<CVorticonSpriteObject> obj( ptr );
        m_Objvect.push_back( move(obj) );
    }    

    if( const AddPointsToAllPlayers *ev = dynamic_cast<const AddPointsToAllPlayers*>(evPtr) )
    {
        for( auto &obj : m_Objvect )
        {
            // Only remove non-player objects!
            if( CPlayer *player = dynamic_cast<CPlayer*>(obj.get()) )
            {
                player->inventory.score += ev->mPoints;
            }
        }
    }


    if( dynamic_cast<const EventEraseAllEnemies*>(evPtr) )
    {
        for( auto &obj : m_Objvect )
        {
            // Only remove non-player objects!
            if( dynamic_cast<CPlayer*>(obj.get()) == nullptr )
            {
                obj->exists = false;
            }
        }
    }

    if( dynamic_cast<const EventEraseAllMeeps*>(evPtr) )
    {
        for( auto &obj : m_Objvect )
        {
            // Only remove non-player objects!
            if( dynamic_cast<CMeep*>(obj.get()) != nullptr )
            {
                obj->exists = false;
            }
        }
    }
}

//////////////////
// AI Processes //
//////////////////
void CVorticonSpriteObjectAI::process()
{
    auto objectPtr = m_Objvect.begin();
	for( ; objectPtr != m_Objvect.end() ; objectPtr++ )
	{
		CVorticonSpriteObject &object = *(objectPtr->get());

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
					    if(object.isNearby(*it_player))
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

				}

				object.process();

				auto theOther = objectPtr; theOther++;
				for( ; theOther != m_Objvect.end() ; theOther++ )
				{
                    bool nearBy = false;

                    nearBy |= object.isNearby(**theOther);
                    nearBy |= (*theOther)->isNearby(object);

                    if(nearBy)
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
