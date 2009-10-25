#include "../../common/CObject.h"
#include "../../sdl/sound/CSound.h"

// raygun blast, shot by keen, and by the tank robots in ep1&2.
#include "ray.h"
#include "../../game.h"

#define Sprite g_pGfxEngine->Sprite

void CObject::ray_ai( int episode, bool automatic_raygun, char pShotSpeed )
{
/*int i;
int hitlethal;
int rayspeed;
  if (needinit)
  {
    ai.ray.state = RAY_STATE_FLY;
    inhibitfall = 1;
    needinit = 0;

    // if we shoot directly up against a wall have
    // the ZAP appear next to the wall, not in it
    if (ai.ray.direction==RIGHT && blockedr)
    {
       x = (x >> 4 >> CSF) << 4 << CSF;
       if (TileProperty[getmaptileat(x>>CSF,y>>CSF)][BLEFT])
    	   x -= (16<<CSF);

       ai.ray.state = RAY_STATE_SETZAPZOT;

       if (onscreen)
    	   g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
    }
    else if (ai.ray.direction==LEFT && blockedl)
    {
       x = (x >> 4 >> CSF) << 4 << CSF;
       if (TileProperty[getmaptileat(x>>CSF,y>>CSF)][BRIGHT]) x += (16<<CSF);
       //if (tiles[getmaptileat(x>>CSF,y>>CSF)].solidl) x -= (16<<CSF);
       ai.ray.state = RAY_STATE_SETZAPZOT;
       if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
    }
  }

  // shots from "fully automatic" raygun go faster
  if (sprite!=objdefsprites[OBJ_RAY] || !automatic_raygun)
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
       for(i=1 ; i<highest_objslot ; i++)
       {
         if (!objects[i].exists || i==o) continue;
         if (ai.ray.dontHitEnable)
         {
           if (objects[i].type==ai.ray.dontHit) continue;
         }

		 if (objects[i].type==OBJ_RAY) continue;

		 if (objects[i].canbezapped && objects[i].onscreen)
         {
            if (hitdetect(i, o))
            {
              ai.ray.state = RAY_STATE_SETZAPZOT;
              objects[i].zapped++;
              objects[i].zapx = x;
              objects[i].zapy = y;
              objects[i].zapd = ai.ray.direction;
              if (sprite==ENEMYRAY || sprite==ENEMYRAYEP2 || sprite==ENEMYRAYEP3)
					objects[i].zappedbyenemy = 1;
              else
					objects[i].zappedbyenemy = 0;

            }
         }
       }
       // check if ray hit keen. if canpk=0, only enemy rays can hurt keen
       if (touchPlayer)
       {
         if (player[touchedBy].pfrozentime > PFROZEN_THAW && episode==1)
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
       	   if (TileProperty[getmaptileat((x>>CSF)+Sprite[sprite]->getWidth(), (y>>CSF)+1)][BEHAVIOR] == 1)
              hitlethal = 1;
           else if (TileProperty[getmaptileat((x>>CSF)+Sprite[sprite]->getWidth(), (y>>CSF)+(Sprite[sprite]->getHeight()-1))][BEHAVIOR] == 1)
              hitlethal = 1;
           else
        	  hitlethal = 0;

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
           if (TileProperty[getmaptileat((x>>CSF)-1, (y>>CSF)+1)][BEHAVIOR] == 1)
              hitlethal = 1;
           else if (TileProperty[getmaptileat((x>>CSF)-1, (y>>CSF)+(Sprite[sprite]->getHeight()-1))][BEHAVIOR] == 1)
              hitlethal = 1;
           else
        	  hitlethal = 0;

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
         { delete_object(o); }
       else ai.ray.zapzottimer--;
  break;
  }*/
}

