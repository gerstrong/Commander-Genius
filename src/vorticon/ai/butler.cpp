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

#define BUTLERPUSHAMOUNT         44
#define BUTLERPUSHAMOUNTFAST     30

// distance in pixels butler should look ahead to avoid falling
// off an edge
#define BUTLER_LOOK_AHEAD_DIST   1

// frames
#define BUTLER_WALK_LEFT_FRAME   92
#define BUTLER_WALK_RIGHT_FRAME  88
#define BUTLER_TURNLEFT_FRAME    96
#define BUTLER_TURNRIGHT_FRAME   97

void  CObjectAI::butler_ai(CObject &object, char difficulty)
{
	 char not_about_to_fall;
	 Uint16 butler_height, butler_width;
	 
	 if (object.needinit)
	 {
		 object.ai.butler.state = BUTLER_WALK;
		 object.ai.butler.movedir = RIGHT;
		 object.ai.butler.animtimer = 0;
		 object.canbezapped = 1;  // will stop bullets but are not harmed
		 object.needinit = 0;
		 object.y -= 8;
	 }
	 // push keen
     if (object.touchPlayer && !m_Player[object.touchedBy].pdie)
     {
    	 if(!((m_Player[0].pdir == object.ai.butler.movedir) && (m_Player[0].pwalking)))
    	 {
    		 g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, object.scrx);
	 
    		 short butlerpushamount;
    		 butlerpushamount = BUTLERPUSHAMOUNT;
	 
    		 if(m_Player[0].pwalking) butlerpushamount = 3*BUTLERPUSHAMOUNT/2;
	 
    		 if (m_Player[0].x < object.x)
    		 {
    			 m_Player[object.touchedBy].playpushed_x = -butlerpushamount;
    			 if (difficulty>1) m_Player[object.touchedBy].playpushed_x -= BUTLERPUSHAMOUNTFAST;
    			 m_Player[object.touchedBy].playpushed_decreasetimer = 0;
    			 m_Player[object.touchedBy].pdir = m_Player[object.touchedBy].pshowdir = LEFT;
    		 }
    		 else
    		 {
    			 m_Player[object.touchedBy].playpushed_x = butlerpushamount;
    			 if (difficulty>1) m_Player[object.touchedBy].playpushed_x += BUTLERPUSHAMOUNTFAST;
    			 m_Player[object.touchedBy].playpushed_decreasetimer = 0;
    			 m_Player[object.touchedBy].pdir = m_Player[object.touchedBy].pshowdir = RIGHT;
    		 }
    	 }
     }
	 
	 switch(object.ai.butler.state)
	 {
	 case BUTLER_TURN:
		 if (object.ai.butler.timer > BUTLER_TURN_TIME)
		 {
			 object.ai.butler.movedir ^= 1;
			 object.ai.butler.animtimer = 0;
			 object.ai.butler.state = BUTLER_WALK;
		 } else object.ai.butler.timer++;
		 break;
	 case BUTLER_WALK:
		 CSprite *sprite = g_pGfxEngine->Sprite[BUTLER_WALK_LEFT_FRAME];
		 stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
		 butler_height = sprite->getHeight()<<STC;
		 butler_width = sprite->getWidth()<<STC;
		 if (object.ai.butler.movedir==LEFT)
		 {  // move left
			 not_about_to_fall = TileProperty[mp_Map->at((object.x-BUTLER_LOOK_AHEAD_DIST)>>CSF, (object.y+butler_height)>>CSF)].bup;
			 object.sprite = BUTLER_WALK_LEFT_FRAME + object.ai.butler.frame;
			 if (!object.blockedl && not_about_to_fall)
			 {
				 if (difficulty>1)
					 object.x -= BUTLER_WALK_SPEED_FAST;
				 else
					 object.x -= BUTLER_WALK_SPEED;
			 }
			 else
			 {
				 object.sprite = BUTLER_TURNRIGHT_FRAME;
				 object.ai.butler.frame = 0;
				 object.ai.butler.timer = 0;
				 object.ai.butler.animtimer = 0;
				 object.ai.butler.state = BUTLER_TURN;
			 }
		 }
		 else
		 {  // move right
	 
			 not_about_to_fall = TileProperty[mp_Map->at((object.x+butler_width+BUTLER_LOOK_AHEAD_DIST)>>CSF, (object.y+butler_height)>>CSF)].bup;
			 object.sprite = BUTLER_WALK_RIGHT_FRAME + object.ai.butler.frame;
			 if (!object.blockedr && not_about_to_fall)
			 {
				 if (difficulty>1)
					 object.x += BUTLER_WALK_SPEED_FAST;
				 else
					 object.x += BUTLER_WALK_SPEED;
			 }
			 else
			 {
				 object.sprite = BUTLER_TURNLEFT_FRAME;
				 object.ai.butler.frame = 0;
				 object.ai.butler.timer = 0;
				 object.ai.butler.animtimer = 0;
				 object.ai.butler.state = BUTLER_TURN;
			 }
		 }
		 // walk animation
		 if (object.ai.butler.animtimer > BUTLER_WALK_ANIM_TIME ||
				 (object.ai.butler.animtimer > BUTLER_WALK_ANIM_TIME_FAST && difficulty>1))
		 {
			 if (object.ai.butler.frame>=3) object.ai.butler.frame=0;
			 else object.ai.butler.frame++;
			 object.ai.butler.animtimer = 0;
		 }
		 else object.ai.butler.animtimer++;
		 break;
	 }
}
