#include "CButler.h"
#include "sdl/sound/CSound.h"
#include "graphics/CGfxEngine.h"
#include "common/CBehaviorEngine.h"

CButler::CButler(CMap *pmap, Uint32 x, Uint32 y) :
CVorticonSpriteObject(pmap, x, y-(1<<STC), OBJ_BUTLER)
{
	 state = BUTLER_WALK;
	 xDirection = LEFT;
	 animtimer = 0;
	 canbezapped = true;  // will stop bullets but are not harmed
	 m_invincible = true;
	 blockedd = true;
	 inhibitfall = false;
}

void CButler::process()
{   
	 switch(state)
	 {
	 case BUTLER_TURN:
		 if (timer > BUTLER_TURN_TIME)
		 {
			 xDirection = (xDirection == LEFT) ? RIGHT : LEFT;
			 animtimer = 0;
			 state = BUTLER_WALK;
		 } else timer++;
		 break;
		 
	 case BUTLER_WALK:	     
		 if (xDirection==LEFT)
		 {  // move left
			 sprite = BUTLER_WALK_LEFT_FRAME + frame;
			 if (!blockedl)
			 {
				 xinertia = -BUTLER_WALK_SPEED;
			 }
			 else
			 {
				 sprite = BUTLER_TURNRIGHT_FRAME;
				 frame = 0;
				 timer = 0;
				 animtimer = 0;
				 state = BUTLER_TURN;				 
			 }
		 }
		 else
		 {  // move right
			 sprite = BUTLER_WALK_RIGHT_FRAME + frame;
			 if (!blockedr)
			 {
				 xinertia = BUTLER_WALK_SPEED;
			 }
			 else
			 {
				 sprite = BUTLER_TURNLEFT_FRAME;
				 frame = 0;
				 timer = 0;
				 animtimer = 0;
				 state = BUTLER_TURN;
			 }
		 }
		 // walk animation
		 if (animtimer > BUTLER_WALK_ANIM_TIME)
		 {
			 if (frame>=3) frame=0;
			 else frame++;
			 animtimer = 0;
		 }
		 else animtimer++;
		 break;
	 default: break;
	 }
}

void CButler::getTouchedBy(CVorticonSpriteObject &theObject)
{   // push keen
	if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
	{
	    direction_t dir = xDirection<0 ? LEFT : RIGHT;
	    player->bump( dir );
	}
}

int CButler::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CSpriteObject::checkSolidD(x1, x2, y2, push_mode);
}
