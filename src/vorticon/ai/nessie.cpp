#include "../../keen.h"
#include "../../game.h"

//#include "enemyai.h"

// Nessie (in ep3) (on the world map)

#define NESSIE_SWIMNORMAL         0
#define NESSIE_SWIMUPTOMORTIMER   1
#define NESSIE_PAUSE_MORTIMER     2
#define NESSIE_SWIMAWAY_MORTIMER  3

#define NESSIE_PAUSE_TIME       1000

#define NESSIE_ANIM_RATE        25

#define NESSIE_DOWNLEFT_FRAME   130
#define NESSIE_DOWNRIGHT_FRAME  132
#define NESSIE_UPRIGHT_FRAME    134
#define NESSIE_UPLEFT_FRAME     136

#define NESSIE_SPEED            4

#define NESSIE_MORTIMER_SWIM_AMT        ((8<<CSF)/NESSIE_SPEED)
#define NESSIE_MORTIMER_SWIMAWAY_AMT    ((16<<CSF)/NESSIE_SPEED)

void nessie_find_next_checkpoint(int o);

void nessie_ai(int o)
{
	/*unsigned int i;
	 int isMounted;
	 
	 if (objects[o].needinit)
	 {
	 objects[o].ai.nessie.baseframe = NESSIE_DOWNLEFT_FRAME;
	 objects[o].ai.nessie.leftrightdir = LEFT;
	 objects[o].ai.nessie.updowndir = DOWN;
	 objects[o].ai.nessie.animframe = 0;
	 objects[o].ai.nessie.animtimer = 0;
	 objects[o].ai.nessie.state = NESSIE_SWIMNORMAL;
	 objects[o].ai.nessie.pausetimer = 0;
	 objects[o].ai.nessie.mortimer_swim_amt = 0;
	 objects[o].ai.nessie.destx = 0;
	 objects[o].ai.nessie.desty = 0;
	 objects[o].inhibitfall = 1;
	 objects[o].canbezapped = 0;
	 objects[o].needinit = 0;
	 
	 for(i=0;i<NESSIETRAILLEN;i++)
	 {
	 objects[o].ai.nessie.tiletrailX[i] = 0;
	 objects[o].ai.nessie.tiletrailY[i] = 0;
	 }
	 // kick nessie into going counter-clockwise
	 // (otherwise she'll go clockwise)
	 objects[o].ai.nessie.tiletrailX[0] = (objects[o].x>>4>>CSF);
	 objects[o].ai.nessie.tiletrailY[0] = (objects[o].y>>4>>CSF);
	 objects[o].ai.nessie.tiletrailX[1] = (objects[o].x>>4>>CSF)+1;
	 objects[o].ai.nessie.tiletrailY[1] = (objects[o].y>>4>>CSF);
	 objects[o].ai.nessie.tiletrailhead = 2;
	 
	 for(i=0;i<MAX_PLAYERS;i++)
	 {
	 objects[o].ai.nessie.mounted[i] = 0;
	 }
	 
	 nessie_find_next_checkpoint(o);
	 }
	 
	 // find out if nessie is mounted, and for all players that are
	 // mounted keep them stuck to nessie
	 isMounted = 0;
	 for(i=0;i<numplayers;i++)
	 {
	 if (player[i].isPlaying)
	 {
	 if (objects[o].ai.nessie.mounted[i])
	 {
	 player[i].x = objects[o].x;
	 player[i].y = objects[o].y;
	 isMounted = 1;
	 }
	 }
	 }
	 
	 // animation
	 objects[o].sprite = objects[o].ai.nessie.baseframe + objects[o].ai.nessie.animframe;
	 if (isMounted) objects[o].sprite += 8;
	 
	 
	 if (objects[o].ai.nessie.animtimer > NESSIE_ANIM_RATE)
	 {
	 objects[o].ai.nessie.animframe ^= 1;
	 objects[o].ai.nessie.animtimer = 0;
	 }
	 else objects[o].ai.nessie.animtimer++;
	 
	 switch(objects[o].ai.nessie.state)
	 {
	 case NESSIE_SWIMUPTOMORTIMER:
     if (!objects[o].ai.nessie.mortimer_swim_amt)
     {
	 objects[o].ai.nessie.state = NESSIE_PAUSE_MORTIMER;
	 objects[o].ai.nessie.pausetimer = NESSIE_PAUSE_TIME;
	 objects[o].ai.nessie.baseframe = NESSIE_DOWNRIGHT_FRAME;
     }
     else
     {
	 objects[o].y -= NESSIE_SPEED;
	 objects[o].ai.nessie.mortimer_swim_amt--;
     }
	 break;
	 
	 case NESSIE_PAUSE_MORTIMER:
     if (!objects[o].ai.nessie.pausetimer)
     {
	 //       objects[o].ai.nessie.state = NESSIE_SWIMAWAY_MORTIMER;
	 //     objects[o].ai.nessie.mortimer_swim_amt = NESSIE_MORTIMER_SWIMAWAY_AMT;
	 objects[o].ai.nessie.state = NESSIE_SWIMNORMAL;
     }
     else objects[o].ai.nessie.pausetimer--;
	 break;
	 
	 case NESSIE_SWIMAWAY_MORTIMER:
     if (!objects[o].ai.nessie.mortimer_swim_amt)
     {
	 objects[o].ai.nessie.state = NESSIE_SWIMNORMAL;
     }
     else
     {
	 objects[o].x += NESSIE_SPEED;
	 objects[o].y += NESSIE_SPEED;
	 objects[o].ai.nessie.mortimer_swim_amt--;
     }
	 break;
	 
	 case NESSIE_SWIMNORMAL:
	 
     // arrived at destination?
     if (objects[o].x>>CSF==objects[o].ai.nessie.destx>>CSF && \
	 objects[o].y>>CSF==objects[o].ai.nessie.desty>>CSF)
     {
	 // if we're approaching the mount point by Mortimer's castle
	 // go into a special state that aligns her properly with the island
	 // (i couldn't get this to work right any other way)
	 if (objects[o].ai.nessie.desty>>CSF>>4 < map.ysize>>2)
	 {
	 if (map.objectlayer[(objects[o].ai.nessie.destx>>CSF>>4)+2][(((objects[o].ai.nessie.desty>>CSF)+8)>>4)-1]==NESSIE_PAUSE)
	 {
	 objects[o].ai.nessie.state = NESSIE_SWIMUPTOMORTIMER;
	 objects[o].ai.nessie.mortimer_swim_amt = NESSIE_MORTIMER_SWIM_AMT;
	 }
	 }
	 
	 nessie_find_next_checkpoint(o);
	 
	 // set up/down and left/right direction flags for frame selection
	 if (objects[o].ai.nessie.destx > objects[o].x)
	 {
	 objects[o].ai.nessie.leftrightdir = RIGHT;
	 }
	 else if (objects[o].ai.nessie.destx < objects[o].x)
	 {
	 objects[o].ai.nessie.leftrightdir = LEFT;
	 }
	 
	 if (objects[o].ai.nessie.desty < objects[o].y)
	 {
	 objects[o].ai.nessie.updowndir = UP;
	 }
	 else
	 {
	 objects[o].ai.nessie.updowndir = DOWN;
	 }
     }
	 
     if (objects[o].ai.nessie.pausetimer)
     {
	 objects[o].ai.nessie.updowndir = DOWN;
     }
	 
     // select proper frame based on up/down and left/right direction flags
     if (objects[o].ai.nessie.updowndir==DOWN && objects[o].ai.nessie.leftrightdir==LEFT)
     {
	 objects[o].ai.nessie.baseframe = NESSIE_DOWNLEFT_FRAME;
     }
     else if (objects[o].ai.nessie.updowndir==DOWN && objects[o].ai.nessie.leftrightdir==RIGHT)
     {
	 objects[o].ai.nessie.baseframe = NESSIE_DOWNRIGHT_FRAME;
     }
     else if (objects[o].ai.nessie.updowndir==UP && objects[o].ai.nessie.leftrightdir==LEFT)
     {
	 objects[o].ai.nessie.baseframe = NESSIE_UPLEFT_FRAME;
     }
     else if (objects[o].ai.nessie.updowndir==UP && objects[o].ai.nessie.leftrightdir==RIGHT)
     {
	 objects[o].ai.nessie.baseframe = NESSIE_UPRIGHT_FRAME;
     }
	 
     // pause if needed
     if (objects[o].ai.nessie.pausetimer)
     {
	 objects[o].ai.nessie.pausetimer--;
	 return;
     }
	 
     // head to destination
     if (objects[o].x < objects[o].ai.nessie.destx)
     {
	 if (objects[o].ai.nessie.destx - objects[o].x > NESSIE_SPEED)
	 {
	 objects[o].x += NESSIE_SPEED;
	 }
	 else
	 {
	 objects[o].x++;
	 }
     }
     else if (objects[o].x > objects[o].ai.nessie.destx)
     {
	 if (objects[o].x - objects[o].ai.nessie.destx > NESSIE_SPEED)
	 {
	 objects[o].x -= NESSIE_SPEED;
	 }
	 else
	 {
	 objects[o].x--;
	 }
     }
	 
     if (objects[o].y < objects[o].ai.nessie.desty)
     {
	 if (objects[o].ai.nessie.desty - objects[o].y > NESSIE_SPEED)
	 {
	 objects[o].y += NESSIE_SPEED;
	 }
	 else
	 {
	 objects[o].y++;
	 }
     }
     else if (objects[o].y > objects[o].ai.nessie.desty)
     {
	 if (objects[o].y - objects[o].ai.nessie.desty > NESSIE_SPEED)
	 {
	 objects[o].y -= NESSIE_SPEED;
	 }
	 else
	 {
	 objects[o].y--;
	 }
     }
	 
	 break;
	 }*/
}

void nessie_find_next_checkpoint(int o)
{
	/*int x,y,i;
	 int xa,ya;
	 unsigned int destx, desty;
	 int obj;
	 int oneoflasttiles;
	 
	 // search in the 8 surrounding tiles and head to the first pathtile
	 // we find that's not one of the last 5 we've been to
	 x = (objects[o].x>>CSF>>4)-1;
	 y = (((objects[o].y>>CSF)+8)>>4)-1;
	 destx = desty = 0;
	 for(ya=0;ya<3;ya++)
	 {
	 for(xa=0;xa<3;xa++)
	 {
	 destx = x+xa;
	 desty = y+ya;
	 
	 obj = map.objectlayer[destx][desty];
	 if (obj==NESSIE_PATH || obj==NESSIE_PAUSE)
	 {
	 // find out if this is one of the last tiles we've been to
	 oneoflasttiles = 0;
	 for(i=0;i<NESSIETRAILLEN;i++)
	 {
	 if (objects[o].ai.nessie.tiletrailX[i]==destx &&
	 objects[o].ai.nessie.tiletrailY[i]==desty)
	 {
	 oneoflasttiles = 1;
	 break;
	 }
	 }
	 
	 if (!oneoflasttiles)
	 {
	 objects[o].ai.nessie.tiletrailX[objects[o].ai.nessie.tiletrailhead] = destx;
	 objects[o].ai.nessie.tiletrailY[objects[o].ai.nessie.tiletrailhead] = desty;
	 objects[o].ai.nessie.tiletrailhead++;
	 if (objects[o].ai.nessie.tiletrailhead>=NESSIETRAILLEN)
	 {
	 objects[o].ai.nessie.tiletrailhead = 0;
	 }
	 goto foundtile;
	 } // end if (!oneoflasttiles)
	 }  // end if(obj==8192 ...
	 
	 } // end for(xa...
	 } // end for(ya...
	 foundtile: ;
	 
	 objects[o].ai.nessie.destx = (destx<<4<<CSF);
	 objects[o].ai.nessie.desty = (((desty<<4)-8)<<CSF);
	 
	 // make nessie pause at pause points, not in the top quarter
	 // of the map (i.e. not at the mortimer's castle mount point...
	 // this is done separetly elsewhere with a special state)
	 if (desty > map.ysize>>2)
	 {
	 if (map.objectlayer[destx][desty]==NESSIE_PAUSE)
	 {
	 // make nessie pause
	 objects[o].ai.nessie.pausetimer = NESSIE_PAUSE_TIME;
	 objects[o].ai.nessie.pausex = destx;
	 objects[o].ai.nessie.pausey = desty;
	 }
	 }*/
	
}

