#include "../../sdl/sound/CSound.h"

#include "../../keen.h"
#include "../../game.h"

#include "baby.h"

// Baby Vorticon (the superfast little blue creatures that knock you down)
// (ep 2 & 3)

#define BABY_RUN        0
#define BABY_DYING      1
#define BABY_DEAD       2

#define BABY_WALK_SPEED         15

#define BABY_WALK_ANIM_RATE     20

#define BABY_WALK_LEFT_FRAME    48
#define BABY_WALK_RIGHT_FRAME   52
#define BABY_FRY_FRAME          56
#define BABY_DEAD_FRAME         57

#define BABY_JUMP_PROB			80
#define BABY_BOUNCE_PROB		10

#define BABY_FRY_TIME           80
#define BABY_DIE_INERTIA        20

#define BABY_JUMP_BIG           1
#define BABY_JUMP_SMALL         0
void baby_jump(int o, int big);

void baby_ai(int o, int episode, bool hard)
{
	/*bool ep3;
	 
	 if (objects[o].needinit)
	 {
	 objects[o].ai.baby.state = BABY_RUN;
	 objects[o].ai.baby.dir = rnd()&1?LEFT:RIGHT;
	 objects[o].ai.baby.walkframe = 0;
	 objects[o].ai.baby.walktimer = 0;
	 objects[o].ai.baby.inertia_x = 0;
	 objects[o].ai.baby.inertia_y = 0;
	 objects[o].canbezapped = 1;
	 objects[o].needinit = 0;
	 objects[o].blockedl = objects[o].blockedr = objects[o].blockedu = objects[o].blockedd = 1;
	 
	 // babies are in ep2 & ep3, but frameset starts one index prior in ep3
	 if (episode==3) ep3 = true; else ep3 = false;
	 objects[o].sprite = BABY_WALK_RIGHT_FRAME - ep3;
	 return;
	 }
	 if (objects[o].ai.baby.state==BABY_DEAD)
	 {
	 objects[o].inhibitfall = 0;
	 return;
	 }
	 
	 // babies are in ep2 & ep3, but frameset starts one index prior in ep3
	 if (episode==3) ep3 = true; else ep3 = false;
	 
	 // jumping
	 if (objects[o].ai.baby.inertia_y < 0 || !objects[o].blockedd)
	 {
	 if (objects[o].blockedu && objects[o].ai.baby.inertia_y < 0)
	 {
	 objects[o].ai.baby.inertia_y = 0;
	 }
	 
	 objects[o].y += objects[o].ai.baby.inertia_y;
	 
	 if (objects[o].ai.baby.jumpdectimer >= objects[o].ai.baby.jumpdecrate)
	 {
	 if (objects[o].ai.baby.inertia_y < (1<<CSF))
	 {
	 objects[o].ai.baby.inertia_y++;
	 }
	 objects[o].ai.baby.jumpdectimer = 0;
	 }
	 else objects[o].ai.baby.jumpdectimer++;
	 }
	 else	// blockedd = 1, and inertia_y >= 0
	 {
	 objects[o].ai.baby.inertia_y = 0;
	 if (objects[o].ai.baby.state == BABY_RUN)
	 {
	 if (rnd()%BABY_JUMP_PROB == (BABY_JUMP_PROB/2))
	 {
	 baby_jump(o, BABY_JUMP_SMALL);
	 }
	 }
	 }
	 
	 
	 // got hit?
	 if (objects[o].zapped)
	 {
	 if (objects[o].zapped > 1 || !hard)
	 {  // we're fried!!
	 if (objects[o].ai.baby.state != BABY_DYING)
	 {
	 objects[o].ai.baby.dietimer = 0;
	 objects[o].ai.baby.state = BABY_DYING;
	 objects[o].ai.baby.jumpdecrate = 5;
	 objects[o].sprite = BABY_FRY_FRAME - ep3;
	 objects[o].zapped = 0;
	 objects[o].canbezapped = 0;
	 if (objects[o].onscreen && !g_pSound->isPlaying(SOUND_VORT_DIE))
	 g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, objects[o].scrx);
	 
	 if (objects[o].ai.baby.dir == RIGHT)
	 objects[o].ai.baby.inertia_x = BABY_DIE_INERTIA;
	 else
	 objects[o].ai.baby.inertia_x = -BABY_DIE_INERTIA;
	 
	 }
	 }
	 }
	 
	 // touched player
	 if (objects[o].touchPlayer && objects[o].ai.baby.state!=BABY_DYING && !player[objects[o].touchedBy].pfrozentime)
	 {
	 player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = objects[o].ai.baby.dir;
	 freezeplayer(objects[o].touchedBy);
	 }
	 
	 switch(objects[o].ai.baby.state)
	 {
	 case BABY_DYING:
     if ((objects[o].ai.baby.inertia_x < 0 && objects[o].blockedl) ||\
	 (objects[o].ai.baby.inertia_x > 0 && objects[o].blockedr))
     {
	 objects[o].ai.baby.inertia_x = 0;
     }
	 
     objects[o].x += objects[o].ai.baby.inertia_x;
     if (objects[o].ai.baby.xdectimer >= 10)
     {
	 if (objects[o].ai.baby.inertia_x < 0)
	 {
	 objects[o].ai.baby.inertia_x++;
	 }
	 else if (objects[o].ai.baby.inertia_x > 0)
	 {
	 objects[o].ai.baby.inertia_x--;
	 }
	 
	 if (objects[o].ai.baby.inertia_x == 0 &&
	 objects[o].ai.baby.inertia_y == 0 &&
	 objects[o].sprite==(unsigned int)(BABY_DEAD_FRAME-ep3))
	 {
	 objects[o].ai.baby.state = BABY_DEAD;
	 }
	 objects[o].ai.baby.xdectimer = 0;
     }
     else objects[o].ai.baby.xdectimer++;
	 
     if (objects[o].ai.baby.dietimer > BABY_FRY_TIME)
     {
	 objects[o].sprite = BABY_DEAD_FRAME-ep3;
     }
     else objects[o].ai.baby.dietimer++;
	 break;
	 case BABY_RUN:
     // run in appropriate direction
     if (objects[o].ai.baby.dir==RIGHT)
     { // running right
	 objects[o].sprite = BABY_WALK_RIGHT_FRAME + objects[o].ai.baby.walkframe - ep3;
	 if (objects[o].blockedr)
	 {
	 objects[o].ai.baby.dir = LEFT;
	 if ((rnd()&BABY_BOUNCE_PROB)==BABY_BOUNCE_PROB/2) baby_jump(o, BABY_JUMP_BIG);
	 }
	 else
	 {
	 objects[o].x += BABY_WALK_SPEED;
	 }
     }
     else
     { // running left
	 objects[o].sprite = BABY_WALK_LEFT_FRAME + objects[o].ai.baby.walkframe - ep3;
	 if (objects[o].blockedl)
	 {
	 objects[o].ai.baby.dir = RIGHT;
	 if ((rnd()&BABY_BOUNCE_PROB)==BABY_BOUNCE_PROB/2) baby_jump(o, BABY_JUMP_BIG);
	 }
	 else
	 {
	 objects[o].x -= BABY_WALK_SPEED;
	 objects[o].inhibitfall = 1;
	 }
     }
	 
     // run animation
     if (objects[o].ai.baby.walktimer > BABY_WALK_ANIM_RATE)
     {
	 objects[o].ai.baby.walkframe++;
	 if (objects[o].ai.baby.walkframe > 3) objects[o].ai.baby.walkframe = 0;
	 objects[o].ai.baby.walktimer = 0;
     } else objects[o].ai.baby.walktimer++;
	 break;
	 }*/
}

#define BABY_BIGJUMP                 25
#define BABY_BIGJUMP_DEC_RATE        10

#define BABY_MIN_SMALLJUMP             23
#define BABY_SMALLJUMP_MIN_DEC_RATE    18
#define BABY_MAX_SMALLJUMP             30
#define BABY_SMALLJUMP_MAX_DEC_RATE    20
void baby_jump(int o, int big)
{
    /*if ((rnd()&2)==0) big = 1-big;
	 if (big==BABY_JUMP_BIG)
	 {
	 objects[o].ai.baby.inertia_y = -BABY_BIGJUMP;
	 objects[o].ai.baby.jumpdecrate = BABY_BIGJUMP_DEC_RATE;
	 }
	 else
	 {
	 objects[o].ai.baby.inertia_y = -20;//(rnd()%(BABY_MAX_SMALLJUMP-BABY_MIN_SMALLJUMP))+BABY_MIN_SMALLJUMP;
	 objects[o].ai.baby.jumpdecrate = 10;//(rnd()%(BABY_SMALLJUMP_MAX_DEC_RATE-BABY_SMALLJUMP_MIN_DEC_RATE))+BABY_SMALLJUMP_MIN_DEC_RATE;
	 }
	 
	 
	 objects[o].ai.baby.jumpdectimer = 0;*/
}


