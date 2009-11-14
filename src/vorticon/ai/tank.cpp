#include "CObjectAI.h"
#include "../spritedefines.h"
#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "../../game.h"

// Tank Robot (ep1)
enum{
TANK_LOOK,TANK_WALK,TANK_FIRE
};

#define TANK_SAME_LEVEL_TIME   100
#define TANK_REPEAT_FIRE_TIME  200

#define TANK_SAME_LEVEL_TIME_FAST    200
#define TANK_REPEAT_FIRE_TIME_FAST   1200

// when this probability is satisfied, there is 50% probability
// of a look, 50% probability of a fire.
#define TANK_LOOKFIRE_PROB    500
#define TANK_MINTRAVELDIST    200

#define TANK_WALK_SPEED         16
#define TANK_WALK_ANIM_TIME     15
#define TANK_LOOK_ANIM_TIME     18
#define TANK_LOOK_TOTALTIME     45
#define TANK_PREPAREFIRE_TIME   45
#define TANK_PREPAREFIRE_TIME_FAST   80

// frames
#define TANK_WALK_LEFT_FRAME        102
#define TANK_WALK_RIGHT_FRAME       98
#define TANK_LOOK_FRAME             106

#define TANKPUSHAMOUNT        16

#define Sprite g_pGfxEngine->Sprite

//reference to ../misc.cpp
unsigned int rnd(void);

void CObjectAI::tank_ai(CObject *p_object, bool hardmode)
{
	 if (p_object->needinit)
	 {  // first time initilization
		 p_object->ai.tank.state = TANK_WALK;
		 p_object->ai.tank.movedir = RIGHT;
		 p_object->ai.tank.fireafterlook = 0;
		 p_object->ai.tank.animtimer = 0;
		 p_object->ai.tank.frame = 0;
		 p_object->ai.tank.timer = 0;
		 p_object->ai.tank.ponsameleveltime = 0;
		 p_object->ai.tank.alreadyfiredcauseonsamelevel = 0;
		 p_object->ai.tank.dist_traveled = 0;
		 p_object->ai.tank.detectedPlayer = 0;
		 p_object->ai.tank.detectedPlayerIndex = 0;
		 p_object->canbezapped = true;  // will stop bullets but are not harmed
		 p_object->needinit = false;
	 }
	 // stop keen from walking through our sprite
     if (p_object->touchPlayer && !mp_Player[p_object->touchedBy].pdie)
     {
    	 g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, p_object->scrx);
	 
    	 if (!((mp_Player[p_object->touchedBy].y) < (p_object->y - 300))) // give the mp_Player a little jump-over room
    	 {
    		 if (mp_Player[p_object->touchedBy].x < p_object->x)
    		 {
    			 mp_Player[p_object->touchedBy].playpushed_x = -TANKPUSHAMOUNT;
    			 mp_Player[p_object->touchedBy].playpushed_decreasetimer = 0;
    			 mp_Player[p_object->touchedBy].pdir = mp_Player[p_object->touchedBy].pshowdir = LEFT;
    		 }
    		 else
    		 {
    			 mp_Player[p_object->touchedBy].playpushed_x = TANKPUSHAMOUNT;
    			 mp_Player[p_object->touchedBy].playpushed_decreasetimer = 0;
    			 mp_Player[p_object->touchedBy].pdir = mp_Player[p_object->touchedBy].pshowdir = RIGHT;
    		 }
    	 }
     }
	 
	 switch(p_object->ai.tank.state)
	 {
	 case TANK_LOOK:
		 p_object->sprite = TANK_LOOK_FRAME + p_object->ai.tank.frame;
		 // animation
		 if (p_object->ai.tank.animtimer > TANK_LOOK_ANIM_TIME)
		 {
			 p_object->ai.tank.frame ^= 1;
			 p_object->ai.tank.animtimer = 0;
		 } else p_object->ai.tank.animtimer++;
	 
		 // when time is up go back to moving
		 if (p_object->ai.tank.timer > TANK_LOOK_TOTALTIME)
		 {
			 // decide what direction to go
			 // did we go into this state for the purpose of turning and firing?
			 if (!p_object->ai.tank.fireafterlook)
			 { // no
				 if (!tank_CanMoveRight(p_object, Sprite[TANK_WALK_RIGHT_FRAME]->getWidth()<<STC,
												 Sprite[TANK_WALK_RIGHT_FRAME]->getHeight()<<STC))
				 { p_object->ai.tank.movedir = LEFT; }
				 else if (!tank_CanMoveLeft(p_object,
											 Sprite[TANK_WALK_RIGHT_FRAME]->getHeight())<<STC)
				 { p_object->ai.tank.movedir = RIGHT; }
				 else if (rnd()&1) // turn towards mp_Player
				 { p_object->ai.tank.movedir = LEFT; }
				 else
				 { p_object->ai.tank.movedir = RIGHT; }
	 
				 p_object->ai.tank.state = TANK_WALK;
				 p_object->ai.tank.frame = 0;
				 p_object->ai.tank.animtimer = 0;
				 p_object->ai.tank.timer = 0;
				 p_object->ai.tank.dist_traveled = 0;
			 }
			 else
			 { // yes, face towards mp_Player and fire.
				 if (mp_Player[p_object->ai.tank.detectedPlayerIndex].x < p_object->x)
				 {
					 p_object->ai.tank.movedir = LEFT;
					 p_object->sprite = TANK_WALK_LEFT_FRAME;
				 }
				 else
				 {
					 p_object->ai.tank.movedir = RIGHT;
					 p_object->sprite = TANK_WALK_RIGHT_FRAME;
				 }
				 p_object->ai.tank.timer = 0;
				 p_object->ai.tank.fireafterlook = 0;
				 p_object->ai.tank.state = TANK_FIRE;
			 }
		 } else p_object->ai.tank.timer++;
	 break;
	 case TANK_WALK:
		 // is keen on same level?
		 p_object->ai.tank.detectedPlayer = 0;
		 for(Uint32 i=0;i<m_NumPlayers;i++)
		 {
			 if (mp_Player[i].y >= p_object->y-(48<<CSF))
			 {
				 if ((mp_Player[i].y>>CSF)+Sprite[0]->getHeight() <= (p_object->y>>CSF)+Sprite[p_object->sprite]->getHeight()+48)
				 {
					 p_object->ai.tank.detectedPlayer = 1;
					 p_object->ai.tank.detectedPlayerIndex = i;
					 break;
				 }
			 }
	 }
	 
	 if (p_object->ai.tank.detectedPlayer)
	 {
		 p_object->ai.tank.ponsameleveltime++;
		 if ((p_object->ai.tank.ponsameleveltime > TANK_SAME_LEVEL_TIME) ||
				 (p_object->ai.tank.ponsameleveltime > TANK_SAME_LEVEL_TIME_FAST && hardmode))
		 {   // keen would be a good target now (hard mode).
			 if (!p_object->ai.tank.alreadyfiredcauseonsamelevel ||\
					 p_object->ai.tank.ponsameleveltime > TANK_REPEAT_FIRE_TIME || \
					 (p_object->ai.tank.ponsameleveltime > TANK_REPEAT_FIRE_TIME_FAST && hardmode))
			 {
				 // facing keen?
				 p_object->ai.tank.alreadyfiredcauseonsamelevel = 1;
				 if (((mp_Player[p_object->ai.tank.detectedPlayerIndex].x < p_object->x) && p_object->ai.tank.movedir==LEFT) || \
						 ((mp_Player[p_object->ai.tank.detectedPlayerIndex].x > p_object->x) && p_object->ai.tank.movedir==RIGHT))
				 { // yes, we're facing him, we see him, we fire!
					 p_object->ai.tank.timer = 0;
					 p_object->ai.tank.state = TANK_FIRE;
					 p_object->ai.tank.ponsameleveltime = 0;
				 }
				 else
				 { // no, we're not facing him. turn and fire!
					 p_object->ai.tank.timer = 0;
					 p_object->ai.tank.frame = 0;
					 p_object->ai.tank.fireafterlook = 1;
					 p_object->ai.tank.state = TANK_LOOK;
				 }
			 }
		 }
	 }
	 else
	 {  // no, not on same level
		 p_object->ai.tank.ponsameleveltime = 0;
		 p_object->ai.tank.alreadyfiredcauseonsamelevel = 0;
	 }

	 if (p_object->ai.tank.dist_traveled > TANK_MINTRAVELDIST && p_object->onscreen)
	 {
		 if (rnd()%TANK_LOOKFIRE_PROB==(TANK_LOOKFIRE_PROB/2))
		 {  // we're either going to look or fire
			 if (rnd()&1)
			 { // look
				 p_object->ai.tank.timer = 0;
				 p_object->ai.tank.frame = 0;
				 p_object->ai.tank.state = TANK_LOOK;
			 }
			 else
			 { // FIRE!
				 p_object->ai.tank.timer = 0;
				 p_object->ai.tank.state = TANK_FIRE;
			 }
			 break;
		 }
	 }

	 if (p_object->ai.tank.movedir==LEFT)
	 {  // move left
		 p_object->sprite = TANK_WALK_LEFT_FRAME + p_object->ai.tank.frame;
		 if (tank_CanMoveLeft(p_object, Sprite[TANK_WALK_RIGHT_FRAME]->getHeight()))
		 {
			 p_object->x -= TANK_WALK_SPEED;
			 p_object->ai.tank.dist_traveled++;
		 }
		 else
		 {
			 p_object->ai.tank.frame = 0;
			 p_object->ai.tank.timer = 0;
			 p_object->ai.tank.animtimer = 0;
			 p_object->ai.tank.state = TANK_LOOK;
		 }
	 }
	 else
	 {  // move right
		 p_object->sprite = TANK_WALK_RIGHT_FRAME + p_object->ai.tank.frame;
		 if (tank_CanMoveRight(p_object, Sprite[TANK_WALK_RIGHT_FRAME]->getWidth(),
				 Sprite[TANK_WALK_RIGHT_FRAME]->getHeight()))
		 {
			 p_object->x += TANK_WALK_SPEED;
			 p_object->ai.tank.dist_traveled++;
		 }
		 else
		 {
			 p_object->ai.tank.frame = 0;
			 p_object->ai.tank.timer = 0;
			 p_object->ai.tank.animtimer = 0;
			 p_object->ai.tank.state = TANK_LOOK;
		 }
	 }
	 // walk animation
	 if (p_object->ai.tank.animtimer > TANK_WALK_ANIM_TIME)
	 {
		 if (p_object->ai.tank.frame>=3) p_object->ai.tank.frame=0;
		 else p_object->ai.tank.frame++;
		 p_object->ai.tank.animtimer = 0;
	 } else p_object->ai.tank.animtimer++;
	 break;
	 case TANK_FIRE:
		 if (p_object->ai.tank.timer > TANK_PREPAREFIRE_TIME || \
				 (p_object->ai.tank.timer > TANK_PREPAREFIRE_TIME_FAST && hardmode))
		 {
			 CObject newobject;
			 if (p_object->onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, p_object->scrx);
			 if (p_object->ai.tank.movedir==RIGHT)
			 {
				 newobject.spawn(p_object->x+(Sprite[OBJ_RAY_DEFSPRITE_EP1]->getWidth()<<STC), p_object->y+(5<<STC), OBJ_RAY);
				 newobject.ai.ray.direction = RIGHT;
			 }
			 else
			 {
				 newobject.spawn(p_object->x-(Sprite[OBJ_RAY_DEFSPRITE_EP1]->getWidth()<<STC), p_object->y+(5<<STC), OBJ_RAY);
				 newobject.ai.ray.direction = LEFT;
			 }
			 newobject.sprite = ENEMYRAY;
			 newobject.ai.ray.dontHitEnable = 0;
			 mp_Objvect->push_back(newobject);

			 p_object->ai.tank.state = TANK_WALK;

		 } else p_object->ai.tank.timer++;
		 break;
	 }
}

bool CObjectAI::tank_CanMoveLeft(CObject *p_object, int h)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
	bool not_about_to_fall = TileProperty[mp_Map->at((p_object->x-1)>>CSF, (p_object->y+h)>>CSF)].bup;
	if (!p_object->blockedl && not_about_to_fall) return true;
	return false;
}

bool CObjectAI::tank_CanMoveRight(CObject *p_object, int w, int h)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
	bool not_about_to_fall = TileProperty[mp_Map->at((p_object->x+w+1)>>CSF, (p_object->y+h)>>CSF)].bup;
	if (!p_object->blockedr && not_about_to_fall) return true;
	return false;
}
