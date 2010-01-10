/*
 * CObjectAI.cpp
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#include "CObjectAI.h"
#include "../../sdl/CVideoDriver.h"
#include "../../CLogFile.h"

CObjectAI::CObjectAI(CMap *p_map, std::vector<CObject> &objvect, std::vector<CPlayer> &Player,
					 stOption *p_options, int NumPlayers, int episode, int level,
					 char difficulty, CPhysicsSettings &PhysicsSettings, bool &dark) :
m_Objvect(objvect),
m_Player(Player),
m_PhysicsSettings(PhysicsSettings),
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
	//size_t num_objects = m_Objvect.size();
	for( size_t i=0 ; i < m_Objvect.size() ; i++ )
	{
		CObject &object = m_Objvect.at(i);
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
						if (!it_player->godmode)
						{
							object.touchPlayer = true;
							object.touchedBy = it_player->m_index;
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

	if(m_gunfiretimer< ((m_Episode==3) ? 180 : 60 )) m_gunfiretimer++;
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

void CObjectAI::performSpecialAIType( CObject &object )
{
	switch(object.m_type)
	{
		//KEEN1
		case OBJ_YORP: yorp_ai(object, &m_Player[0], m_difficulty>1); break;
		case OBJ_GARG: garg_ai(object, &m_Player[0], m_difficulty>1); break;
		case OBJ_VORT: vort_ai(object, m_Level, m_Episode, m_difficulty, m_dark ); break;
		case OBJ_BUTLER: butler_ai(object, m_difficulty); break;
		case OBJ_TANK: tank_ai(object, m_difficulty>1); break;
		case OBJ_ICECANNON: icecannon_ai(object); break;
		case OBJ_ICECHUNK: icechunk_ai(object); break;
		case OBJ_ICEBIT: icebit_ai(object); break;
		case OBJ_TELEPORTER: teleporter_ai(object); break;
		case OBJ_ROPE: rope_ai(object); break;

		//KEEN2
		case OBJ_SCRUB: scrub_ai(object); break;
		case OBJ_TANKEP2: tankep2_ai(object, m_difficulty>1); break;
		case OBJ_PLATFORM: platform_ai(object); break;
		case OBJ_VORTELITE: vortelite_ai(object, m_dark); break;
		case OBJ_SECTOREFFECTOR: se_ai(object); break;
		case OBJ_BABY: baby_ai(object, m_Episode, m_difficulty>1); break;
		case OBJ_EXPLOSION: explosion_ai(object); break;
		case OBJ_EARTHCHUNK: earthchunk_ai(object); break;
		case OBJ_SPARK: spark_ai(object, sparks_left ); break;

		//KEEN3
		case OBJ_FOOB: foob_ai(object, m_difficulty>1); break;
		case OBJ_NINJA: ninja_ai(object, m_difficulty>1); break;
		case OBJ_MEEP: meep_ai(object); break;
		case OBJ_SNDWAVE: sndwave_ai(object, m_difficulty>1); break;
		case OBJ_MOTHER: mother_ai(object, m_difficulty>1 ); break;
		case OBJ_FIREBALL: fireball_ai(object, m_difficulty>1 ); break;
		case OBJ_BALL: ballandjack_ai(object); break;
		case OBJ_JACK: ballandjack_ai(object); break;
		case OBJ_PLATVERT: platvert_ai(object); break;
		case OBJ_NESSIE: nessie_ai(object); break;

		//Common Objects
		case OBJ_RAY: ray_ai( object, mp_Options[OPT_FULLYAUTOMATIC].value ); break;
		case OBJ_DOOR: door_ai( object, DOWN); break;
		case OBJ_AUTORAY: case OBJ_AUTORAY_V: autoray_ai(object); break;
		case OBJ_GOTPOINTS: gotpoints_ai(object); break;

			//case OBJ_DEMOMSG: break;

		default:
			g_pLogFile->ftextOut("gamedo_enemy_ai: Object is of invalid type %d\n", object.m_type);
			break;
    }
}

void CObjectAI::SetAllCanSupportPlayer(CObject &object, bool state)
{
	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
		object.cansupportplayer = state;
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

	 std::vector<CObject>::iterator object;
	 for( object=m_Objvect.begin() ; object!=m_Objvect.end() ; object++ )
	 {
		 x = object->getXPosition();
		 y = object->getYPosition();
		 if (object->exists)
			 if (xpix <= x && xpix+(1<<CSF) >= x)
				 if (ypix <= y && ypix+(1<<CSF) >= y)
					 object->kill();
	 }
}

bool CObjectAI::getPlatMoving()
{
	return PlatExtending;
}

void CObjectAI::triggerPlat(bool value)
{
	PlatExtending = value;
}

///
// Cleanup Routine
///
void CObjectAI::deleteObj(CObject &object)
{
	object.exists = false;
}

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
