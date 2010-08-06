#include "CButler.h"
#include "../../../keen.h"
#include "../../../sdl/sound/CSound.h"
#include "../../../graphics/CGfxEngine.h"
#include "../../../common/CBehaviorEngine.h"

CButler::CButler(CMap *pmap, Uint32 x, Uint32 y,
		std::vector<CPlayer> &PlayerVect) :
CObject(pmap, x, y, OBJ_BUTLER),
m_Player(PlayerVect)
{
	 state = BUTLER_WALK;
	 movedir = RIGHT;
	 animtimer = 0;
	 canbezapped = true;  // will stop bullets but are not harmed
	 m_invincible = true;
}

void CButler::process()
{
	 bool not_about_to_fall;

	 switch(state)
	 {
	 case BUTLER_TURN:
		 if (timer > BUTLER_TURN_TIME)
		 {
			 movedir = (movedir == LEFT) ? RIGHT : LEFT;
			 animtimer = 0;
			 state = BUTLER_WALK;
		 } else timer++;
		 break;
	 case BUTLER_WALK:

		 if (movedir==LEFT)
		 {  // move left
			 size_t tile = mp_Map->at((getXLeftPos()-(BUTLER_LOOK_AHEAD_DIST<<STC))>>CSF, (getYDownPos()+(1<<STC))>>CSF);
			 not_about_to_fall = g_pBehaviorEngine->getTileProperties().at(tile).bup;
			 sprite = BUTLER_WALK_LEFT_FRAME + frame;
			 if (!blockedl && not_about_to_fall)
			 {
				 moveLeft(BUTLER_WALK_SPEED);
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
	 
			 size_t tile = mp_Map->at((getXRightPos()+(BUTLER_LOOK_AHEAD_DIST<<STC))>>CSF, (getYDownPos()+(1<<STC))>>CSF);
			 not_about_to_fall = g_pBehaviorEngine->getTileProperties().at(tile).bup;
			 sprite = BUTLER_WALK_RIGHT_FRAME + frame;
			 if (!blockedr && not_about_to_fall)
			 {
				 moveRight(BUTLER_WALK_SPEED);
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

void CButler::getTouchedBy(CObject &theObject)
{   // push keen
	if(hitdetect(theObject))
	{
		if( theObject.m_type == OBJ_PLAYER )
		{
			CPlayer &Player = dynamic_cast<CPlayer&>(theObject);
			Player.bump( *this, movedir );
		}
	}
}
