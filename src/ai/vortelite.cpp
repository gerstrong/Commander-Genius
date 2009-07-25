#include "../keen.h"
#include "vortelite.h"
#include "../include/game.h"
#include "../sdl/sound/CSound.h"

// the "Vorticon Elite" enemies in ep2
#define VORTELITE_WALK          0
#define VORTELITE_JUMP          1
#define VORTELITE_ABOUTTOFIRE   2
#define VORTELITE_FIRED         3
#define VORTELITE_DYING         4
#define VORTELITE_DEAD          5

#define VORTELITE_JUMP_PROB          400
#define VORTELITE_FIRE_PROB          320

#define VORTELITE_MIN_TIME_BETWEEN_FIRE    100
#define VORTELITE_HOLD_GUN_OUT_TIME         90
#define VORTELITE_HOLD_GUN_AFTER_FIRE_TIME  75

#define VORTELITE_MIN_JUMP_HEIGHT    15
#define VORTELITE_MAX_JUMP_HEIGHT    25
#define VORTELITE_MAX_FALL_SPEED     20
#define VORTELITE_JUMP_FRICTION      6

#define VORTELITE_WALK_SPEED         7
#define VORTELITE_WALK_ANIM_TIME     50

// number of shots to kill
#define VORTELITE_HP                 4

#define VORTELITE_DIE_ANIM_TIME     180

#define VORTELITE_LOOK_ANIM_TIME     60

#define VORTELITE_WALK_LEFT_FRAME    88
#define VORTELITE_WALK_RIGHT_FRAME   92
#define VORTELITE_JUMP_RIGHT_FRAME   98
#define VORTELITE_JUMP_LEFT_FRAME    99
#define VORTELITE_DYING_FRAME        100
#define VORTELITE_DEAD_FRAME         101
#define VORTELITE_FIRE_LEFT_FRAME    96
#define VORTELITE_FIRE_RIGHT_FRAME   97

#define VORTELITE_PALETTE_FLASH_TIME  5

#define VORTELITE_TRAPPED_DIST        150

void vortelite_initiatejump(int o);

void vortelite_ai(int o, bool darkness)
{
int bonk;
int newobject;

   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.vortelite.state = VORTELITE_WALK;
     objects[o].ai.vortelite.movedir = LEFT;
     objects[o].sprite = VORTELITE_WALK_LEFT_FRAME;
     objects[o].ai.vortelite.frame = 0;
     objects[o].ai.vortelite.animtimer = 0;
     objects[o].ai.vortelite.timer = 0;
     objects[o].ai.vortelite.timesincefire = 0;
     objects[o].ai.vortelite.dist_traveled = VORTELITE_TRAPPED_DIST+1;
     objects[o].canbezapped = 1;
     objects[o].needinit = 0;
   }
   if (objects[o].ai.vortelite.state==VORTELITE_DEAD)
   {
     objects[o].hasbeenonscreen = 0;
     return;
   }

   if (objects[o].canbezapped)
   {
     // if we touch a glowcell, we die!
     if (getmaptileat((objects[o].x>>CSF)+12, (objects[o].y>>CSF)+16)==TILE_GLOWCELL)
     {
       objects[o].zapped += VORTELITE_HP;
     }

     if (objects[o].zapped >= VORTELITE_HP)
     {
       objects[o].inhibitfall = 0;
       objects[o].canbezapped = 0;
       objects[o].ai.vortelite.animtimer = 0;
       objects[o].ai.vortelite.frame = 0;
       objects[o].ai.vortelite.state = VORTELITE_DYING;
       if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, objects[o].scrx);
     }
   }
   // deadly to the touch
   if (objects[o].touchPlayer && objects[o].canbezapped)
   {
     killplayer(objects[o].touchedBy);
   }

reprocess: ;
	switch(objects[o].ai.vortelite.state)
	{
	  case VORTELITE_WALK:
		objects[o].ai.vortelite.dist_traveled++;

		if (rand()%VORTELITE_JUMP_PROB == (VORTELITE_JUMP_PROB/2) && !darkness && !objects[o].blockedu)
		{  // let's jump.
			vortelite_initiatejump(o);
			goto reprocess;
		}
		else
		{
			if (objects[o].ai.vortelite.timesincefire > VORTELITE_MIN_TIME_BETWEEN_FIRE)
			{
			   if (rand()%VORTELITE_FIRE_PROB == (VORTELITE_FIRE_PROB/2))
			   {  // let's fire
				  // usually shoot toward keen
				  if (rand()%5 != 0)
				  {
					if (objects[o].x < player[primaryplayer].x)
					{
					   objects[o].ai.vortelite.movedir = RIGHT;
					}
					else
					{
					   objects[o].ai.vortelite.movedir = LEFT;
					}
				  }
				  objects[o].ai.vortelite.timer = 0;
				  objects[o].ai.vortelite.state = VORTELITE_ABOUTTOFIRE;
			   }
			}
			else objects[o].ai.vortelite.timesincefire++;
		}

		if (objects[o].ai.vortelite.movedir==LEFT)
		{  // move left
		  objects[o].sprite = VORTELITE_WALK_LEFT_FRAME + objects[o].ai.vortelite.frame;
		  if (!objects[o].blockedl)
		  {
			objects[o].x -= VORTELITE_WALK_SPEED;
		  }
		  else
		  {
			objects[o].ai.vortelite.movedir = RIGHT;

			// if we only traveled a tiny amount before hitting a wall, we've
			// probably fallen into a small narrow area, and we need to try
			// to jump out of it
			if (objects[o].ai.vortelite.dist_traveled < VORTELITE_TRAPPED_DIST && !darkness && objects[o].blockedd)
			{
			  vortelite_initiatejump(o);
			  goto reprocess;
			}
			else objects[o].ai.vortelite.dist_traveled = 0;
		  }
		}
		else
		{  // move right
		  objects[o].sprite = VORTELITE_WALK_RIGHT_FRAME + objects[o].ai.vortelite.frame;
		  if (!objects[o].blockedr)
		  {
			objects[o].x += VORTELITE_WALK_SPEED;
		  }
		  else
		  {
			objects[o].ai.vortelite.movedir = LEFT;

			// if we only traveled a tiny amount before hitting a wall, we've
			// probably fallen into a small narrow area, and we need to try
			// to jump out of it
			if (objects[o].ai.vortelite.dist_traveled < VORTELITE_TRAPPED_DIST && !darkness && objects[o].blockedd)
			{
			  vortelite_initiatejump(o);
			  goto reprocess;
			}
			else objects[o].ai.vortelite.dist_traveled = 0;
		  }
		}

		// walk animation
		if (objects[o].ai.vortelite.animtimer > VORTELITE_WALK_ANIM_TIME)
		{
		  if (objects[o].ai.vortelite.frame>=3) objects[o].ai.vortelite.frame=0;
									  else objects[o].ai.vortelite.frame++;
		  objects[o].ai.vortelite.animtimer = 0;
		} else objects[o].ai.vortelite.animtimer++;
	 break;
	 case VORTELITE_JUMP:
		if (objects[o].ai.vortelite.movedir == RIGHT)
		  { if (!objects[o].blockedr) objects[o].x += VORTELITE_WALK_SPEED; }
		else
		  { if (!objects[o].blockedl) objects[o].x -= VORTELITE_WALK_SPEED; }

		   if (objects[o].ai.vortelite.inertiay>0 && objects[o].blockedd)
		   {  // the bear has landed
			  objects[o].inhibitfall = 0;
			  objects[o].ai.vortelite.state = VORTELITE_WALK;
			  goto reprocess;
		   }
		   // check if the bear has bonked into a ceiling, if so,
		   // immediately terminate the jump
			  bonk = 0;
			  if (TileProperty[getmaptileat((objects[o].x>>CSF)+1, (objects[o].y>>CSF))][BDOWN]) bonk = 1;
			  else if (TileProperty[getmaptileat((objects[o].x>>CSF)+16, (objects[o].y>>CSF))][BDOWN]) bonk = 1;
			  else if (TileProperty[getmaptileat((objects[o].x>>CSF)+23, (objects[o].y>>CSF))][BDOWN]) bonk = 1;
			  if (bonk && objects[o].ai.vortelite.inertiay < 0)
			  {
				objects[o].ai.vortelite.inertiay = 0;
			  }

			  // apply Y inertia
			  objects[o].y += objects[o].ai.vortelite.inertiay;

			  if (objects[o].ai.vortelite.timer > VORTELITE_JUMP_FRICTION)
			  { // slowly decrease upgoing rate
				if (objects[o].ai.vortelite.inertiay<VORTELITE_MAX_FALL_SPEED)
				{
				  objects[o].ai.vortelite.inertiay++;
				}
				objects[o].ai.vortelite.timer = 0;
			  } else objects[o].ai.vortelite.timer++;
		break;
	 case VORTELITE_ABOUTTOFIRE:
		if (objects[o].ai.vortelite.movedir==RIGHT)
		  { objects[o].sprite = VORTELITE_FIRE_RIGHT_FRAME; }
		else
		  { objects[o].sprite = VORTELITE_FIRE_LEFT_FRAME; }
		if (objects[o].ai.vortelite.timer > VORTELITE_HOLD_GUN_OUT_TIME)
		{
			objects[o].ai.vortelite.timer = 0;
			objects[o].ai.vortelite.state = VORTELITE_FIRED;
			if (objects[o].ai.vortelite.movedir==RIGHT)
			{
			  newobject = spawn_object(objects[o].x+(sprites[VORTELITE_FIRE_RIGHT_FRAME].xsize<<CSF), objects[o].y+(9<<CSF), OBJ_RAY);
			  objects[newobject].ai.ray.direction = RIGHT;
			}
			else
			{
			  newobject = spawn_object(objects[o].x-(sprites[ENEMYRAYEP2].xsize<<CSF), objects[o].y+(9<<CSF), OBJ_RAY);
			  objects[newobject].ai.ray.direction = LEFT;
			}
			objects[newobject].sprite = ENEMYRAYEP2;
			// don't shoot other vorticon elite
			objects[newobject].ai.ray.dontHitEnable = 1;
			objects[newobject].ai.ray.dontHit = OBJ_VORTELITE;

			if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_KEEN_FIRE, PLAY_NOW, objects[o].scrx);
		}
		else objects[o].ai.vortelite.timer++;
		break;
	 case VORTELITE_FIRED:
		if (objects[o].ai.vortelite.movedir==RIGHT)
		  { objects[o].sprite = VORTELITE_FIRE_RIGHT_FRAME; }
		else
		  { objects[o].sprite = VORTELITE_FIRE_LEFT_FRAME; }

		if (objects[o].ai.vortelite.timer > VORTELITE_HOLD_GUN_AFTER_FIRE_TIME)
		{
			objects[o].ai.vortelite.timer = 0;
			objects[o].ai.vortelite.frame = 0;
			objects[o].ai.vortelite.timesincefire = 0;
			objects[o].ai.vortelite.state = VORTELITE_WALK;
			// head toward keen
			if (objects[o].x < player[primaryplayer].x)
			{
			  objects[o].ai.vortelite.movedir = RIGHT;
			}
			else
			{
			  objects[o].ai.vortelite.movedir = LEFT;
			}
		}
		else objects[o].ai.vortelite.timer++;
		break;
	 case VORTELITE_DYING:
	 objects[o].sprite = VORTELITE_DYING_FRAME;
		if (objects[o].ai.vortelite.animtimer > VORTELITE_DIE_ANIM_TIME)
		{
		  objects[o].sprite = VORTELITE_DEAD_FRAME;
		  objects[o].ai.vortelite.state = VORTELITE_DEAD;
		}
		else
		{
		  objects[o].ai.vortelite.animtimer++;
		}
	 break;
	 default: break;
	}
}

void vortelite_initiatejump(int o)
{
           if (objects[o].ai.vortelite.state==VORTELITE_JUMP) return;

           objects[o].ai.vortelite.frame = 0;
           objects[o].ai.vortelite.animtimer = 0;
           objects[o].ai.vortelite.inertiay = \
             -((rand()%(VORTELITE_MAX_JUMP_HEIGHT-VORTELITE_MIN_JUMP_HEIGHT))+VORTELITE_MIN_JUMP_HEIGHT);

           if (objects[o].ai.vortelite.movedir==RIGHT)
           {
             objects[o].sprite = VORTELITE_JUMP_RIGHT_FRAME;
           }
           else
           {
             objects[o].sprite = VORTELITE_JUMP_LEFT_FRAME;
           }

           objects[o].inhibitfall = 1;
           objects[o].ai.vortelite.state = VORTELITE_JUMP;
}
