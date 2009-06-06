// ai for the ball and the jack in ep3
#include "../keen.h"

#include "../include/enemyai.h"

#include "../include/game.h"

#define BALL_FRAME      109
#define JACK_FRAME      110

#define BALL_SPEED      15
#define JACK_SPEED      10
#define JACK_ANIM_RATE  50

#define BALLPUSHAMOUNT  20

char BJ_BlockedD(int o);

void ballandjack_ai(int o, stCloneKeenPlus *pCKP)
{
  if (objects[o].needinit)
  {
    objects[o].ai.bj.dir = DUPLEFT;
    objects[o].ai.bj.animframe = 0;
    objects[o].ai.bj.animtimer = 0;
    objects[o].blockedl = 0;
    objects[o].blockedr = 0;
    objects[o].blockedu = 0;
    objects[o].blockedd = 0;
    objects[o].inhibitfall = 1;

    if (objects[o].type==OBJ_BALL)
    {
      objects[o].sprite = BALL_FRAME;
      objects[o].ai.bj.speed = BALL_SPEED;
    }
    else
    {
      objects[o].sprite = JACK_FRAME;
      objects[o].ai.bj.speed = JACK_SPEED;
    }
    objects[o].needinit = 0;
  }

  if (objects[o].touchPlayer)
  {
      if (objects[o].type==OBJ_BALL)
      {
        if (player[objects[o].touchedBy].x < objects[o].x)
        {
          player[objects[o].touchedBy].playpushed_x = -BALLPUSHAMOUNT;
          player[objects[o].touchedBy].playpushed_decreasetimer = 0;
        }
        else
        {
          player[objects[o].touchedBy].playpushed_x = BALLPUSHAMOUNT;
          player[objects[o].touchedBy].playpushed_decreasetimer = 0;
        }
      }
      else killplayer(objects[o].touchedBy, pCKP);
  }

  switch(objects[o].ai.bj.dir)
  {
   case DUPLEFT:
     if (objects[o].blockedu) { objects[o].ai.bj.dir = DDOWNLEFT; }
     else objects[o].y -= objects[o].ai.bj.speed;

     if (objects[o].blockedl) { objects[o].ai.bj.dir = DUPRIGHT; }
     else objects[o].x -= objects[o].ai.bj.speed;
     break;
   case DUPRIGHT:
     if (objects[o].blockedu) { objects[o].ai.bj.dir = DDOWNRIGHT; }
     else objects[o].y -= objects[o].ai.bj.speed;

     if (objects[o].blockedr) { objects[o].ai.bj.dir = DUPLEFT; }
     else objects[o].x += objects[o].ai.bj.speed;
     break;
   case DDOWNLEFT:
     if (BJ_BlockedD(o)) { objects[o].ai.bj.dir = DUPLEFT; }
     else objects[o].y += objects[o].ai.bj.speed;

     if (objects[o].blockedl) { objects[o].ai.bj.dir = DDOWNRIGHT; }
     else objects[o].x -= objects[o].ai.bj.speed;
     break;
   case DDOWNRIGHT:
     if (BJ_BlockedD(o)) { objects[o].ai.bj.dir = DUPRIGHT; }
     else objects[o].y += objects[o].ai.bj.speed;

     if (objects[o].blockedr) { objects[o].ai.bj.dir = DDOWNLEFT; }
     else objects[o].x += objects[o].ai.bj.speed;
     break;
  }

  if (objects[o].type==OBJ_BALL)
  {
    objects[o].sprite = BALL_FRAME;
  }
  else
  {
    objects[o].sprite = JACK_FRAME + objects[o].ai.bj.animframe;
    if (objects[o].ai.bj.animtimer > JACK_ANIM_RATE)
    {
      objects[o].ai.bj.animframe++;
      if (objects[o].ai.bj.animframe>3) objects[o].ai.bj.animframe=0;
      objects[o].ai.bj.animtimer = 0;
    }
    else objects[o].ai.bj.animtimer++;
  }
}

char BJ_BlockedD(int o)
{
  // we do our own blockedd, because we don't want the ball/jack to
  // bounce off the top of platforms that have only solidfall set--
  // so we test blockedd against solidl/r instead

  if (objects[o].blockedd)
  {
    // ensure that the tile common_enemy_ai said we hit also has
    // solid l/r set

	if (TileProperty[getmaptileat((objects[o].x>>CSF)+2, (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize)][BLEFT])
    //if (tiles[getmaptileat((objects[o].x>>CSF)+2, (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize)].solidl)
      { return 1; }
	if (TileProperty[getmaptileat((objects[o].x>>CSF)+(sprites[objects[o].sprite].xsize-2), (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize)][BLEFT])
    //if (tiles[getmaptileat((objects[o].x>>CSF)+(sprites[objects[o].sprite].xsize-2), (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize)].solidl)
      { return 1; }
  }

  // ensure it's not a ball no-pass point
  if (getlevelat((objects[o].x>>CSF)+2, (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize)==BALL_NOPASSPOINT)
    { return 1; }
  if (getlevelat((objects[o].x>>CSF)+(sprites[objects[o].sprite].xsize-2), (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize)==BALL_NOPASSPOINT)
    { return 1; }

  return 0;
}
