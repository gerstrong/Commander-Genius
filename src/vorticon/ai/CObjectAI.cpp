/*
 * CObjectAI.cpp
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#include "CObjectAI.h"
#include "../../sdl/CVideoDriver.h"
#include "../../CLogFile.h"

CObjectAI::CObjectAI(CMap *p_map, std::vector<CObject> *p_objvect, CPlayer *p_player,
					 stOption *p_options, int NumPlayers, int episode, int level ,char difficulty)  :
m_difficulty(difficulty)
{
	mp_Map = p_map;
	mp_Objvect = p_objvect;
	mp_Options = p_options;
	m_Level = level;
	m_Episode = episode;
	m_NumPlayers = NumPlayers;
	mp_Player = p_player;
}

//////////////////
// AI Processes //
//////////////////
void CObjectAI::process()
{
	CPlayer *p_player;
	CObject *i_object;
	for( size_t i = 0 ; i<mp_Objvect->size() ; i++ )
	{
		i_object = &mp_Objvect->at(i);
		if( checkforAIObject(&(*i_object)) )
		{
			i_object->processFalling();
			i_object->performCollision(mp_Map);

		    // hit detection with players
			i_object->touchPlayer = false;
		    for( int cplayer=0 ; cplayer<m_NumPlayers ; cplayer++)
		    {
		    	p_player = &mp_Player[cplayer];
				//if (p_player->isPlaying)
				{
		    		CObject *p_playerobj;
		    		p_playerobj = &mp_Objvect->at(p_player->m_player_number);
		    		p_playerobj->x = p_player->x;
		    		p_playerobj->y = p_player->y;
		    		p_playerobj->sprite = 0;
					if (!p_player->pdie)
					{
						if ( i_object->hitdetect(p_playerobj) )
						{
							if (!p_player->godmode)
							{
								i_object->touchPlayer = true;
								i_object->touchedBy = cplayer;
							}
							else
							{
								if (i_object->m_type==OBJ_MOTHER || i_object->m_type==OBJ_BABY ||\
									i_object->m_type==OBJ_MEEP || i_object->m_type==OBJ_YORP)
								{
									if (i_object->canbezapped)
										i_object->zapped += 100;
								}
							}
							break;
						}
					}
				}
		    }
			performSpecialAIType( &(*i_object) );
		}
		i_object->process();
	}

}

///
// do object and enemy AI
///
bool CObjectAI::checkforAIObject( CObject *p_object )
{
	int scrx = (p_object->x>>STC)-mp_Map->m_scrollx;
	int scry = (p_object->y>>STC)-mp_Map->m_scrolly;
	unsigned int type = p_object->m_type;

	if ( !p_object->exists || type==OBJ_PLAYER ) return false;

    //gamedo_calcenemyvisibility(i);

    // This will do the function gamedo_calcenemyvisibility(i);
    // check if object is really in the map!!!
    if (p_object->x < 0 || p_object->y < 0) return false;

    if (p_object->x > (mp_Map->m_width<<CSF) || p_object->y > (mp_Map->m_height<<CSF) )
		return false;

    if (scrx < -(g_pGfxEngine->Sprite[p_object->sprite]->getWidth()) || scrx > g_pVideoDriver->getGameResolution().w
		|| scry < -(g_pGfxEngine->Sprite[p_object->sprite]->getHeight()) || scry > g_pVideoDriver->getGameResolution().h)
    {
    	p_object->onscreen = false;
    	p_object->wasoffscreen = true;
        if (type==OBJ_ICEBIT) p_object->exists = false;
    }
    else
    {
    	p_object->onscreen = true;
    	p_object->hasbeenonscreen = true;
    }

	if (p_object->hasbeenonscreen || p_object->zapped ||
		type==OBJ_RAY || \
		type==OBJ_ICECHUNK || type==OBJ_PLATFORM ||
		type==OBJ_PLATVERT || type==OBJ_YORP ||
		type==OBJ_FOOB || type==OBJ_SCRUB)

	{
		return true;
    }
	return false;
}

void CObjectAI::performSpecialAIType( CObject *p_object )
{
	switch(p_object->m_type)
	{
		//KEEN1
		case OBJ_YORP: yorp_ai(p_object, mp_Player, m_difficulty); break;
		case OBJ_GARG: garg_ai(p_object, mp_Player, m_difficulty); break;
		case OBJ_VORT: vort_ai(p_object, m_Level, m_Episode, m_difficulty, false ); break;
		case OBJ_BUTLER: butler_ai(p_object, m_difficulty); break;
		case OBJ_TANK: tank_ai(p_object, m_difficulty>1); break;
		case OBJ_ICECANNON: icecannon_ai(p_object); break;
		case OBJ_ICECHUNK: icechunk_ai(p_object); break;
		case OBJ_ICEBIT: icebit_ai(p_object); break;
		case OBJ_TELEPORTER: teleporter_ai(p_object); break;
		case OBJ_ROPE: rope_ai(p_object); break;

		//KEEN2
		//case OBJ_SCRUB: scrub_ai(p_object); break;
		//case OBJ_TANKEP2: tankep2_ai(i, p_levelcontrol->hardmode); break;
		//case OBJ_PLATFORM: platform_ai(i, *p_levelcontrol); break;
		//case OBJ_VORTELITE: vortelite_ai(i, p_levelcontrol->dark); break;
		//case OBJ_SECTOREFFECTOR: se_ai(i, p_levelcontrol ); break;
		//case OBJ_BABY: baby_ai(i, p_levelcontrol->episode,
		//p_levelcontrol->hardmode); break;
		//case OBJ_EXPLOSION: explosion_ai(i); break;
		//case OBJ_EARTHCHUNK: earthchunk_ai(i); break;
		//case OBJ_SPARK: spark_ai(i, &(p_levelcontrol->sparks_left) ); break;

		//KEEN3
		//case OBJ_FOOB: foob_ai(i, p_levelcontrol->hardmode); break;
		//case OBJ_NINJA: ninja_ai( i, p_levelcontrol->hardmode); break;
		//case OBJ_MEEP: meep_ai( i, *p_levelcontrol ); break;
		//case OBJ_SNDWAVE: sndwave_ai( i, p_levelcontrol->hardmode); break;
		//case OBJ_MOTHER: mother_ai( i, *p_levelcontrol ); break;
		//case OBJ_FIREBALL: fireball_ai( i, p_levelcontrol->hardmode ); break;
		//case OBJ_BALL: ballandjack_ai(i); break;
		//case OBJ_JACK: ballandjack_ai(i); break;
		//case OBJ_PLATVERT: platvert_ai(i); break;
		//case OBJ_NESSIE: nessie_ai(i); break;

			 //Common Objects*/
		case OBJ_RAY: ray_ai( p_object, mp_Options[OPT_FULLYAUTOMATIC].value ); break;
		case OBJ_DOOR: door_ai( p_object, DOWN); break;
		//case OBJ_AUTORAY: case OBJ_AUTORAY_V: autoray_ai(i); break;
		//case OBJ_GOTPOINTS: gotpoints_ai(i); break;

			//case OBJ_DEMOMSG: break;

		default:
			//g_pLogFile->ftextOut("gamedo_enemy_ai: Object is of invalid type %d\n", p_object->m_type);
			break;
    }
}

void CObjectAI::SetAllCanSupportPlayer(CObject &object, int state)
{
	unsigned int i;
	 for(i=0;i<m_NumPlayers;i++)
		 object.cansupportplayer[i] = state;
}

void CObjectAI::killplayer(int theplayer)
{
	mp_Player[theplayer].kill();
}

///
// Cleanup Routine
///
void CObjectAI::deleteObj(CObject *p_object)
{
	p_object->exists = false;

	// The real delete happens, when all the AI is done
	// If the last object was deleted, throw it out of the list
	if( mp_Objvect->at(mp_Objvect->size()-1).exists == false )
		mp_Objvect->pop_back();
}

CObjectAI::~CObjectAI() {
	// TODO Auto-generated destructor stub
}
