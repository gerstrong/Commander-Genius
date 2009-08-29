#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// The red creatures that follow the wall (ep2)

#define SCRUB_WALK     0     // walking
#define SCRUB_FALLING  1     // oops, we fell off!
#define SCRUB_DYING    2     // getting fried!
#define SCRUB_DEAD     3     // dead scrub! here's a dead scrub!

#define SCRUB_WALK_ANIM_TIME  50
#define SCRUB_WALK_SPEED      4

#define SCRUB_FALLSPDINCRATE   2
#define SCRUB_MIN_FALL_SPEED  10
#define SCRUB_MAX_FALL_SPEED  25

#define SCRUBDIE_START_INERTIA      -10
#define SCRUBDIE_INERTIA_DECREASE    2

#define SCRUBPUSHAMOUNT       7

// frames
#define SCRUB_WALK_LEFT       102
#define SCRUB_WALK_UP         104
#define SCRUB_WALK_RIGHT      106
#define SCRUB_WALK_DOWN       108
#define SCRUB_FRY_FRAME       110
#define SCRUB_DEAD_FRAME      111

void Scrub_TurnOnCansupportWhereNotKicked(int o);

// Reference to ../game.cpp
void bumpplayer(int p, int pushamt, bool solid);

void scrub_ai(int o, stLevelControl levelcontrol)
{
unsigned int p;
unsigned int i=0;
int nopush;
int floor;

   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.scrub.walkdir = LEFT;
     objects[o].ai.scrub.state = SCRUB_WALK;
     objects[o].ai.scrub.walkframe = 0;
     objects[o].ai.scrub.animtimer = 0;
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
       objects[o].ai.scrub.kickedplayer[i] = 0;
     }
   }
   if (objects[o].ai.scrub.state==SCRUB_DEAD)
   {
     objects[o].hasbeenonscreen = 0;
     return;
   }

   // after kicking a player, wait until he falls beneath the scrub
   // before turning cansupportplayer back on...just in case we check
   // for some other things to (when he stops falling, or if he jumps)
   for(p=0;p<numplayers;p++)
   {
     if (objects[o].ai.scrub.kickedplayer[p])
     {
       if (player[p].y > objects[o].y || !player[p].pfalling || player[p].pjumping)
       {
         objects[o].cansupportplayer[p] = 1;
         objects[o].ai.scrub.kickedplayer[p] = 0;
       }
     }
   }

   // push player horizontally
   if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie &&\
       player[i].psupportingobject!=o)
   {
      nopush = 0;
      /* don't push the player if he's standing on top of the scrub */
      if (player[objects[o].touchedBy].pfalling)
      {
          if (objects[0].scry+sprites[0].ysize >\
             objects[o].scry+(sprites[objects[o].sprite].ysize>>1))
          {
             nopush = 1;
          }
      }
      /* don't push the player as he falls through us during a kick-off */
      else if (objects[o].ai.scrub.kickedplayer[objects[o].touchedBy])
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
        	 bumpplayer(objects[o].touchedBy, -SCRUBPUSHAMOUNT, 1);
         else
        	 bumpplayer(objects[o].touchedBy, SCRUBPUSHAMOUNT, 1);
      }
   }

   if (objects[o].canbezapped)
   {
     // if we touch a glowcell, we die!
     if (getmaptileat((objects[o].x>>CSF)+8, (objects[o].y>>CSF)+8)==TILE_GLOWCELL)
     {
       objects[o].ai.scrub.state = SCRUB_DYING;
       objects[o].ai.scrub.dietimer = 0;
       objects[o].zapped = 0;
       objects[o].canbezapped = 0;
     }
     // die if shot
     if (objects[o].zapped)
     {
       objects[o].ai.scrub.state = SCRUB_DYING;
       objects[o].ai.scrub.dietimer = 0;
       objects[o].zapped = 0;
       objects[o].canbezapped = 0;
       objects[o].y -= 10;
       objects[o].ai.scrub.scrubdie_inertia_y = SCRUBDIE_START_INERTIA;
       g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
     }
   }

   switch(objects[o].ai.scrub.state)
   {
     case SCRUB_DYING:
       SetAllCanSupportPlayer(o, 0);
       objects[o].sprite = SCRUB_FRY_FRAME;
       objects[o].y += objects[o].ai.scrub.scrubdie_inertia_y;
       if (objects[o].ai.scrub.dietimer>SCRUBDIE_INERTIA_DECREASE)
       {
           if (objects[o].ai.scrub.scrubdie_inertia_y < SCRUB_MAX_FALL_SPEED)
           {
             objects[o].ai.scrub.scrubdie_inertia_y++;
           }
           objects[o].ai.scrub.dietimer = 0;
       }
       else objects[o].ai.scrub.dietimer++;
       if (objects[o].ai.scrub.scrubdie_inertia_y >= 0 && objects[o].blockedd)
       {
          objects[o].sprite = SCRUB_DEAD_FRAME;
          objects[o].ai.scrub.state = SCRUB_DEAD;
          objects[o].y = (objects[o].y>>CSF>>4)<<4<<CSF;
          objects[o].dead = 1;
          SetAllCanSupportPlayer(o, 0);
       }
       return;
     break;
     case SCRUB_WALK:
       switch(objects[o].ai.scrub.walkdir)
       {
         case LEFT:
           objects[o].sprite = SCRUB_WALK_LEFT + objects[o].ai.scrub.walkframe;
           if (!objects[o].blockedd)
           { // walked off the edge
              objects[o].sprite = SCRUB_WALK_DOWN + objects[o].ai.scrub.walkframe;
              objects[o].ai.scrub.walkdir = DOWN;
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
              Scrub_TurnOnCansupportWhereNotKicked(o);
           }
           else if (objects[o].blockedl)
           {
              objects[o].sprite = SCRUB_WALK_UP + objects[o].ai.scrub.walkframe;
              objects[o].ai.scrub.walkdir = UP;
              Scrub_TurnOnCansupportWhereNotKicked(o);
           }
           else
           {
              objects[o].x -= SCRUB_WALK_SPEED;
              for(i=0;i<numplayers;i++)
              {
                if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
                {
                  if (!player[i].blockedl)
                  {
                    player[i].x -= SCRUB_WALK_SPEED;
                  }
                }
              }
           }
           break;
         case RIGHT:
           objects[o].sprite = SCRUB_WALK_RIGHT + objects[o].ai.scrub.walkframe;
           if (!objects[o].blockedu)
           {
             objects[o].ai.scrub.walkdir = UP;
             objects[o].sprite = SCRUB_WALK_UP + objects[o].ai.scrub.walkframe;
             Scrub_TurnOnCansupportWhereNotKicked(o);
             objects[o].y -= (2<<CSF);
             common_enemy_ai(o);                // recalculate blockedx's
           }
           else if (objects[o].blockedr)
           {
             objects[o].ai.scrub.walkdir = DOWN;
             objects[o].sprite = SCRUB_WALK_DOWN + objects[o].ai.scrub.walkframe;
             Scrub_TurnOnCansupportWhereNotKicked(o);
           }
           else
           {
             objects[o].x += SCRUB_WALK_SPEED;
           }
           break;
         case DOWN:
           objects[o].sprite = SCRUB_WALK_DOWN + objects[o].ai.scrub.walkframe;
           if (!objects[o].blockedr)
           {
              objects[o].ai.scrub.walkdir = RIGHT;
              objects[o].sprite = SCRUB_WALK_RIGHT + objects[o].ai.scrub.walkframe;
              SetAllCanSupportPlayer(o, 0);
              objects[o].y += (1<<CSF);
              objects[o].x += (2<<CSF);
              common_enemy_ai(o);
           }
           else if (objects[o].blockedd)
           {
              objects[o].ai.scrub.walkdir = LEFT;
              objects[o].sprite = SCRUB_WALK_LEFT + objects[o].ai.scrub.walkframe;
              Scrub_TurnOnCansupportWhereNotKicked(o);
           }
           else
           {
              objects[o].y += SCRUB_WALK_SPEED;
              for(i=0;i<numplayers;i++)
              {
                if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
                {
                  // ensure that player is not blocked by a floor (can happen
                  // in certain situations if player is hanging off the right side
                  // of the scrub a bit)
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
                    player[i].y += SCRUB_WALK_SPEED;
                  }
                }
              }
           }
           break;
         case UP:
           objects[o].sprite = SCRUB_WALK_UP + objects[o].ai.scrub.walkframe;
           if (!objects[o].blockedl)
           {
             objects[o].ai.scrub.walkdir = LEFT;
             objects[o].sprite = SCRUB_WALK_LEFT + objects[o].ai.scrub.walkframe;
             Scrub_TurnOnCansupportWhereNotKicked(o);
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
             objects[o].ai.scrub.walkdir = RIGHT;
             objects[o].sprite = SCRUB_WALK_RIGHT + objects[o].ai.scrub.walkframe;
             SetAllCanSupportPlayer(o, 0);
           }
           {
             objects[o].y -= SCRUB_WALK_SPEED;
             for(i=0;i<numplayers;i++)
             {
               if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
               {
                 // kick player off if we're running him into the ceiling
                 if (player[i].blockedu)
                 {
                   objects[o].cansupportplayer[i] = 0;
                   objects[o].ai.scrub.kickedplayer[i] = 1;
                 }
                 else player[i].y -= SCRUB_WALK_SPEED;
               }
             }
           }
           break;
       }

       // walk animation
       if (objects[o].ai.scrub.animtimer > SCRUB_WALK_ANIM_TIME)
       {
         objects[o].ai.scrub.walkframe ^= 1;
         objects[o].ai.scrub.animtimer = 0;
       } else objects[o].ai.scrub.animtimer++;
     break;
     case SCRUB_FALLING:
       objects[o].sprite = SCRUB_WALK_DOWN;
       if (objects[o].blockedd)
       {
         objects[o].ai.scrub.walkdir = LEFT;
         objects[o].ai.scrub.state = SCRUB_WALK;
         objects[o].ai.scrub.walkframe = 0;
         objects[o].ai.scrub.animtimer = 0;
         Scrub_TurnOnCansupportWhereNotKicked(o);
       }
       else
       {
         if (objects[o].ai.scrub.fallinctimer > SCRUB_FALLSPDINCRATE)
         {
           if (objects[o].ai.scrub.fallspeed < SCRUB_MAX_FALL_SPEED)
           {
             objects[o].ai.scrub.fallspeed++;
           }
           objects[o].ai.scrub.fallinctimer = 0;
         } else objects[o].ai.scrub.fallinctimer++;

         objects[o].y += objects[o].ai.scrub.fallspeed;
       }
     break;
   }

   // sometimes it's possible for a scrub to fall off, for example
   // if it's walking left on a platform that has solidfall but not solidlr,
   // then turns to walk down. in this case we need to go into a falling
   // state and fall until we reach solid ground again. this keeps it from
   // freaking out and going flying across the screen.
   if (!objects[o].blockedl && !objects[o].blockedr &&
       !objects[o].blockedu && !objects[o].blockedd)
   {
       if (objects[o].ai.scrub.state!=SCRUB_FALLING)
       {
         objects[o].ai.scrub.fallinctimer = 0;
         objects[o].ai.scrub.fallspeed = SCRUB_MIN_FALL_SPEED;
         objects[o].ai.scrub.state = SCRUB_FALLING;
         SetAllCanSupportPlayer(o, 0);
       }
   }
}

void Scrub_TurnOnCansupportWhereNotKicked(int o)
{
unsigned int i;
  for(i=0;i<numplayers;i++)
  {
    if (!objects[o].ai.scrub.kickedplayer[i])
    {
      objects[o].cansupportplayer[i] = 1;
    }
  }
}
