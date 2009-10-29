#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"

// raygun blast, shot by keen, and by the tank robots in ep1&2.
#include "ray.h"
#include "../spritedefines.h"

#define Sprite g_pGfxEngine->Sprite

void CObjectAI::ray_ai( CObject *p_object, bool automatic_raygun, char pShotSpeed )
{
int hitlethal;
int rayspeed;
stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
std::vector<CObject>::iterator it_obj;
  if (p_object->needinit)
  {
	p_object->ai.ray.state = RAY_STATE_FLY;
	p_object->inhibitfall = true;
	p_object->needinit = false;

    // if we shoot directly up against a wall have
    // the ZAP appear next to the wall, not in it
    if (p_object->ai.ray.direction==RIGHT && p_object->blockedr)
    {
       p_object->x = (p_object->x >> CSF) << CSF;
       if (TileProperty[mp_Map->at(p_object->x>>CSF,p_object->y>>CSF)].bleft)
    	   p_object->x--;

       p_object->ai.ray.state = RAY_STATE_SETZAPZOT;

       if (p_object->onscreen)
    	   g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, p_object->scrx);
    }
    else if (p_object->ai.ray.direction==LEFT && p_object->blockedl)
    {
       p_object->x = (p_object->x >> CSF) << CSF;
       if (TileProperty[mp_Map->at(p_object->x>>CSF,p_object->y>>CSF)].bright) p_object->x--;
       //if (tiles[getmaptileat(x>>CSF,y>>CSF)].solidl) x -= (16<<CSF);
       p_object->ai.ray.state = RAY_STATE_SETZAPZOT;
       if (p_object->onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, p_object->scrx);
    }
  }

  // shots from "fully automatic" raygun go faster
  if (p_object->sprite!=OBJ_RAY_DEFSPRITE_EP1 || !automatic_raygun)
  {
	if (!pShotSpeed)
		rayspeed = RAY_SPEED;
	else
		rayspeed = pShotSpeed;
  }
  else
	  rayspeed = RAY_AUTO_SPEED;


  switch(p_object->ai.ray.state)
  {
  case RAY_STATE_FLY:
       // test if it hit a baddie
       for( it_obj = mp_Objvect->begin() ; it_obj!=mp_Objvect->begin() ; it_obj++)
       {
         if (!it_obj->exists || &(*it_obj)==p_object ) continue;
         if (p_object->ai.ray.dontHitEnable)
         {
           if (it_obj->m_type==p_object->ai.ray.dontHit) continue;
         }

		 if (it_obj->m_type==OBJ_RAY) continue;

		 if (it_obj->canbezapped && it_obj->onscreen)
         {
            if (it_obj->hitdetect(p_object))
            {
              p_object->ai.ray.state = RAY_STATE_SETZAPZOT;
              it_obj->zapped++;
              it_obj->zapx = it_obj->x;
              it_obj->zapy = it_obj->y;
              it_obj->zapd = it_obj->ai.ray.direction;
              if (p_object->sprite==ENEMYRAY || p_object->sprite==ENEMYRAYEP2 || p_object->sprite==ENEMYRAYEP3)
            	  it_obj->zappedbyenemy = true;
              else
            	  it_obj->zappedbyenemy = false;

            }
         }
       }
       // check if ray hit keen. if canpk=0, only enemy rays can hurt keen
       if (p_object->touchPlayer)
       {
         if (mp_Player[p_object->touchedBy].pfrozentime > PFROZEN_THAW && m_Episode==1)
         {
            // shot a frozen player--melt the ice
            mp_Player[p_object->touchedBy].pfrozentime = PFROZEN_THAW;
            p_object->ai.ray.state = RAY_STATE_SETZAPZOT;
         }
         else
         {
            if (p_object->ai.ray.dontHitEnable==0 || p_object->ai.ray.dontHit!=OBJ_PLAYER)
            {
              //killplayer(p_object->touchedBy);
              p_object->ai.ray.state = RAY_STATE_SETZAPZOT;
            }
         }
       }

       if (p_object->ai.ray.direction == RIGHT)
       {
           // don't go through bonklethal tiles, even if they're not solid
           // (for the arms on mortimer's machine)
       	   if (TileProperty[mp_Map->at(((p_object->x>>(CSF-4))+Sprite[p_object->sprite]->getWidth())>>4, (p_object->y>>CSF)+1)].behaviour == 1)
       		   hitlethal = true;
           else if (TileProperty[mp_Map->at(((p_object->x>>(CSF-4))+Sprite[p_object->sprite]->getWidth())>>4, ((p_object->y>>(CSF-4))+(Sprite[p_object->sprite]->getHeight()-1))>>(CSF-4))].behaviour == 1)
        	   hitlethal = true;
           else
        	   hitlethal = false;

           if (p_object->blockedr)
           {
        	   p_object->ai.ray.state = RAY_STATE_SETZAPZOT;
        	   if (p_object->onscreen)
            	  g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, p_object->scrx);
           }

           p_object->x += rayspeed;
       }
       else if (p_object->ai.ray.direction == LEFT)
       {
           if (TileProperty[mp_Map->at((p_object->x-1)>>CSF, (p_object->y+1)>>CSF)].behaviour == 1)
        	   hitlethal = true;
           else if (TileProperty[mp_Map->at((p_object->x-1)>>CSF, ((p_object->y>>(CSF-4))+(Sprite[p_object->sprite]->getHeight()-1))>>(CSF-4))].behaviour == 1)
        	   hitlethal = true;
           else
        	   hitlethal = false;

           if (p_object->blockedl)
           {
        	   p_object->ai.ray.state = RAY_STATE_SETZAPZOT;
              if (p_object->onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, p_object->scrx);
           }

           p_object->x -= rayspeed;
       }
       else if (p_object->ai.ray.direction == DOWN)
       {
           if (p_object->blockedd || p_object->blockedu)
           {
        	   p_object->ai.ray.state = RAY_STATE_SETZAPZOT;
        	   if (p_object->onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, p_object->scrx);
           }

           p_object->y += rayspeed;
       }
  break;
  case RAY_STATE_SETZAPZOT:
	   p_object->ai.ray.state = RAY_STATE_ZAPZOT;
	   p_object->ai.ray.zapzottimer = RAY_ZAPZOT_TIME;

       if (m_Episode==1)
       {
         if (rnd()&1)
           { p_object->sprite = RAY_FRAME_ZAP_EP1; }
         else
           { p_object->sprite = RAY_FRAME_ZOT_EP1; }
       }
       else if (m_Episode==2)
       {
         if (rnd()&1)
           { p_object->sprite = RAY_FRAME_ZAP_EP2; }
         else
           { p_object->sprite = RAY_FRAME_ZOT_EP2; }
       }
       else
       {
         if (rnd()&1)
           { p_object->sprite = RAY_FRAME_ZAP_EP3; }
         else
           { p_object->sprite = RAY_FRAME_ZOT_EP3; }
       }

       if (p_object->ai.ray.direction==LEFT || p_object->ai.ray.direction==RIGHT)
       {
    	   p_object->y -= 2;
       }
       else
       {
    	   p_object->x -= 4;
       }
       // ... and fall through
  case RAY_STATE_ZAPZOT:
       if (p_object->ai.ray.zapzottimer == 0)
         { deleteObj(p_object); }
       else p_object->ai.ray.zapzottimer--;
  break;
  }
}

