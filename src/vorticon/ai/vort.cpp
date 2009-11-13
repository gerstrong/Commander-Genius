#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"
#include "../../graphics/CGfxEngine.h"
#include "vort.h"

// Vorticon (all episodes, albeit the behavior changes slightly
// depending on levelcontrol.episode).

// Reference to ../misc.cpp
unsigned int rnd(void);

void CObjectAI::vort_ai(CObject *p_object, int level, int episode, char difficulty, bool dark)
{
	Uint32 bboxX1 = g_pGfxEngine->Sprite.at(p_object->sprite)->m_bboxX1;
	Uint32 bboxX2 = g_pGfxEngine->Sprite.at(p_object->sprite)->m_bboxX2;
	Uint32 bboxY1 = g_pGfxEngine->Sprite.at(p_object->sprite)->m_bboxY1;
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	bool bonk,kill;
	 if (p_object->needinit)
	 {  // first time initilization
		 p_object->ai.vort.frame = 0;
		 p_object->ai.vort.animtimer = 0;
		 p_object->ai.vort.state = VORT_LOOK;
		 p_object->ai.vort.timer = 0;
		 p_object->ai.vort.dist_traveled = VORT_TRAPPED_DIST+1;
		 p_object->canbezapped = 1;
		 p_object->needinit = 0;
	 
		 // give him some health points, depening on episode and level
		 if(episode == 1)
		 {
			 if(level == 16) // He is the vorticon commander and has much more HP
				 p_object->ai.vort.hp = 105;
			 else
				 p_object->ai.vort.hp = 4;
		 }
		 else
			 p_object->ai.vort.hp = (difficulty > 3) ? 4 : 1;
		 // In EP2 and 3 the Vorticons are much weaker, when no hardmode selected
	 
		 // copy in animation frame indexes for the current ep
		 if (episode==1)
		 {
			 p_object->ai.vort.WalkLeftFrame = VORT1_WALK_LEFT_FRAME;
			 p_object->ai.vort.WalkRightFrame = VORT1_WALK_RIGHT_FRAME;
			 p_object->ai.vort.LookFrame = VORT1_LOOK_FRAME;
			 p_object->ai.vort.JumpRightFrame = VORT1_JUMP_RIGHT_FRAME;
			 p_object->ai.vort.JumpLeftFrame = VORT1_JUMP_LEFT_FRAME;
			 p_object->ai.vort.DyingFrame = VORT1_DYING_FRAME;
		 }
		 else if (episode==2)
		 {
			 p_object->ai.vort.WalkLeftFrame = VORT2_WALK_LEFT_FRAME;
			 p_object->ai.vort.WalkRightFrame = VORT2_WALK_RIGHT_FRAME;
			 p_object->ai.vort.LookFrame = VORT2_LOOK_FRAME;
			 p_object->ai.vort.JumpRightFrame = VORT2_JUMP_RIGHT_FRAME;
			 p_object->ai.vort.JumpLeftFrame = VORT2_JUMP_LEFT_FRAME;
			 p_object->ai.vort.DyingFrame = VORT2_DYING_FRAME;
			 p_object->ai.vort.DeadFrame = VORT2_DEAD_FRAME;
		 }
		 else if (episode==3)
		 {
			 p_object->ai.vort.WalkLeftFrame = VORT3_WALK_LEFT_FRAME;
			 p_object->ai.vort.WalkRightFrame = VORT3_WALK_RIGHT_FRAME;
			 p_object->ai.vort.LookFrame = VORT3_LOOK_FRAME;
			 p_object->ai.vort.JumpRightFrame = VORT3_JUMP_RIGHT_FRAME;
			 p_object->ai.vort.JumpLeftFrame = VORT3_JUMP_LEFT_FRAME;
			 p_object->ai.vort.DyingFrame = VORT3_DYING_FRAME;
			 p_object->ai.vort.DeadFrame = VORT3_DEAD_FRAME;
		 }
	 }
	 
	 if (p_object->ai.vort.state==VORT_DEAD)   return;

	 if (p_object->canbezapped)
	 {
		 kill = false;
		 // if we touch a glowcell, we die!
		 if ( p_object->zapped >= p_object->ai.vort.hp ) kill = true;
		 else if (episode==2 && mp_Map->at((p_object->x + bboxX1)>>CSF, (p_object->y+bboxY1)>>CSF)==TILE_GLOWCELL)
			 kill = true;
	 
		 if (kill)
		 {
			 p_object->inhibitfall = false;
			 p_object->canbezapped = false;
			 p_object->ai.vort.animtimer = 0;
			 p_object->ai.vort.frame = 0;
			 p_object->ai.vort.palflashtimer = VORT_PALETTE_FLASH_TIME + 1;
			 p_object->ai.vort.palflashamt = 255;
			 if (episode == 1)
			 {
				 // TODO: Flash animation must be performed here!
				 p_object->ai.vort.state = VORT_DYING;
			 }
			 else
				 p_object->ai.vort.state = VORT2_DYING;
	 
			 g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, p_object->scrx);
		 }
	 }
	 // deadly to the touch
	 if (p_object->touchPlayer && p_object->canbezapped)
		 killplayer(p_object->touchedBy);
	 
	 vort_reprocess: ;
	 switch(p_object->ai.vort.state)
	 {
	 case VORT_JUMP:
		 if (p_object->ai.vort.movedir == RIGHT)
		 {
			 if (!p_object->blockedr) p_object->x += VORT_WALK_SPEED;
		 }
		 else
		 {
			 if (!p_object->blockedl) p_object->x -= VORT_WALK_SPEED;
		 }
	 
		 if (p_object->ai.vort.inertiay>0 && p_object->blockedd)
		 {  // The Vorticon Has Landed!
			 p_object->inhibitfall = 0;
			 p_object->ai.vort.state = VORT_LOOK;
			 goto vort_reprocess;
		 }
		 // check if the vorticon has bonked into a ceiling, if so,
		 // immediately terminate the jump
		 bonk = false;
	 
		 if (TileProperty[mp_Map->at((p_object->x + bboxX1)>>CSF, (p_object->y+bboxY1)>>CSF)].bdown) bonk = true;
		 else if (TileProperty[mp_Map->at((p_object->x + bboxX2)>>CSF, (p_object->y + bboxY1)>>CSF)].bdown) bonk = true;
	 
		 if (bonk && p_object->ai.vort.inertiay < 0)
		 {
			 p_object->ai.vort.inertiay = 0;
		 }
	 
		 // apply Y inertia
		 p_object->y += p_object->ai.vort.inertiay;
	 
		 if (p_object->ai.vort.timer > VORT_JUMP_FRICTION)
		 { // slowly decrease upgoing rate
			 if (p_object->ai.vort.inertiay<VORT_MAX_FALL_SPEED)
				 p_object->ai.vort.inertiay+=1<<4;

			 p_object->ai.vort.timer = 0;
		 }
		 else p_object->ai.vort.timer++;
		 break;
	 case VORT_LOOK:
			 p_object->sprite = p_object->ai.vort.LookFrame + p_object->ai.vort.frame;
	 
			 if (p_object->ai.vort.animtimer > VORT_LOOK_ANIM_TIME)
			 {
				 if (p_object->ai.vort.frame>0)
				 {
					 if (p_object->blockedl)
					 { p_object->ai.vort.movedir = RIGHT; }
					 else if (p_object->blockedr)
					 { p_object->ai.vort.movedir = LEFT; }
					 else
					 { // not blocked on either side, head towards player
						 if ( mp_Player[0].x < p_object->x)
						 { p_object->ai.vort.movedir = LEFT; }
						 else
						 { p_object->ai.vort.movedir = RIGHT; }
					 }
					 p_object->ai.vort.timer = 0;
					 p_object->ai.vort.frame = 0;
					 p_object->ai.vort.state = VORT_WALK;
				 } else p_object->ai.vort.frame++;
				 p_object->ai.vort.animtimer = 0;
			 } else p_object->ai.vort.animtimer++;
			 break;
	 case VORT_WALK:
			 p_object->ai.vort.dist_traveled++;
	 
			 if (rnd()%VORT_JUMP_PROB == (VORT_JUMP_PROB/2))
			 {  // let's jump.
				 if (!dark && !p_object->blockedu)
				 {
					 vort_initiatejump(p_object);
					 goto vort_reprocess;
				 }
			 }
			 if (p_object->ai.vort.movedir==LEFT)
			 {  // move left
				 p_object->sprite = p_object->ai.vort.WalkLeftFrame + p_object->ai.vort.frame;
	 
				 if (!p_object->blockedl)
				 {
					 p_object->x -= VORT_WALK_SPEED;
				 }
				 else
				 {
					 p_object->ai.vort.frame = 0;
					 p_object->ai.vort.animtimer = 0;
					 p_object->ai.vort.state = VORT_LOOK;
	 
					 // if we only traveled a tiny amount before hitting a wall, we've
					 // probably fallen into a small narrow area, and we need to try
					 // to jump out of it
					 if (p_object->ai.vort.dist_traveled < VORT_TRAPPED_DIST && !dark && p_object->blockedd && !p_object->blockedu)
					 {
						 vort_initiatejump(p_object);
						 if (rnd()&1)
						 {
							 p_object->ai.vort.inertiay = -VORT_MAX_JUMP_HEIGHT;
						 }
						 else
						 {
							 p_object->ai.vort.inertiay = -VORT_MIN_JUMP_HEIGHT;
						 }
						 goto vort_reprocess;
					 }
					 else p_object->ai.vort.dist_traveled = 0;
				 }
			 }
			 else
			 {  // move right
				 p_object->sprite = p_object->ai.vort.WalkRightFrame + p_object->ai.vort.frame;

				 if (!p_object->blockedr)
				 {
					 p_object->x += VORT_WALK_SPEED;
				 }
				 else
				 {
					 p_object->ai.vort.frame = 0;
					 p_object->ai.vort.animtimer = 0;
					 p_object->ai.vort.state = VORT_LOOK;
	 
					 if (p_object->ai.vort.dist_traveled < VORT_TRAPPED_DIST && !dark && p_object->blockedd && !p_object->blockedu)
					 {
						 vort_initiatejump(p_object);
						 if (rand()&1)
						 {
							 p_object->ai.vort.inertiay = -VORT_MAX_JUMP_HEIGHT;
						 }
						 else
						 {
							 p_object->ai.vort.inertiay = -VORT_MIN_JUMP_HEIGHT;
						 }
						 goto vort_reprocess;
					 }
					 else p_object->ai.vort.dist_traveled = 0;
				 }
			 }
			 // walk animation
			 if (p_object->ai.vort.animtimer > VORT_WALK_ANIM_TIME)
			 {
				 if (p_object->ai.vort.frame>=3) p_object->ai.vort.frame=0;
				 else p_object->ai.vort.frame++;
				 p_object->ai.vort.animtimer = 0;
			 } else p_object->ai.vort.animtimer++;
			 break;
		 case VORT_DYING:
			 p_object->sprite = p_object->ai.vort.DyingFrame + p_object->ai.vort.frame;

			 if (p_object->ai.vort.animtimer > VORT_DIE_ANIM_TIME)
			 {
				 p_object->ai.vort.frame++;
				 if (p_object->ai.vort.frame>=6)
				 {
					 p_object->ai.vort.frame = 5;
					 g_pGfxEngine->Palette.fadeto(0, FADE_SPEED_VERY_SLOW);
					 if(!g_pGfxEngine->Palette.in_progress())
						 p_object->ai.vort.state = VORT_DEAD;
				 }
	 
				 p_object->ai.vort.animtimer = 0;
			 } else p_object->ai.vort.animtimer++;
			 break;
		 case VORT2_DYING:
			 p_object->sprite = p_object->ai.vort.DyingFrame;
			 if (p_object->ai.vort.animtimer > VORT2_DIE_ANIM_TIME)
			 {
				 p_object->sprite = p_object->ai.vort.DeadFrame;
				 p_object->ai.vort.state = VORT_DEAD;
			 }
			 else
			 {
				 p_object->ai.vort.animtimer++;
			 }
			 break;
		 default: break;
		 }
}
	 
void CObjectAI::vort_initiatejump(CObject *p_object)
{
	 // must be standing on floor to jump
	 if (!p_object->blockedd) return;
	 
	 p_object->ai.vort.frame = 0;
	 p_object->ai.vort.animtimer = 0;
	 p_object->ai.vort.inertiay =
	 -((rnd()%(VORT_MAX_JUMP_HEIGHT-VORT_MIN_JUMP_HEIGHT))+VORT_MIN_JUMP_HEIGHT);
	 
	 if (p_object->ai.vort.movedir==RIGHT)
		 p_object->sprite = p_object->ai.vort.JumpRightFrame;
	 else
		 p_object->sprite = p_object->ai.vort.JumpLeftFrame;
	 
	 p_object->inhibitfall = true;
	 p_object->ai.vort.state = VORT_JUMP;
}
