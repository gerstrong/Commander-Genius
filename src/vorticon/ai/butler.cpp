#include "CObjectAI.h"
#include "../../keen.h"
#include "../../game.h"
#include "../../sdl/sound/CSound.h"
#include "../../graphics/CGfxEngine.h"

// AI for "butler" robot (ep1)
enum Butleractionn{
	BUTLER_TURN, BUTLER_WALK
};

#define BUTLER_WALK_SPEED        36
#define BUTLER_WALK_SPEED_FAST   40
#define BUTLER_WALK_ANIM_TIME    6
#define BUTLER_WALK_ANIM_TIME_FAST    1
#define BUTLER_TURN_TIME         10

#define BUTLERPUSHAMOUNT         22
#define BUTLERPUSHAMOUNTFAST     15

// distance in pixels butler should look ahead to avoid falling
// off an edge
#define BUTLER_LOOK_AHEAD_DIST   1

// frames
#define BUTLER_WALK_LEFT_FRAME   92
#define BUTLER_WALK_RIGHT_FRAME  88
#define BUTLER_TURNLEFT_FRAME    96
#define BUTLER_TURNRIGHT_FRAME   97

void  CObjectAI::butler_ai(CObject *p_object, char difficulty)
{
	 char not_about_to_fall;
	 Uint16 butler_height, butler_width;
	 
	 if (p_object->needinit)
	 {
		 p_object->ai.butler.state = BUTLER_WALK;
		 p_object->ai.butler.movedir = RIGHT;
		 p_object->ai.butler.animtimer = 0;
		 p_object->canbezapped = 1;  // will stop bullets but are not harmed
		 p_object->needinit = 0;
		 p_object->y -= 8;
	 }
	 // push keen
     if (p_object->touchPlayer && !mp_Player[p_object->touchedBy].pdie)
     {
    	 if(!((mp_Player[0].pdir == p_object->ai.butler.movedir) && (mp_Player[0].pwalking)))
    	 {
    		 g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, p_object->scrx);
	 
    		 short butlerpushamount;
	 
    		 butlerpushamount = BUTLERPUSHAMOUNT;
	 
    		 if(mp_Player[0].pwalking) butlerpushamount = 3*BUTLERPUSHAMOUNT/2;
	 
    		 if (mp_Player[0].x < p_object->x)
    		 {
    			 mp_Player[p_object->touchedBy].playpushed_x = -butlerpushamount;
    			 if (difficulty>3) mp_Player[p_object->touchedBy].playpushed_x -= BUTLERPUSHAMOUNTFAST;
    			 mp_Player[p_object->touchedBy].playpushed_decreasetimer = 0;
    			 mp_Player[p_object->touchedBy].pdir = mp_Player[p_object->touchedBy].pshowdir = LEFT;
    		 }
    		 else
    		 {
    			 mp_Player[p_object->touchedBy].playpushed_x = butlerpushamount;
    			 if (difficulty>3) mp_Player[p_object->touchedBy].playpushed_x += BUTLERPUSHAMOUNTFAST;
    			 mp_Player[p_object->touchedBy].playpushed_decreasetimer = 0;
    			 mp_Player[p_object->touchedBy].pdir = mp_Player[p_object->touchedBy].pshowdir = RIGHT;
    		 }
    	 }
     }
	 
	 switch(p_object->ai.butler.state)
	 {
	 case BUTLER_TURN:
		 if (p_object->ai.butler.timer > BUTLER_TURN_TIME)
		 {
			 p_object->ai.butler.movedir ^= 1;
			 p_object->ai.butler.animtimer = 0;
			 p_object->ai.butler.state = BUTLER_WALK;
		 } else p_object->ai.butler.timer++;
		 break;
	 case BUTLER_WALK:
		 CSprite *sprite = g_pGfxEngine->Sprite[BUTLER_WALK_LEFT_FRAME];
		 stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
		 butler_height = sprite->getHeight()<<STC;
		 butler_width = sprite->getWidth()<<STC;
		 if (p_object->ai.butler.movedir==LEFT)
		 {  // move left
			 not_about_to_fall = TileProperty[mp_Map->at((p_object->x-BUTLER_LOOK_AHEAD_DIST)>>CSF, (p_object->y+butler_height)>>CSF)].bup;
			 p_object->sprite = BUTLER_WALK_LEFT_FRAME + p_object->ai.butler.frame;
			 if (!p_object->blockedl && not_about_to_fall)
			 {
				 if (difficulty>3)
					 p_object->x -= BUTLER_WALK_SPEED_FAST;
				 else
					 p_object->x -= BUTLER_WALK_SPEED;
			 }
			 else
			 {
				 p_object->sprite = BUTLER_TURNRIGHT_FRAME;
				 p_object->ai.butler.frame = 0;
				 p_object->ai.butler.timer = 0;
				 p_object->ai.butler.animtimer = 0;
				 p_object->ai.butler.state = BUTLER_TURN;
			 }
		 }
		 else
		 {  // move right
	 
			 not_about_to_fall = TileProperty[mp_Map->at((p_object->x+butler_width+BUTLER_LOOK_AHEAD_DIST)>>CSF, (p_object->y+butler_height)>>CSF)].bup;
			 p_object->sprite = BUTLER_WALK_RIGHT_FRAME + p_object->ai.butler.frame;
			 if (!p_object->blockedr && not_about_to_fall)
			 {
				 if (difficulty>3)
					 p_object->x += BUTLER_WALK_SPEED_FAST;
				 else
					 p_object->x += BUTLER_WALK_SPEED;
			 }
			 else
			 {
				 p_object->sprite = BUTLER_TURNLEFT_FRAME;
				 p_object->ai.butler.frame = 0;
				 p_object->ai.butler.timer = 0;
				 p_object->ai.butler.animtimer = 0;
				 p_object->ai.butler.state = BUTLER_TURN;
			 }
		 }
		 // walk animation
		 if (p_object->ai.butler.animtimer > BUTLER_WALK_ANIM_TIME ||
				 (p_object->ai.butler.animtimer > BUTLER_WALK_ANIM_TIME_FAST && difficulty>3))
		 {
			 if (p_object->ai.butler.frame>=3) p_object->ai.butler.frame=0;
			 else p_object->ai.butler.frame++;
			 p_object->ai.butler.animtimer = 0;
		 }
		 else p_object->ai.butler.animtimer++;
		 break;
	 }
}
