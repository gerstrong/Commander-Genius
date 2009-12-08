#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"
#include "../../graphics/CGfxEngine.h"
#include "vort.h"

// Vorticon (all episodes, albeit the behavior changes slightly
// depending on levelcontrol.episode).

// Reference to ../misc.cpp
unsigned int rnd(void);

void CObjectAI::vort_ai(CObject &object, int level, int episode, char difficulty, bool dark)
{
	Uint32 bboxX1 = g_pGfxEngine->Sprite.at(object.sprite)->m_bboxX1;
	Uint32 bboxX2 = g_pGfxEngine->Sprite.at(object.sprite)->m_bboxX2;
	Uint32 bboxY1 = g_pGfxEngine->Sprite.at(object.sprite)->m_bboxY1;
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	bool bonk,kill;
	 if (object.needinit)
	 {  // first time initilization
		 object.ai.vort.frame = 0;
		 object.ai.vort.animtimer = 0;
		 object.ai.vort.state = VORT_LOOK;
		 object.ai.vort.timer = 0;
		 object.ai.vort.dist_traveled = VORT_TRAPPED_DIST+1;
		 object.canbezapped = 1;
		 object.needinit = 0;
	 
		 // give him some health points, depening on episode and level
		 if(episode == 1)
		 {
			 if(level == 16) // He is the vorticon commander and has much more HP
				 object.ai.vort.hp = 105;
			 else
				 object.ai.vort.hp = 4;
		 }
		 else
			 object.ai.vort.hp = (difficulty > 1) ? 4 : 1;
		 // In EP2 and 3 the Vorticons are much weaker, when no hardmode selected
	 
		 // copy in animation frame indexes for the current ep
		 if (episode==1)
		 {
			 object.ai.vort.WalkLeftFrame = VORT1_WALK_LEFT_FRAME;
			 object.ai.vort.WalkRightFrame = VORT1_WALK_RIGHT_FRAME;
			 object.ai.vort.LookFrame = VORT1_LOOK_FRAME;
			 object.ai.vort.JumpRightFrame = VORT1_JUMP_RIGHT_FRAME;
			 object.ai.vort.JumpLeftFrame = VORT1_JUMP_LEFT_FRAME;
			 object.ai.vort.DyingFrame = VORT1_DYING_FRAME;
		 }
		 else if (episode==2)
		 {
			 object.ai.vort.WalkLeftFrame = VORT2_WALK_LEFT_FRAME;
			 object.ai.vort.WalkRightFrame = VORT2_WALK_RIGHT_FRAME;
			 object.ai.vort.LookFrame = VORT2_LOOK_FRAME;
			 object.ai.vort.JumpRightFrame = VORT2_JUMP_RIGHT_FRAME;
			 object.ai.vort.JumpLeftFrame = VORT2_JUMP_LEFT_FRAME;
			 object.ai.vort.DyingFrame = VORT2_DYING_FRAME;
			 object.ai.vort.DeadFrame = VORT2_DEAD_FRAME;
		 }
		 else if (episode==3)
		 {
			 object.ai.vort.WalkLeftFrame = VORT3_WALK_LEFT_FRAME;
			 object.ai.vort.WalkRightFrame = VORT3_WALK_RIGHT_FRAME;
			 object.ai.vort.LookFrame = VORT3_LOOK_FRAME;
			 object.ai.vort.JumpRightFrame = VORT3_JUMP_RIGHT_FRAME;
			 object.ai.vort.JumpLeftFrame = VORT3_JUMP_LEFT_FRAME;
			 object.ai.vort.DyingFrame = VORT3_DYING_FRAME;
			 object.ai.vort.DeadFrame = VORT3_DEAD_FRAME;
		 }
	 }
	 
	 if (object.ai.vort.state==VORT_DEAD)   return;

	 if (object.canbezapped)
	 {
		 kill = false;
		 // if we touch a glowcell, we die!
		 if ( object.zapped >= object.ai.vort.hp ) kill = true;
		 else if (episode==2 && mp_Map->at((object.x + bboxX1)>>CSF, (object.y+bboxY1)>>CSF)==TILE_GLOWCELL)
			 kill = true;
	 
		 if (kill)
		 {
			 object.inhibitfall = false;
			 object.canbezapped = false;
			 object.ai.vort.animtimer = 0;
			 object.ai.vort.frame = 0;
			 object.ai.vort.palflashtimer = VORT_PALETTE_FLASH_TIME + 1;
			 object.ai.vort.palflashamt = 255;
			 if (episode == 1)
			 {
				 // TODO: Flash animation must be performed here!
				 object.ai.vort.state = VORT_DYING;
			 }
			 else
				 object.ai.vort.state = VORT2_DYING;
	 
			 g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, object.scrx);
		 }
	 }
	 // deadly to the touch
	 if (object.touchPlayer && object.canbezapped)
		 mp_Player[object.touchedBy].kill();
	 
	 vort_reprocess: ;
	 switch(object.ai.vort.state)
	 {
	 case VORT_JUMP:
		 if (object.ai.vort.movedir == RIGHT)
		 {
			 if (!object.blockedr) object.x += VORT_WALK_SPEED;
		 }
		 else
		 {
			 if (!object.blockedl) object.x -= VORT_WALK_SPEED;
		 }
	 
		 if (object.ai.vort.inertiay>0 && object.blockedd)
		 {  // The Vorticon Has Landed!
			 object.inhibitfall = 0;
			 object.ai.vort.state = VORT_LOOK;
			 goto vort_reprocess;
		 }
		 // check if the vorticon has bonked into a ceiling, if so,
		 // immediately terminate the jump
		 bonk = false;
	 
		 if (TileProperty[mp_Map->at((object.x + bboxX1)>>CSF, (object.y+bboxY1)>>CSF)].bdown) bonk = true;
		 else if (TileProperty[mp_Map->at((object.x + bboxX2)>>CSF, (object.y + bboxY1)>>CSF)].bdown) bonk = true;
	 
		 if (bonk && object.ai.vort.inertiay < 0)
		 {
			 object.ai.vort.inertiay = 0;
		 }
	 
		 // apply Y inertia
		 object.y += object.ai.vort.inertiay;
	 
		 if (object.ai.vort.timer > VORT_JUMP_FRICTION)
		 { // slowly decrease upgoing rate
			 if (object.ai.vort.inertiay<VORT_MAX_FALL_SPEED)
				 object.ai.vort.inertiay+=1<<4;

			 object.ai.vort.timer = 0;
		 }
		 else object.ai.vort.timer++;
		 break;
	 case VORT_LOOK:
			 object.sprite = object.ai.vort.LookFrame + object.ai.vort.frame;
	 
			 if (object.ai.vort.animtimer > VORT_LOOK_ANIM_TIME)
			 {
				 if (object.ai.vort.frame>0)
				 {
					 if (object.blockedl)
					 { object.ai.vort.movedir = RIGHT; }
					 else if (object.blockedr)
					 { object.ai.vort.movedir = LEFT; }
					 else
					 { // not blocked on either side, head towards player
						 if ( mp_Player[0].x < object.x)
						 { object.ai.vort.movedir = LEFT; }
						 else
						 { object.ai.vort.movedir = RIGHT; }
					 }
					 object.ai.vort.timer = 0;
					 object.ai.vort.frame = 0;
					 object.ai.vort.state = VORT_WALK;
				 } else object.ai.vort.frame++;
				 object.ai.vort.animtimer = 0;
			 } else object.ai.vort.animtimer++;
			 break;
	 case VORT_WALK:
			 object.ai.vort.dist_traveled++;
	 
			 if (rnd()%VORT_JUMP_PROB == (VORT_JUMP_PROB/2))
			 {  // let's jump.
				 if (!dark && !object.blockedu)
				 {
					 vort_initiatejump(object);
					 goto vort_reprocess;
				 }
			 }
			 if (object.ai.vort.movedir==LEFT)
			 {  // move left
				 object.sprite = object.ai.vort.WalkLeftFrame + object.ai.vort.frame;
	 
				 if (!object.blockedl)
				 {
					 object.x -= VORT_WALK_SPEED;
				 }
				 else
				 {
					 object.ai.vort.frame = 0;
					 object.ai.vort.animtimer = 0;
					 object.ai.vort.state = VORT_LOOK;
	 
					 // if we only traveled a tiny amount before hitting a wall, we've
					 // probably fallen into a small narrow area, and we need to try
					 // to jump out of it
					 if (object.ai.vort.dist_traveled < VORT_TRAPPED_DIST && !dark && object.blockedd && !object.blockedu)
					 {
						 vort_initiatejump(object);
						 if (rnd()&1)
						 {
							 object.ai.vort.inertiay = -VORT_MAX_JUMP_HEIGHT;
						 }
						 else
						 {
							 object.ai.vort.inertiay = -VORT_MIN_JUMP_HEIGHT;
						 }
						 goto vort_reprocess;
					 }
					 else object.ai.vort.dist_traveled = 0;
				 }
			 }
			 else
			 {  // move right
				 object.sprite = object.ai.vort.WalkRightFrame + object.ai.vort.frame;

				 if (!object.blockedr)
				 {
					 object.x += VORT_WALK_SPEED;
				 }
				 else
				 {
					 object.ai.vort.frame = 0;
					 object.ai.vort.animtimer = 0;
					 object.ai.vort.state = VORT_LOOK;
	 
					 if (object.ai.vort.dist_traveled < VORT_TRAPPED_DIST && !dark && object.blockedd && !object.blockedu)
					 {
						 vort_initiatejump(object);
						 if (rand()&1)
						 {
							 object.ai.vort.inertiay = -VORT_MAX_JUMP_HEIGHT;
						 }
						 else
						 {
							 object.ai.vort.inertiay = -VORT_MIN_JUMP_HEIGHT;
						 }
						 goto vort_reprocess;
					 }
					 else object.ai.vort.dist_traveled = 0;
				 }
			 }
			 // walk animation
			 if (object.ai.vort.animtimer > VORT_WALK_ANIM_TIME)
			 {
				 if (object.ai.vort.frame>=3) object.ai.vort.frame=0;
				 else object.ai.vort.frame++;
				 object.ai.vort.animtimer = 0;
			 } else object.ai.vort.animtimer++;
			 break;
		 case VORT_DYING:
			 object.sprite = object.ai.vort.DyingFrame + object.ai.vort.frame;

			 if (object.ai.vort.animtimer > VORT_DIE_ANIM_TIME)
			 {
				 object.ai.vort.frame++;
				 if (object.ai.vort.frame>=6)
				 {
					 object.ai.vort.frame = 5;
					 g_pGfxEngine->Palette.fadeto(0, FADE_SPEED_VERY_SLOW);
					 if(!g_pGfxEngine->Palette.in_progress())
						 object.ai.vort.state = VORT_DEAD;
				 }
	 
				 object.ai.vort.animtimer = 0;
			 } else object.ai.vort.animtimer++;
			 break;
		 case VORT2_DYING:
			 object.sprite = object.ai.vort.DyingFrame;
			 if (object.ai.vort.animtimer > VORT2_DIE_ANIM_TIME)
			 {
				 object.sprite = object.ai.vort.DeadFrame;
				 object.ai.vort.state = VORT_DEAD;
			 }
			 else
			 {
				 object.ai.vort.animtimer++;
			 }
			 break;
		 default: break;
		 }
}
	 
void CObjectAI::vort_initiatejump(CObject &object)
{
	 // must be standing on floor to jump
	 if (!object.blockedd) return;
	 
	 object.ai.vort.frame = 0;
	 object.ai.vort.animtimer = 0;
	 object.ai.vort.inertiay =
	 -((rnd()%(VORT_MAX_JUMP_HEIGHT-VORT_MIN_JUMP_HEIGHT))+VORT_MIN_JUMP_HEIGHT);
	 
	 if (object.ai.vort.movedir==RIGHT)
		 object.sprite = object.ai.vort.JumpRightFrame;
	 else
		 object.sprite = object.ai.vort.JumpLeftFrame;
	 
	 object.inhibitfall = true;
	 object.ai.vort.state = VORT_JUMP;
}
