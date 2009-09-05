// ai for the ball and the jack in ep3
#include "../keen.h"

#include "../include/game.h"
#include "../graphics/CGfxEngine.h"

#include "balljack.h"

#define BALL_SPEED      15
#define JACK_SPEED      10
#define JACK_ANIM_RATE  50

#define BALLPUSHAMOUNT	30

char BJ_BlockedD(int o);

void ballandjack_ai(int o)
{
  if (objects[o].needinit)
  {
	int i = rnd()%4;
	switch(i)
	{
		case 0: objects[o].ai.bj.dir = DUPLEFT; break;
		case 1: objects[o].ai.bj.dir = DUPRIGHT; break;
		case 2: objects[o].ai.bj.dir = DDOWNLEFT; break;
		case 3: objects[o].ai.bj.dir = DDOWNRIGHT; break;
	}

	objects[o].ai.bj.animframe = 0;
	objects[o].ai.bj.animtimer = 0;
	objects[o].blockedl = 0;
	objects[o].blockedr = 0;
	objects[o].blockedu = 0;
	objects[o].blockedd = 0;
	objects[o].inhibitfall = 1;

    if (objects[o].type==OBJ_BALL)
    {
      objects[o].ai.bj.speed = BALL_SPEED;
	  objects[o].canbezapped = 1;
    }
    else
    {
      objects[o].ai.bj.speed = JACK_SPEED;
	  objects[o].canbezapped = 0;
    }
    objects[o].needinit = 0;
  }

	if (objects[o].touchPlayer)
	{
		if (objects[o].type==OBJ_BALL)
		{
			if (player[objects[o].touchedBy].x < objects[o].x)
			{
				bumpplayer(objects[o].touchedBy, -BALLPUSHAMOUNT, true);
			}
			else
			{
				bumpplayer(objects[o].touchedBy, BALLPUSHAMOUNT, true);
			}

			switch(objects[o].ai.bj.dir)
			{
				case DUPRIGHT: objects[o].ai.bj.dir = DUPLEFT; break;
				case DUPLEFT: objects[o].ai.bj.dir = DUPRIGHT; break;
				case DDOWNRIGHT: objects[o].ai.bj.dir = DDOWNLEFT; break;
				case DDOWNLEFT: objects[o].ai.bj.dir = DDOWNRIGHT; break;
			}
		}
		else killplayer(objects[o].touchedBy);
	}

  if (objects[o].zapped)
  {
		// have ball change direction when zapped
		if (objects[o].zapd==LEFT)
		{
			switch(objects[o].ai.bj.dir)
			{
				case DUPRIGHT: objects[o].ai.bj.dir = DUPLEFT; break;
				case DDOWNRIGHT: objects[o].ai.bj.dir = DDOWNLEFT; break;
			}
		}
		else
		{
			switch(objects[o].ai.bj.dir)
			{
				case DUPLEFT: objects[o].ai.bj.dir = DUPRIGHT; break;
				case DDOWNLEFT: objects[o].ai.bj.dir = DDOWNRIGHT; break;
			}
		}
		objects[o].zapped = 0;
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
	objects[o].sprite = OBJ_BALL_DEFSPRITE;
  }
  else
  {
    objects[o].sprite = OBJ_JACK_DEFSPRITE + objects[o].ai.bj.animframe;
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
  CSprite *sprites = g_pGfxEngine->Sprite[objects[o].sprite];
  Uint16 obj_width, obj_height;
  obj_width = sprites->getWidth();
  obj_height = sprites->getHeight();

  if (objects[o].blockedd)
  {
    // ensure that the tile common_enemy_ai said we hit also has
    // solid l/r set
	if (TileProperty[getmaptileat((objects[o].x>>CSF)+2, (objects[o].y>>CSF)+obj_height)][BLEFT])
      { return 1; }
	if (TileProperty[getmaptileat((objects[o].x>>CSF)+(obj_width-2), (objects[o].y>>CSF)+obj_height)][BLEFT])
      { return 1; }

  }

  // ensure it's not a ball no-pass point
  if (getlevelat((objects[o].x>>CSF)+2, (objects[o].y>>CSF)+obj_height)==BALL_NOPASSPOINT)
    { return 1; }
  if (getlevelat((objects[o].x>>CSF)+(obj_width-2), (objects[o].y>>CSF)+obj_height)==BALL_NOPASSPOINT)
    { return 1; }

  return 0;
}
