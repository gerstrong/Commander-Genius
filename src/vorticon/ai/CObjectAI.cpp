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
		  stOption *p_options, int NumPlayers, int episode) {
	mp_Map = p_map;
	mp_Objvect = p_objvect;
	mp_Options = p_options;
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
			performCommonAI(&(*i_object));

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
							p_playerobj->touchPlayer = true;
							p_playerobj->touchedBy = cplayer;
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
	}

}

///
// do object and enemy AI
///
bool CObjectAI::checkforAIObject( CObject *p_object )
{
int scrx = (p_object->x>>(CSF-4))-mp_Map->m_scrollx;
int scry = (p_object->y>>(CSF-4))-mp_Map->m_scrolly;
unsigned int type = p_object->m_type;

	if ( !p_object->exists || type==OBJ_PLAYER ) return false;

    //gamedo_calcenemyvisibility(i);

    // This will do the function gamedo_calcenemyvisibility(i);
    // check if object is really in the map!!!
    if (p_object->x < 0 || p_object->y < 0) return false;

    if (p_object->x > (mp_Map->m_width<<CSF) || p_object->y > (mp_Map->m_height<<CSF) )
    	  return false;

    if (scrx < -(g_pGfxEngine->Sprite[p_object->sprite]->getWidth()) || scrx > g_pVideoDriver->getGameResRect().w
          || scry < -(g_pGfxEngine->Sprite[p_object->sprite]->getHeight()) || scry > g_pVideoDriver->getGameResRect().h)
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

// common enemy/object ai, such as falling, setting blocked variables,
// detecting player contact, etc.
void CObjectAI::performCommonAI( CObject *p_object )
{
int x0,y0,xa,ya,xsize,ysize;
int temp;
stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	if (p_object->m_type==OBJ_GOTPOINTS) return;

	ysize = g_pGfxEngine->Sprite[p_object->sprite]->getHeight()<<(CSF-TILE_S);
	xsize = g_pGfxEngine->Sprite[p_object->sprite]->getWidth()<<(CSF-TILE_S);

	// set value of blockedd--should object fall?
	temp = p_object->y+ysize;
	if ((temp>>CSF)<<CSF != temp) p_object->blockedd = false;
	else
	{ // on a tile boundary, test if tile under object is solid
		p_object->blockedd = false;
		x0 = p_object->x;
		y0 = p_object->y+ysize+1;
		for(xa=0;xa<xsize-2;xa+=(1<<CSF))
		{
			if ( TileProperty[mp_Map->at((x0+xa)>>CSF,y0>>CSF)].bup )
			{
				p_object->blockedd = true;
				return;
			}
		}

		if (!p_object->blockedd)	// check final point
		{
			if ( TileProperty[mp_Map->at((x0+xsize-2)>>CSF, y0>>CSF)].bup )
				p_object->blockedd = true;
		}

		// If the object is out of map
		if(y0 >= (int)((mp_Map->m_height-2)<<CSF)) p_object->blockedd = true;
	}

	// set blockedu
	p_object->blockedu = false;
	x0 = p_object->x;
	y0 = p_object->y;
	for(xa=1;xa<xsize;xa+=(1<<CSF))		// change start pixel to xa=1 for icecannon in ep1l8
	{
		if ( TileProperty[getmaptileat((x0+xa)>>CSF,y0>>CSF)].bdown )
		{
			p_object->blockedu = true;
			break;
		}
    }

	if (!p_object->blockedu)	// check final point
	{
		if ( TileProperty[mp_Map->at((x0+xsize-2)>>CSF, y0>>CSF)].bdown )
			p_object->blockedu = true;
	}

	// If the object is out of map
	if(y0 <= (2<<CSF)) p_object->blockedu = true;

	// set blockedl
	p_object->blockedl = false;
    x0 = p_object->x-1;
    y0 = p_object->y+1;
    for(ya=0;ya<ysize;ya+=(1<<CSF))
    {
        if (TileProperty[mp_Map->at(x0>>CSF,(y0+ya)>>CSF)].bright )
        {
          p_object->blockedl = true;
          goto blockedl_set;
        }
    }
    if ( TileProperty[mp_Map->at(x0>>CSF, (y0+ysize-1)>>CSF)].bright )
    	p_object->blockedl = true;

    blockedl_set: ;

	// If the object is out of map
	if(p_object->x <= (2<<CSF)) p_object->blockedl = true;

 // set blockedr
	p_object->blockedr = false;
    x0 = p_object->x+xsize;
    y0 = p_object->y+1;
    for(ya=0;ya<ysize;ya+=(1<<CSF))
    {
        if ( TileProperty[mp_Map->at(x0>>CSF,(y0+ya)>>CSF)].bleft )
        {
        	p_object->blockedr = true;
          goto blockedr_set;
        }
    }
    if ( TileProperty[mp_Map->at(x0>>CSF, ((y0+ysize-1)>>CSF))].bleft )
    {
    	p_object->blockedr = true;
    }
    blockedr_set: ;

    if(p_object->x >= (mp_Map->m_width-2)<<CSF) p_object->blockedr = true;

    // have object fall if it should
      if (!p_object->inhibitfall)
      {
           #define OBJFALLSPEED   20
           if (p_object->blockedd)
           {
        	   p_object->yinertia = 0;
           }
           else
           {
    #define OBJ_YINERTIA_RATE  5
             if (p_object->yinertiatimer>OBJ_YINERTIA_RATE)
             {
               if (p_object->yinertia < OBJFALLSPEED) p_object->yinertia++;
               p_object->yinertiatimer = 0;
             } else p_object->yinertiatimer++;
           }
           p_object->y += p_object->yinertia;
      }
}

void CObjectAI::performSpecialAIType( CObject *p_object )
{
  	 switch(p_object->m_type)
  	 {
  	 /*//KEEN1
	  case OBJ_YORP: yorp_ai(i, *p_levelcontrol); break;
	  case OBJ_GARG: garg_ai(i, p_levelcontrol->hardmode); break;
	  case OBJ_VORT: vort_ai(i, p_levelcontrol ); break;
	  case OBJ_BUTLER: butler_ai(i, p_levelcontrol->hardmode); break;
	  case OBJ_TANK: tank_ai(i, p_levelcontrol->hardmode); break;
	  case OBJ_DOOR: door_ai(i, p_levelcontrol->cepvars.DoorOpenDir); break;
	  case OBJ_ICECANNON: icecannon_ai(i); break;
	  case OBJ_ICECHUNK: icechunk_ai(i); break;
	  case OBJ_ICEBIT: icebit_ai(i); break;
	  case OBJ_TELEPORTER: teleporter_ai(i, *p_levelcontrol); break;
	  case OBJ_ROPE: rope_ai(i); break;

	  //KEEN2
	  case OBJ_SCRUB: scrub_ai(i, *p_levelcontrol); break;
	  case OBJ_TANKEP2: tankep2_ai(i, p_levelcontrol->hardmode); break;
	  case OBJ_PLATFORM: platform_ai(i, *p_levelcontrol); break;
	  case OBJ_VORTELITE: vortelite_ai(i, p_levelcontrol->dark); break;
	  case OBJ_SECTOREFFECTOR: se_ai(i, p_levelcontrol ); break;
	  case OBJ_BABY: baby_ai(i, p_levelcontrol->episode,
						  p_levelcontrol->hardmode); break;
	  case OBJ_EXPLOSION: explosion_ai(i); break;
	  case OBJ_EARTHCHUNK: earthchunk_ai(i); break;
	  case OBJ_SPARK: spark_ai(i, &(p_levelcontrol->sparks_left) ); break;

	  //KEEN3
	  case OBJ_FOOB: foob_ai(i, p_levelcontrol->hardmode); break;
	  case OBJ_NINJA: ninja_ai( i, p_levelcontrol->hardmode); break;
	  case OBJ_MEEP: meep_ai( i, *p_levelcontrol ); break;
	  case OBJ_SNDWAVE: sndwave_ai( i, p_levelcontrol->hardmode); break;
	  case OBJ_MOTHER: mother_ai( i, *p_levelcontrol ); break;
	  case OBJ_FIREBALL: fireball_ai( i, p_levelcontrol->hardmode ); break;
	  case OBJ_BALL: ballandjack_ai(i); break;
	  case OBJ_JACK: ballandjack_ai(i); break;
	  case OBJ_PLATVERT: platvert_ai(i); break;
	  case OBJ_NESSIE: nessie_ai(i); break;

	  //Specials*/
	  case OBJ_RAY: ray_ai( p_object, mp_Options[OPT_FULLYAUTOMATIC].value, 1 ); break;
	  //case OBJ_AUTORAY: case OBJ_AUTORAY_V: autoray_ai(i); break;
	  //case OBJ_GOTPOINTS: gotpoints_ai(i); break;

	  //case OBJ_DEMOMSG: break;

	  default:
		  g_pLogFile->ftextOut("gamedo_enemy_ai: Object is of invalid type %d\n", p_object->m_type);
  	  break;
    }
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
