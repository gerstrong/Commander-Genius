#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// The red creatures that follow the wall (ep2)

#define WALKER_WALK     0     // walking
#define WALKER_FALLING  1     // oops, we fell off!
#define WALKER_DYING    2     // getting fried!
#define WALKER_DEAD     3     // dead walker! here's a dead walker!

#define WALKER_WALK_ANIM_TIME  50
#define WALKER_WALK_SPEED      4

#define WALKER_FALLSPDINCRATE   2
#define WALKER_MIN_FALL_SPEED  10
#define WALKER_MAX_FALL_SPEED  25

#define WALKERDIE_START_INERTIA      -10
#define WALKERDIE_INERTIA_DECREASE    2

#define WALKERPUSHAMOUNT       7

// frames
#define WALKER_WALK_LEFT       102
#define WALKER_WALK_UP         104
#define WALKER_WALK_RIGHT      106
#define WALKER_WALK_DOWN       108
#define WALKER_FRY_FRAME       110
#define WALKER_DEAD_FRAME      111

void Walker_TurnOnCansupportWhereNotKicked(int o);

// Reference to ../game.cpp
void bumpplayer(int p, int pushamt, bool solid);

void walker_ai(int o, stLevelControl levelcontrol)
{
unsigned int p;
unsigned int i=0;
int nopush;
int floor;

   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.walker.walkdir = LEFT;
     objects[o].ai.walker.state = WALKER_WALK;
     objects[o].ai.walker.walkframe = 0;
     objects[o].ai.walker.animtimer = 0;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
     objects[o].canbezapped = 1;
     objects[o].y = (objects[o].y>>CSF>>4)<<4<<CSF;
     objects[o].blockedd = 1;
     objects[o].blockedl = 0;
     objects[o].blockedr = 0;
     objects[o].blockedu = 0;
     objects[o].dead = 0;
     SetAllCanSupportPlayer(o, 1);
     for(i=0;i<numplayers;i++)
     {
       objects[o].ai.walker.kickedplayer[i] = 0;
     }
   }
   if (objects[o].ai.walker.state==WALKER_DEAD)
   {
     objects[o].hasbeenonscreen = 0;
     return;
   }

   // after kicking a player, wait until he falls beneath the walker
   // before turning cansupportplayer back on...just in case we check
   // for some other things to (when he stops falling, or if he jumps)
   for(p=0;p<numplayers;p++)
   {
     if (objects[o].ai.walker.kickedplayer[p])
     {
       if (player[p].y > objects[o].y || !player[p].pfalling || player[p].pjumping)
       {
         objects[o].cansupportplayer[p] = 1;
         objects[o].ai.walker.kickedplayer[p] = 0;
       }
     }
   }

   // push player horizontally
   if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie &&\
       player[i].psupportingobject!=o)
   {
      nopush = 0;
      /* don't push the player if he's standing on top of the walker */
      if (player[objects[o].touchedBy].pfalling)
      {
          if (objects[0].scry+sprites[0].ysize >\
             objects[o].scry+(sprites[objects[o].sprite].ysize>>1))
          {
             nopush = 1;
          }
      }
      /* don't push the player as he falls through us during a kick-off */
      else if (objects[o].ai.walker.kickedplayer[objects[o].touchedBy])
      {
        nopush = 1;
      }
      /* don't push the player as he's walking through the exit door */
      else if (levelcontrol.level_done && levelcontrol.level_finished_by == objects[o].touchedBy)
      {
        nopush = 1;
      }

      if (!nopush)
      {
         if (player[objects[o].touchedBy].x < objects[o].x)
        	 bumpplayer(objects[o].touchedBy, -WALKERPUSHAMOUNT, 1);
         else
        	 bumpplayer(objects[o].touchedBy, WALKERPUSHAMOUNT, 1);
      }
   }

   if (objects[o].canbezapped)
   {
     // if we touch a glowcell, we die!
     if (getmaptileat((objects[o].x>>CSF)+8, (objects[o].y>>CSF)+8)==TILE_GLOWCELL)
     {
       objects[o].ai.walker.state = WALKER_DYING;
       objects[o].ai.walker.dietimer = 0;
       objects[o].zapped = 0;
       objects[o].canbezapped = 0;
     }
     // die if shot
     if (objects[o].zapped)
     {
       objects[o].ai.walker.state = WALKER_DYING;
       objects[o].ai.walker.dietimer = 0;
       objects[o].zapped = 0;
       objects[o].canbezapped = 0;
       objects[o].y -= 10;
       objects[o].ai.walker.walkerdie_inertia_y = WALKERDIE_START_INERTIA;
       g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
     }
   }

   switch(objects[o].ai.walker.state)
   {
     case WALKER_DYING:
       SetAllCanSupportPlayer(o, 0);
       objects[o].sprite = WALKER_FRY_FRAME;
       objects[o].y += objects[o].ai.walker.walkerdie_inertia_y;
       if (objects[o].ai.walker.dietimer>WALKERDIE_INERTIA_DECREASE)
       {
           if (objects[o].ai.walker.walkerdie_inertia_y < WALKER_MAX_FALL_SPEED)
           {
             objects[o].ai.walker.walkerdie_inertia_y++;
           }
           objects[o].ai.walker.dietimer = 0;
       }
       else objects[o].ai.walker.dietimer++;
       if (objects[o].ai.walker.walkerdie_inertia_y >= 0 && objects[o].blockedd)
       {
          objects[o].sprite = WALKER_DEAD_FRAME;
          objects[o].ai.walker.state = WALKER_DEAD;
          objects[o].y = (objects[o].y>>CSF>>4)<<4<<CSF;
          objects[o].dead = 1;
          SetAllCanSupportPlayer(o, 0);
       }
       return;
     break;
     case WALKER_WALK:
       switch(objects[o].ai.walker.walkdir)
       {
         case LEFT:
           objects[o].sprite = WALKER_WALK_LEFT + objects[o].ai.walker.walkframe;
           if (!objects[o].blockedd)
           { // walked off the edge
              objects[o].sprite = WALKER_WALK_DOWN + objects[o].ai.walker.walkframe;
              objects[o].ai.walker.walkdir = DOWN;
              objects[o].y += (2<<CSF);
              for(i=0;i<numplayers;i++)
              {
                if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
                {
                  player[i].x -= (1<<CSF);
                  player[i].y += (2<<CSF);
                }
              }
              common_enemy_ai(o);                // recalculate blockedx's
              Walker_TurnOnCansupportWhereNotKicked(o);
           }
           else if (objects[o].blockedl)
           {
              objects[o].sprite = WALKER_WALK_UP + objects[o].ai.walker.walkframe;
              objects[o].ai.walker.walkdir = UP;
              Walker_TurnOnCansupportWhereNotKicked(o);
           }
           else
           {
              objects[o].x -= WALKER_WALK_SPEED;
              for(i=0;i<numplayers;i++)
              {
                if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
                {
                  if (!player[i].blockedl)
                  {
                    player[i].x -= WALKER_WALK_SPEED;
                  }
                }
              }
           }
           break;
         case RIGHT:
           objects[o].sprite = WALKER_WALK_RIGHT + objects[o].ai.walker.walkframe;
           if (!objects[o].blockedu)
           {
             objects[o].ai.walker.walkdir = UP;
             objects[o].sprite = WALKER_WALK_UP + objects[o].ai.walker.walkframe;
             Walker_TurnOnCansupportWhereNotKicked(o);
             objects[o].y -= (2<<CSF);
             common_enemy_ai(o);                // recalculate blockedx's
           }
           else if (objects[o].blockedr)
           {
             objects[o].ai.walker.walkdir = DOWN;
             objects[o].sprite = WALKER_WALK_DOWN + objects[o].ai.walker.walkframe;
             Walker_TurnOnCansupportWhereNotKicked(o);
           }
           else
           {
             objects[o].x += WALKER_WALK_SPEED;
           }
           break;
         case DOWN:
           objects[o].sprite = WALKER_WALK_DOWN + objects[o].ai.walker.walkframe;
           if (!objects[o].blockedr)
           {
              objects[o].ai.walker.walkdir = RIGHT;
              objects[o].sprite = WALKER_WALK_RIGHT + objects[o].ai.walker.walkframe;
              SetAllCanSupportPlayer(o, 0);
              objects[o].y += (1<<CSF);
              objects[o].x += (2<<CSF);
              common_enemy_ai(o);
           }
           else if (objects[o].blockedd)
           {
              objects[o].ai.walker.walkdir = LEFT;
              objects[o].sprite = WALKER_WALK_LEFT + objects[o].ai.walker.walkframe;
              Walker_TurnOnCansupportWhereNotKicked(o);
           }
           else
           {
              objects[o].y += WALKER_WALK_SPEED;
              for(i=0;i<numplayers;i++)
              {
                if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
                {
                  // ensure that player is not blocked by a floor (can happen
                  // in certain situations if player is hanging off the right side
                  // of the walker a bit)
                  floor = 0;
                  if (!TileProperty[getmaptileat((player[i].x>>CSF)+4, (player[i].y>>CSF)+sprites[0].ysize)][BUP])
                  { // lower-left isn't solid
                	if (TileProperty[getmaptileat((player[i].x>>CSF)+12, (player[i].y>>CSF)+sprites[0].ysize)][BUP])
                    {
                      floor = 1;
                    }
                  }
                  else floor = 1;

                  if (!floor)
                  {
                    player[i].y += WALKER_WALK_SPEED;
                  }
                }
              }
           }
           break;
         case UP:
           objects[o].sprite = WALKER_WALK_UP + objects[o].ai.walker.walkframe;
           if (!objects[o].blockedl)
           {
             objects[o].ai.walker.walkdir = LEFT;
             objects[o].sprite = WALKER_WALK_LEFT + objects[o].ai.walker.walkframe;
             Walker_TurnOnCansupportWhereNotKicked(o);
             objects[o].x -= (2<<CSF);
             objects[o].y = (((objects[o].y>>CSF>>4)<<4)+1)<<CSF;
             common_enemy_ai(o);                // recalculate blockedx's
             for(i=0;i<numplayers;i++)
             {
               if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
               {
                 player[i].x -= (2<<CSF);
                 player[i].y = (objects[o].y - (sprites[0].ysize<<CSF));
               }
             }
           }
           else if (objects[o].blockedu)
           {
             objects[o].ai.walker.walkdir = RIGHT;
             objects[o].sprite = WALKER_WALK_RIGHT + objects[o].ai.walker.walkframe;
             SetAllCanSupportPlayer(o, 0);
           }
           {
             objects[o].y -= WALKER_WALK_SPEED;
             for(i=0;i<numplayers;i++)
             {
               if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
               {
                 // kick player off if we're running him into the ceiling
                 if (player[i].blockedu)
                 {
                   objects[o].cansupportplayer[i] = 0;
                   objects[o].ai.walker.kickedplayer[i] = 1;
                 }
                 else player[i].y -= WALKER_WALK_SPEED;
               }
             }
           }
           break;
       }

       // walk animation
       if (objects[o].ai.walker.animtimer > WALKER_WALK_ANIM_TIME)
       {
         objects[o].ai.walker.walkframe ^= 1;
         objects[o].ai.walker.animtimer = 0;
       } else objects[o].ai.walker.animtimer++;
     break;
     case WALKER_FALLING:
       objects[o].sprite = WALKER_WALK_DOWN;
       if (objects[o].blockedd)
       {
         objects[o].ai.walker.walkdir = LEFT;
         objects[o].ai.walker.state = WALKER_WALK;
         objects[o].ai.walker.walkframe = 0;
         objects[o].ai.walker.animtimer = 0;
         Walker_TurnOnCansupportWhereNotKicked(o);
       }
       else
       {
         if (objects[o].ai.walker.fallinctimer > WALKER_FALLSPDINCRATE)
         {
           if (objects[o].ai.walker.fallspeed < WALKER_MAX_FALL_SPEED)
           {
             objects[o].ai.walker.fallspeed++;
           }
           objects[o].ai.walker.fallinctimer = 0;
         } else objects[o].ai.walker.fallinctimer++;

         objects[o].y += objects[o].ai.walker.fallspeed;
       }
     break;
   }

   // sometimes it's possible for a walker to fall off, for example
   // if it's walking left on a platform that has solidfall but not solidlr,
   // then turns to walk down. in this case we need to go into a falling
   // state and fall until we reach solid ground again. this keeps it from
   // freaking out and going flying across the screen.
   if (!objects[o].blockedl && !objects[o].blockedr &&
       !objects[o].blockedu && !objects[o].blockedd)
   {
       if (objects[o].ai.walker.state!=WALKER_FALLING)
       {
         objects[o].ai.walker.fallinctimer = 0;
         objects[o].ai.walker.fallspeed = WALKER_MIN_FALL_SPEED;
         objects[o].ai.walker.state = WALKER_FALLING;
         SetAllCanSupportPlayer(o, 0);
       }
   }
}

void Walker_TurnOnCansupportWhereNotKicked(int o)
{
unsigned int i;
  for(i=0;i<numplayers;i++)
  {
    if (!objects[o].ai.walker.kickedplayer[i])
    {
      objects[o].cansupportplayer[i] = 1;
    }
  }
}
