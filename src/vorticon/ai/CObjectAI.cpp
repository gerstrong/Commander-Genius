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
	for( size_t i = 0 ; i<mp_Objvect->size() ; i++ )
	{
		CObject &object = mp_Objvect->at(i);

		if( checkforAIObject(object) )
		{
			object.processFalling();
			object.performCollision(mp_Map);

		    // hit detection with players
			object.touchPlayer = false;
		    for( int cplayer=0 ; cplayer<m_NumPlayers ; cplayer++)
		    {
		    	CPlayer &player = mp_Player[cplayer];
				CObject &playerobj = mp_Objvect->at(player.m_player_number);
				playerobj.x = player.x;
				playerobj.y = player.y;
				playerobj.sprite = 0;
				if (!player.pdie)
				{
					if ( object.hitdetect(playerobj) )
					{
						if (!player.godmode)
						{
							object.touchPlayer = true;
							object.touchedBy = cplayer;
						}
						else
						{
							if (object.m_type==OBJ_MOTHER || object.m_type==OBJ_BABY ||\
								object.m_type==OBJ_MEEP || object.m_type==OBJ_YORP)
							{
								if (object.canbezapped)
									object.zapped += 100;
							}
						}
						break;
					}
				}

		    }
			performSpecialAIType( object );
		}
		object.process();
	}

}

///
// do object and enemy AI
///
bool CObjectAI::checkforAIObject( CObject &object )
{
	int scrx = (object.x>>STC)-mp_Map->m_scrollx;
	int scry = (object.y>>STC)-mp_Map->m_scrolly;
	unsigned int type = object.m_type;

	if ( !object.exists || type==OBJ_PLAYER ) return false;

    //gamedo_calcenemyvisibility(i);

    // This will do the function gamedo_calcenemyvisibility(i);
    // check if object is really in the map!!!
    if (object.x < 0 || object.y < 0) return false;

    if (object.x > (mp_Map->m_width<<CSF) || object.y > (mp_Map->m_height<<CSF) )
		return false;

    if (scrx < -(g_pGfxEngine->Sprite[object.sprite]->getWidth()) || scrx > g_pVideoDriver->getGameResolution().w
		|| scry < -(g_pGfxEngine->Sprite[object.sprite]->getHeight()) || scry > g_pVideoDriver->getGameResolution().h)
    {
    	object.onscreen = false;
    	object.wasoffscreen = true;
        if (type==OBJ_ICEBIT) object.exists = false;
    }
    else
    {
    	object.onscreen = true;
    	object.hasbeenonscreen = true;
    }

	if (object.hasbeenonscreen || object.zapped ||
		type==OBJ_RAY || \
		type==OBJ_ICECHUNK || type==OBJ_PLATFORM ||
		type==OBJ_PLATVERT || type==OBJ_YORP ||
		type==OBJ_FOOB || type==OBJ_SCRUB)

	{
		return true;
    }
	return false;
}

void CObjectAI::performSpecialAIType( CObject &object )
{
	switch(object.m_type)
	{
		//KEEN1
		case OBJ_YORP: yorp_ai(object, mp_Player, m_difficulty>1); break;
		case OBJ_GARG: garg_ai(object, mp_Player, m_difficulty>1); break;
		case OBJ_VORT: vort_ai(object, m_Level, m_Episode, m_difficulty, false ); break;
		case OBJ_BUTLER: butler_ai(object, m_difficulty); break;
		case OBJ_TANK: tank_ai(object, m_difficulty>1); break;
		case OBJ_ICECANNON: icecannon_ai(object); break;
		case OBJ_ICECHUNK: icechunk_ai(object); break;
		case OBJ_ICEBIT: icebit_ai(object); break;
		case OBJ_TELEPORTER: teleporter_ai(object); break;
		case OBJ_ROPE: rope_ai(object); break;

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
		case OBJ_RAY: ray_ai( object, mp_Options[OPT_FULLYAUTOMATIC].value ); break;
		case OBJ_DOOR: door_ai( object, DOWN); break;
		case OBJ_AUTORAY: case OBJ_AUTORAY_V: autoray_ai(object); break;
		case OBJ_GOTPOINTS: gotpoints_ai(object); break;

			//case OBJ_DEMOMSG: break;

		default:
			//g_pLogFile->ftextOut("gamedo_enemy_ai: Object is of invalid type %d\n", object.m_type);
			break;
    }
}

void CObjectAI::SetAllCanSupportPlayer(CObject &object, int state)
{
	 for(int i=0;i<m_NumPlayers;i++)
		 object.cansupportplayer[i] = state;
}

void CObjectAI::killplayer(int theplayer)
{
	mp_Player[theplayer].kill();
}

///
// Cleanup Routine
///
void CObjectAI::deleteObj(CObject &object)
{
	object.exists = false;

	// The real delete happens, when all the AI is done
	// If the last object was deleted, throw it out of the list
	if( mp_Objvect->at(mp_Objvect->size()-1).exists == false )
		mp_Objvect->pop_back();
}

CObjectAI::~CObjectAI() {
	// TODO Auto-generated destructor stub
}
