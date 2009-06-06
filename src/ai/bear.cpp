#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

#include "../sdl/sound/CSound.h"

// the "bear" enemies in ep2
// (I think the actual name for them is Vorticon Elite)
#define BEAR_WALK          0
#define BEAR_JUMP          1
#define BEAR_ABOUTTOFIRE   2
#define BEAR_FIRED         3
#define BEAR_DYING         4
#define BEAR_DEAD          5

#define BEAR_JUMP_PROB          450
#define BEAR_FIRE_PROB          400

#define BEAR_MIN_TIME_BETWEEN_FIRE    100
#define BEAR_HOLD_GUN_OUT_TIME         80
#define BEAR_HOLD_GUN_AFTER_FIRE_TIME  70

#define BEAR_MIN_JUMP_HEIGHT    15
#define BEAR_MAX_JUMP_HEIGHT    25
#define BEAR_MAX_FALL_SPEED     20
#define BEAR_JUMP_FRICTION      5
#define BEAR_JUMP_SPEED         0

#define BEAR_WALK_SPEED         5
#define BEAR_WALK_ANIM_TIME     60

// number of shots to kill
#define BEAR_HP                 4

#define BEAR_DIE_ANIM_TIME     180

#define BEAR_LOOK_ANIM_TIME     60

#define BEAR_WALK_LEFT_FRAME    88
#define BEAR_WALK_RIGHT_FRAME   92
#define BEAR_JUMP_RIGHT_FRAME   98
#define BEAR_JUMP_LEFT_FRAME    99
#define BEAR_DYING_FRAME        100
#define BEAR_DEAD_FRAME         101
#define BEAR_FIRE_LEFT_FRAME    96
#define BEAR_FIRE_RIGHT_FRAME   97

#define BEAR_PALETTE_FLASH_TIME  5

#define BEAR_TRAPPED_DIST        150

int bear_walk_speed;

void bear_initiatejump(int o);

void bear_ai(int o, stLevelControl levelcontrol, stCloneKeenPlus *pCKP)
{
int bonk;
int newobject;

   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.bear.state = BEAR_WALK;
     objects[o].ai.bear.movedir = LEFT;
     objects[o].sprite = BEAR_WALK_LEFT_FRAME;
     objects[o].ai.bear.frame = 0;
     objects[o].ai.bear.animtimer = 0;
     objects[o].ai.bear.timer = 0;
     objects[o].ai.bear.timesincefire = 0;
     objects[o].ai.bear.dist_traveled = BEAR_TRAPPED_DIST+1;
     objects[o].ai.bear.running = 0;
     objects[o].canbezapped = 1;
     objects[o].needinit = 0;
   }
   if (objects[o].ai.bear.state==BEAR_DEAD)
   {
     objects[o].hasbeenonscreen = 0;
     return;
   }

   if (objects[o].canbezapped)
   {
     // if we touch a glowcell, we die!
     if (getmaptileat((objects[o].x>>CSF)+12, (objects[o].y>>CSF)+16)==TILE_GLOWCELL)
     {
       objects[o].zapped += BEAR_HP;
     }

     if (objects[o].zapped >= BEAR_HP)
     {
       objects[o].inhibitfall = 0;
       objects[o].canbezapped = 0;
       objects[o].ai.bear.animtimer = 0;
       objects[o].ai.bear.frame = 0;
       objects[o].ai.bear.state = BEAR_DYING;
       if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, objects[o].scrx);
     }
   }
   // deadly to the touch
   if (objects[o].touchPlayer && objects[o].canbezapped)
   {
     killplayer(objects[o].touchedBy, pCKP);
   }

bear_reprocess: ;
   switch(objects[o].ai.bear.state)
   {
     case BEAR_WALK:
       objects[o].ai.bear.dist_traveled++;

       if (rand()%BEAR_JUMP_PROB == (BEAR_JUMP_PROB/2) && !levelcontrol.dark && !objects[o].blockedu)
       {  // let's jump.
           bear_initiatejump(o);
           goto bear_reprocess;
       }
       else
       {
           if (objects[o].ai.bear.timesincefire > BEAR_MIN_TIME_BETWEEN_FIRE)
           {
              if (rand()%BEAR_FIRE_PROB == (BEAR_FIRE_PROB/2))
              {  // let's fire
                 // usually shoot toward keen
                 if (rand()%5 != 0)
                 {
                   if (objects[o].x < (objects[player[primaryplayer].useObject].x))
                   {
                      objects[o].ai.bear.movedir = RIGHT;
                      objects[o].ai.bear.running = 2;
                   }
                   else
                   {
                      objects[o].ai.bear.movedir = LEFT;
                      objects[o].ai.bear.running = 2;
                   }
                 }
                 objects[o].ai.bear.timer = 0;
                 objects[o].ai.bear.state = BEAR_ABOUTTOFIRE;
              }
           }
           else objects[o].ai.bear.timesincefire++;
       }

       if (objects[o].ai.bear.movedir==LEFT)
       {  // move left
         objects[o].sprite = BEAR_WALK_LEFT_FRAME + objects[o].ai.bear.frame;
         if (!objects[o].blockedl)
         {
           objects[o].x -= ((BEAR_WALK_SPEED)*(1 + objects[o].ai.bear.running));
           objects[o].ai.bear.running = 0;
         }
         else
         {
           objects[o].ai.bear.movedir = RIGHT;

           // if we only traveled a tiny amount before hitting a wall, we've
           // probably fallen into a small narrow area, and we need to try
           // to jump out of it
           if (objects[o].ai.bear.dist_traveled < BEAR_TRAPPED_DIST && !levelcontrol.dark && objects[o].blockedd)
           {
             bear_initiatejump(o);
             goto bear_reprocess;
           }
           else objects[o].ai.bear.dist_traveled = 0;
         }
       }
       else
       {  // move right
         objects[o].sprite = BEAR_WALK_RIGHT_FRAME + objects[o].ai.bear.frame;
         if (!objects[o].blockedr)
         {
           objects[o].x += ((BEAR_WALK_SPEED)*(1 + objects[o].ai.bear.running));
           objects[o].ai.bear.running = 0;
         }
         else
         {
           objects[o].ai.bear.movedir = LEFT;

           // if we only traveled a tiny amount before hitting a wall, we've
           // probably fallen into a small narrow area, and we need to try
           // to jump out of it
           if (objects[o].ai.bear.dist_traveled < BEAR_TRAPPED_DIST && !levelcontrol.dark && objects[o].blockedd)
           {
             bear_initiatejump(o);
             goto bear_reprocess;
           }
           else objects[o].ai.bear.dist_traveled = 0;
         }
       }

       // walk animation
       if (objects[o].ai.bear.animtimer > BEAR_WALK_ANIM_TIME)
       {
         if (objects[o].ai.bear.frame>=3) objects[o].ai.bear.frame=0;
                                     else objects[o].ai.bear.frame++;
         objects[o].ai.bear.animtimer = 0;
       } else objects[o].ai.bear.animtimer++;
    break;
    case BEAR_JUMP:
       if (objects[o].ai.bear.movedir == RIGHT)
         { if (!objects[o].blockedr) objects[o].x += ((BEAR_WALK_SPEED)*(1 + objects[o].ai.bear.running)); }
       else
         { if (!objects[o].blockedl) objects[o].x -= ((BEAR_WALK_SPEED)*(1 + objects[o].ai.bear.running)); }

          if (objects[o].ai.bear.inertiay>0 && objects[o].blockedd)
          {  // the bear has landed
             objects[o].inhibitfall = 0;
             objects[o].ai.bear.state = BEAR_WALK;
             goto bear_reprocess;
          }
          // check if the bear has bonked into a ceiling, if so,
          // immediately terminate the jump
          bonk = 0;
          if (TileProperty[getmaptileat((objects[o].x>>CSF)+1, (objects[o].y>>CSF))][BDOWN]) bonk = 1;
          else if (TileProperty[getmaptileat((objects[o].x>>CSF)+16, (objects[o].y>>CSF))][BDOWN]) bonk = 1;
          else if (TileProperty[getmaptileat((objects[o].x>>CSF)+23, (objects[o].y>>CSF))][BDOWN]) bonk = 1;

          /*
          if (tiles[getmaptileat((objects[o].x>>CSF)+1, (objects[o].y>>CSF))].solidceil) bonk = 1;
          else if (tiles[getmaptileat((objects[o].x>>CSF)+16, (objects[o].y>>CSF))].solidceil) bonk = 1;
          else if (tiles[getmaptileat((objects[o].x>>CSF)+23, (objects[o].y>>CSF))].solidceil) bonk = 1;
          */
          if (bonk && objects[o].ai.bear.inertiay < 0)
          {
            objects[o].ai.bear.inertiay = 0;
          }

          // apply Y inertia
          objects[o].y += objects[o].ai.bear.inertiay;

          if (objects[o].ai.bear.timer > BEAR_JUMP_FRICTION)
          { // slowly decrease upgoing rate
            if (objects[o].ai.bear.inertiay<BEAR_MAX_FALL_SPEED)
            {
              objects[o].ai.bear.inertiay++;
            }
            objects[o].ai.bear.timer = 0;
          } else objects[o].ai.bear.timer++;
    break;
    case BEAR_ABOUTTOFIRE:
       if (objects[o].ai.bear.movedir==RIGHT)
         { objects[o].sprite = BEAR_FIRE_RIGHT_FRAME; }
       else
         { objects[o].sprite = BEAR_FIRE_LEFT_FRAME; }
       if (objects[o].ai.bear.timer > BEAR_HOLD_GUN_OUT_TIME)
       {
           objects[o].ai.bear.timer = 0;
           objects[o].ai.bear.state = BEAR_FIRED;
           if (objects[o].ai.bear.movedir==RIGHT)
           {
             newobject = spawn_object(objects[o].x+(sprites[BEAR_FIRE_RIGHT_FRAME].xsize<<CSF), objects[o].y+(9<<CSF), OBJ_RAY);
             objects[newobject].ai.ray.direction = RIGHT;
           }
           else
           {
             newobject = spawn_object(objects[o].x-(sprites[ENEMYRAYEP2].xsize<<CSF), objects[o].y+(9<<CSF), OBJ_RAY);
             objects[newobject].ai.ray.direction = LEFT;
           }
           objects[newobject].sprite = ENEMYRAYEP2;
           // don't shoot other bears
           objects[newobject].ai.ray.dontHitEnable = 1;
           objects[newobject].ai.ray.dontHit = OBJ_BEAR;

           if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_KEEN_FIRE, PLAY_NOW, objects[o].scrx);
       }
       else objects[o].ai.bear.timer++;
       break;
    case BEAR_FIRED:
       if (objects[o].ai.bear.movedir==RIGHT)
         { objects[o].sprite = BEAR_FIRE_RIGHT_FRAME; }
       else
         { objects[o].sprite = BEAR_FIRE_LEFT_FRAME; }

       if (objects[o].ai.bear.timer > BEAR_HOLD_GUN_AFTER_FIRE_TIME)
       {
           objects[o].ai.bear.timer = 0;
           objects[o].ai.bear.frame = 0;
           objects[o].ai.bear.timesincefire = 0;
           objects[o].ai.bear.state = BEAR_WALK;
           // head toward keen
           if (objects[o].x < player[primaryplayer].x)
           {
             objects[o].ai.bear.movedir = RIGHT;
           }
           else
           {
             objects[o].ai.bear.movedir = LEFT;
           }
       }
       else objects[o].ai.bear.timer++;
       break;
    case BEAR_DYING:
    objects[o].sprite = BEAR_DYING_FRAME;
       if (objects[o].ai.bear.animtimer > BEAR_DIE_ANIM_TIME)
       {
         objects[o].sprite = BEAR_DEAD_FRAME;
         objects[o].ai.bear.state = BEAR_DEAD;
       }
       else
       {
         objects[o].ai.bear.animtimer++;
       }
    break;
    default: break;
   }
}


void bear_initiatejump(int o)
{
           if (objects[o].ai.bear.state==BEAR_JUMP) return;

           objects[o].ai.bear.frame = 0;
           objects[o].ai.bear.animtimer = 0;
           objects[o].ai.bear.inertiay = \
             -((rand()%(BEAR_MAX_JUMP_HEIGHT-BEAR_MIN_JUMP_HEIGHT))+BEAR_MIN_JUMP_HEIGHT);

           if (objects[o].ai.bear.movedir==RIGHT)
           {
             objects[o].sprite = BEAR_JUMP_RIGHT_FRAME;
           }
           else
           {
             objects[o].sprite = BEAR_JUMP_LEFT_FRAME;
           }

           objects[o].inhibitfall = 1;
           objects[o].ai.bear.state = BEAR_JUMP;
}
