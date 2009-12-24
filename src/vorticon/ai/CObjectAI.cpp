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
					 char difficulty, CPhysicsSettings &PhysicsSettings) :
m_Objvect(objvect),
m_Player(Player),
m_PhysicsSettings(PhysicsSettings),
m_difficulty(difficulty)
{
	mp_Map = p_map;
	mp_Options = p_options;
	m_Level = level;
	m_Episode = episode;
	m_NumPlayers = NumPlayers;
	m_gunfiretimer = 0;
	sparks_left = 0;
	PlatExtending = false;
}

//////////////////
// AI Processes //
//////////////////
void CObjectAI::process()
{
	for( size_t i = 0 ; i<m_Objvect.size() ; i++ )
	{
		CObject &object = m_Objvect.at(i);

		if( checkforAIObject(object) )
		{
			object.processFalling();
			object.performCollision(mp_Map);

		    // hit detection with players
			object.touchPlayer = false;
		    for( int cplayer=0 ; cplayer<m_NumPlayers ; cplayer++)
		    {
		    	CPlayer &player = m_Player[cplayer];
				CObject &playerobj = m_Objvect.at(player.m_player_number);
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
	if(m_gunfiretimer<120) m_gunfiretimer++;
	else m_gunfiretimer=0;
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

	// Check if enemy is near enough. If he isn't, don't make him perform
	bool is_near_enough=false;
	for(int i=0 ; i<m_NumPlayers ; i++)
	{
		is_near_enough |= object.calcVisibility(m_Player[i].x, m_Player[i].y);
	}

	if(!is_near_enough /*|| type != OBJ_NESSIE*/) return false;

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
		case OBJ_VORT: vort_ai(object, m_Level, m_Episode, m_difficulty, false ); break;
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
		case OBJ_VORTELITE: vortelite_ai(object, /*m_dark*/false); break;
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
	m_Player[theplayer].kill();
}

// anything (players/enemies) occupying the map tile at [mpx,mpy] is killed
void CObjectAI::kill_all_intersecting_tile(int mpx, int mpy)
{
	 unsigned int xpix,ypix;
	 xpix = mpx<<CSF;
	 ypix = mpy<<CSF;

	 std::vector<CObject>::iterator object;
	 for( object=m_Objvect.begin() ; object!=m_Objvect.end() ; object++ )
	 {
		 if (object->exists)
			 if (xpix <= object->x && xpix+(1<<CSF) >= object->x)
				 if (ypix <= object->y && ypix+(1<<CSF) >= object->y)
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

	// The real delete happens, when all the AI is done
	// If the last object was deleted, throw it out of the list
	if( m_Objvect.at(m_Objvect.size()-1).exists == false )
		m_Objvect.pop_back();
}

void CObjectAI::deleteAllObjects()
{
	// The real delete happens, when all the AI is done
	// If the last object was deleted, throw it out of the list
	while(m_Objvect.size() > m_NumPlayers)
	{
		m_Objvect.back().exists = false;
		m_Objvect.pop_back();
	}
}

CObjectAI::~CObjectAI() {
	// TODO Auto-generated destructor stub
}
