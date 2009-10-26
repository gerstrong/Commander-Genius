#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"

// raygun blast, shot by keen, and by the tank robots in ep1&2.
#include "ray.h"
#include "../spritedefines.h"

#define Sprite g_pGfxEngine->Sprite

void CObjectAI::ray_ai( bool automatic_raygun, char pShotSpeed )
{
/*std::vector<CObject>::iterator p_object;
int i;
int hitlethal;
int rayspeed;
stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
  if (needinit)
  {
    ai.ray.state = RAY_STATE_FLY;
    inhibitfall = true;
    needinit = false;

    // if we shoot directly up against a wall have
    // the ZAP appear next to the wall, not in it
    if (ai.ray.direction==RIGHT && blockedr)
    {
       x = (x >> CSF) << CSF;
       if (TileProperty[p_map->at(x>>CSF,y>>CSF)].bleft)
    	   x -= (16<<CSF);

       ai.ray.state = RAY_STATE_SETZAPZOT;

       if (onscreen)
    	   g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
    }
    else if (ai.ray.direction==LEFT && blockedl)
    {
       x = (x >> CSF) << CSF;
       if (TileProperty[p_map->at(x>>CSF,y>>CSF)].bright) x -= (16<<CSF);
       //if (tiles[getmaptileat(x>>CSF,y>>CSF)].solidl) x -= (16<<CSF);
       ai.ray.state = RAY_STATE_SETZAPZOT;
       if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
    }
  }

  // shots from "fully automatic" raygun go faster
  if (sprite!=OBJ_RAY_DEFSPRITE_EP1 || !automatic_raygun)
  {
	if (!pShotSpeed)
		rayspeed = RAY_SPEED;
	else
		rayspeed = pShotSpeed;
  }
  else
	rayspeed = RAY_AUTO_SPEED;


  switch(ai.ray.state)
  {
  case RAY_STATE_FLY:
       // test if it hit a baddie
       for( p_object=p_objvect->begin() ; p_object!=p_objvect->end() ; p_object++)
       {
         if (!p_object->exists || p_object==this ) continue;
         if (ai.ray.dontHitEnable)
         {
           if (p_object->type==ai.ray.dontHit) continue;
         }

		 if (p_object->type==OBJ_RAY) continue;

		 if (p_object->canbezapped && p_object->onscreen)
         {
            if (hitdetect(i, o))
            {
              ai.ray.state = RAY_STATE_SETZAPZOT;
              p_object->zapped++;
              p_object->zapx = x;
              p_object->zapy = y;
              p_object->zapd = ai.ray.direction;
              if (sprite==ENEMYRAY || sprite==ENEMYRAYEP2 || sprite==ENEMYRAYEP3)
					p_object->zappedbyenemy = 1;
              else
					p_object->zappedbyenemy = 0;

            }
         }
       }
       // check if ray hit keen. if canpk=0, only enemy rays can hurt keen
       if (touchPlayer)
       {
         if (mp_Player[touchedBy].pfrozentime > PFROZEN_THAW && episode==1)
         {
            // shot a frozen player--melt the ice
            player[touchedBy].pfrozentime = PFROZEN_THAW;
            ai.ray.state = RAY_STATE_SETZAPZOT;
         }
         else
         {
            if (ai.ray.dontHitEnable==0 || ai.ray.dontHit!=OBJ_PLAYER)
            {
              killplayer(touchedBy);
              ai.ray.state = RAY_STATE_SETZAPZOT;
            }
         }
       }

       if (ai.ray.direction == RIGHT)
       {
           // don't go through bonklethal tiles, even if they're not solid
           // (for the arms on mortimer's machine)
       	   if (TileProperty[p_map->at(((x>>(CSF-4))+Sprite[sprite]->getWidth())>>4, (y>>CSF)+1)].behaviour == 1)
              hitlethal = true;
           else if (TileProperty[p_map->at(((x>>(CSF-4))+Sprite[sprite]->getWidth())>>4, ((y>>(CSF-4))+(Sprite[sprite]->getHeight()-1))>>4)].behaviour == 1)
              hitlethal = true;
           else
        	  hitlethal = false;

           if (blockedr)
           {
              ai.ray.state = RAY_STATE_SETZAPZOT;
              if (onscreen)
            	  g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
           }

           x += rayspeed;
       }
       else if (ai.ray.direction == LEFT)
       {
           if (TileProperty[p_map->at((x-1)>>CSF, (y+1)>>CSF)].behaviour == 1)
              hitlethal = true;
           else if (TileProperty[p_map->at((x-1)>>CSF, ((y>>(CSF-4))+(Sprite[sprite]->getHeight()-1))>>4)].behaviour == 1)
              hitlethal = true;
           else
        	  hitlethal = false;

           if (blockedl)
           {
              ai.ray.state = RAY_STATE_SETZAPZOT;
              if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
           }

           x -= rayspeed;
       }
       else if (ai.ray.direction == DOWN)
       {
           if (blockedd || blockedu)
           {
              ai.ray.state = RAY_STATE_SETZAPZOT;
              if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
           }

           y += rayspeed;
       }
  break;
  case RAY_STATE_SETZAPZOT:
       ai.ray.state = RAY_STATE_ZAPZOT;
       ai.ray.zapzottimer = RAY_ZAPZOT_TIME;

       if (episode==1)
       {
         if (rnd()&1)
           { sprite = RAY_FRAME_ZAP_EP1; }
         else
           { sprite = RAY_FRAME_ZOT_EP1; }
       }
       else if (episode==2)
       {
         if (rnd()&1)
           { sprite = RAY_FRAME_ZAP_EP2; }
         else
           { sprite = RAY_FRAME_ZOT_EP2; }
       }
       else
       {
         if (rnd()&1)
           { sprite = RAY_FRAME_ZAP_EP3; }
         else
           { sprite = RAY_FRAME_ZOT_EP3; }
       }

       if (ai.ray.direction==LEFT || ai.ray.direction==RIGHT)
       {
         y -= (2<<CSF);
       }
       else
       {
         x -= (4<<CSF);
       }
       // ... and fall through
  case RAY_STATE_ZAPZOT:
       if (!ai.ray.zapzottimer)
         { delete_object(this); }
       else ai.ray.zapzottimer--;
  break;
  }*/
}

