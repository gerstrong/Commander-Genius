#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"

enum garg_states{
	GARG_LOOK,
	GARG_MOVE,
	GARG_CHARGE,
	GARG_JUMP,
	GARG_DYING,
	GARG_DEAD
};

#define GARG_MINTRAVELDIST          1000
#define GARG_LOOK_PROB              800
#define GARG_WALK_SPEED             3
#define GARG_WALK_ANIM_TIME         50
#define GARG_WALK_SPEED_FAST        5
#define GARG_WALK_ANIM_TIME_FAST    30
#define GARG_CHARGE_SPEED           14
#define GARG_CHARGE_ANIM_TIME       30
#define GARG_JUMP_HEIGHT            40

#define GARG_LOOK_TIME  70
#define GARG_NUM_LOOKS  3

// amount of time keen must be on same level before garg will charge
#define GARG_SAME_LEVEL_TIME     200

#define GARG_STAND         60
#define GARG_WALK_RIGHT    64
#define GARG_WALK_LEFT     66
#define GARG_DYING_FRAME   68
#define GARG_DEAD_FRAME    69

#define GARGDIE_START_INERTIA      -10
#define GARGDIE_INERTIA_DECREASE    2

unsigned int rnd(void);

void CObjectAI::garg_ai(CObject *p_object, CPlayer *p_player, bool hardmode)
{
	unsigned int i;
	Uint16 garg_width = g_pGfxEngine->Sprite[p_object->sprite]->getWidth();
	Uint16 garg_height = g_pGfxEngine->Sprite[p_object->sprite]->getHeight();
	Uint16 player_height = g_pGfxEngine->Sprite[0]->getHeight();
    stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	if (p_object->needinit)
	{  // first time initilization
		p_object->ai.garg.state = GARG_LOOK;
		p_object->ai.garg.looktimes = GARG_NUM_LOOKS+1;
		p_object->ai.garg.timer = 0;
		p_object->ai.garg.lookframe = 0;
		p_object->ai.garg.keenonsameleveltimer = 0;
		p_object->ai.garg.about_to_charge = 0;
		p_object->ai.garg.detectedPlayer = 0;
		p_object->ai.garg.detectedPlayerIndex = 0;
		p_object->canbezapped = 1;
		p_object->needinit = 0;
	}
	if (p_object->ai.garg.state==GARG_DEAD) return;

	// kill player on touch
	if (p_object->ai.garg.state!=GARG_DYING && p_object->touchPlayer)
		killplayer(p_object->touchedBy);

	// did the garg get shot?
	if (p_object->zapped)
	{
		// die, you stupid garg, die!
		p_object->ai.garg.state = GARG_DYING;
		p_object->ai.garg.dietimer = 0;
		p_object->canbezapped = 0;
		p_object->sprite = GARG_DYING_FRAME;
		p_object->zapped=0;
		p_object->ai.garg.gargdie_inertia_y = GARGDIE_START_INERTIA;
		p_object->y -= 10;
		p_object->inhibitfall = 1;
		g_pSound->playStereofromCoord(SOUND_GARG_DIE, PLAY_NOW, p_object->scrx);
	}

	switch(p_object->ai.garg.state)
	{
		case GARG_DYING:
			p_object->y += p_object->ai.garg.gargdie_inertia_y;
			if (p_object->ai.garg.dietimer>GARGDIE_INERTIA_DECREASE)
			{
				p_object->ai.garg.gargdie_inertia_y++;
				p_object->ai.garg.dietimer = 0;
			}
			else p_object->ai.garg.dietimer++;

			if (p_object->ai.garg.gargdie_inertia_y >= 0 && p_object->blockedd)
			{
				p_object->sprite = GARG_DEAD_FRAME;
				p_object->inhibitfall = 0;
				p_object->ai.garg.state = GARG_DEAD;
			}
			break;
		case GARG_LOOK:
			if (p_object->ai.garg.looktimes>GARG_NUM_LOOKS)
			{
				// try to head towards Keen...
				if (p_player[p_object->ai.garg.detectedPlayerIndex].x < p_object->x)
					p_object->ai.garg.movedir = LEFT;
				else
					p_object->ai.garg.movedir = RIGHT;

				if (!p_object->ai.garg.about_to_charge && rnd()%3==1)
				// 25% prob, go the other way (but always charge towards player)
				p_object->ai.garg.movedir ^= 1;

				// however if we're blocked on one side we must go the other way
				if (p_object->blockedl)
					p_object->ai.garg.movedir = RIGHT;
				else if (p_object->blockedr)
					p_object->ai.garg.movedir = LEFT;

				p_object->ai.garg.timer = 0;
				p_object->ai.garg.walkframe = 0;
				p_object->ai.garg.dist_traveled = 0;
				if (p_object->ai.garg.about_to_charge)
				{
					p_object->ai.garg.state = GARG_CHARGE;
					p_object->ai.garg.about_to_charge = 0;
				}
				else p_object->ai.garg.state = GARG_MOVE;
			}

			// look animation
			if (!p_object->ai.garg.timer)
			{
				p_object->sprite = GARG_STAND + p_object->ai.garg.lookframe;
				p_object->ai.garg.looktimes++;

				if (++p_object->ai.garg.lookframe>3)
				{
					p_object->ai.garg.lookframe=0;
				}
				p_object->ai.garg.timer = GARG_LOOK_TIME;
			} else p_object->ai.garg.timer--;
			break;
		case GARG_MOVE:
			// is keen on same level?
			p_object->ai.garg.detectedPlayer = 0;
			for(i=0;i<1;i++) //TODO cycle through players
			{
				if (p_player[i].y >= p_object->y-(8<<CSF))
				{
					if ((p_player[i].y>>CSF)+player_height <= (p_object->y>>CSF)+garg_height+16)
					{
						p_object->ai.garg.detectedPlayer = 1;
						p_object->ai.garg.detectedPlayerIndex = i;
						break;
					}
				}
			}

			if (p_object->ai.garg.detectedPlayer)
			{
				p_object->ai.garg.keenonsameleveltimer++;
				if (p_object->ai.garg.keenonsameleveltimer > GARG_SAME_LEVEL_TIME)
				{ // charge!!
					p_object->ai.garg.keenonsameleveltimer = 0;
					p_object->ai.garg.timer = 0;
					p_object->ai.garg.looktimes = 1;
					p_object->ai.garg.about_to_charge = 1;
					p_object->ai.garg.state = GARG_LOOK;
				}
			} else p_object->ai.garg.keenonsameleveltimer = 0;

			// every now and then go back to look state
			if (p_object->ai.garg.dist_traveled > GARG_MINTRAVELDIST)
			{
				if (rnd()%GARG_LOOK_PROB==(GARG_LOOK_PROB/2))
				{
					p_object->ai.garg.looktimes = 0;
					p_object->ai.garg.timer = 0;
					p_object->ai.garg.state = GARG_LOOK;
					break;
				}
			}

			if (p_object->ai.garg.movedir==LEFT)
			{  // garg is walking left
				p_object->sprite = GARG_WALK_LEFT + p_object->ai.garg.walkframe;

				// do not go left if:
				// * we are blockedl, or
				// * there is empty space for two tiles ahead at floor level,
				//   and there is not a solid block 1-2 tiles ahead at wall level
				//not_about_to_fall1 = tiles[getmaptileat((p_object->x>>CSF)-1, (p_object->y>>CSF)+sprites[GARG_WALK_LEFT].ysize+2)].solidfall;
				//not_about_to_fall2 = tiles[getmaptileat((p_object->x>>CSF)-17, (p_object->y>>CSF)+sprites[GARG_WALK_LEFT].ysize+2)].solidfall;
				//GotoLook = 0;
				//if (p_object->blockedl)
					//GotoLook = 1;
				//else if (!(not_about_to_fall1 || not_about_to_fall2))
				//{
                //    blocked_ahead1 = tiles[getmaptileat((p_object->x>>CSF)-16, (p_object->y>>CSF)+20)].solidr;
				//	blocked_ahead2 = tiles[getmaptileat((p_object->x>>CSF)-28, (p_object->y>>CSF)+20)].solidr;
				//	if (!blocked_ahead1 && !blocked_ahead2)
						//GotoLook = 1;
				//}


				if (!p_object->blockedl)
				{
					if (hardmode)
						p_object->x -= GARG_WALK_SPEED_FAST;
					else
						p_object->x -= GARG_WALK_SPEED;
					p_object->ai.garg.dist_traveled++;
				}
				else
				{
					p_object->ai.garg.looktimes = 0;
					p_object->ai.garg.timer = 0;
					p_object->ai.garg.state = GARG_LOOK;
				}
			}
			else
			{  // garg is walking right
				p_object->sprite = GARG_WALK_RIGHT + p_object->ai.garg.walkframe;
				if (!p_object->blockedr)
				{
					if (hardmode)
						p_object->x += GARG_WALK_SPEED_FAST;
					else
						p_object->x += GARG_WALK_SPEED;
					p_object->ai.garg.dist_traveled++;
				}
				else
				{
					p_object->ai.garg.looktimes = 0;
					p_object->ai.garg.timer = 0;
					p_object->ai.garg.state = GARG_LOOK;
				}
			}

			// walk animation
			if (p_object->ai.garg.timer > GARG_WALK_ANIM_TIME ||
			   (p_object->ai.garg.timer > GARG_WALK_ANIM_TIME && hardmode))
			{
				p_object->ai.garg.walkframe ^= 1;
				p_object->ai.garg.timer = 0;
			} else p_object->ai.garg.timer++;
			break;
		case GARG_JUMP:
			if( p_object->ai.garg.jumpheight > 0 )
				p_object->ai.garg.jumpheight--;

			if(TileProperty[mp_Map->at((p_object->x>>CSF)+garg_width/2, (p_object->y>>CSF)+garg_height+1)].bdown) // There is floor
				p_object->ai.garg.state = GARG_CHARGE;
			else
				p_object->y-=12;

		case GARG_CHARGE:
			if (p_object->ai.garg.movedir==LEFT)
			{  // garg is charging left
				p_object->sprite = GARG_WALK_LEFT + p_object->ai.garg.walkframe;
				if (!p_object->blockedl)
				{
					p_object->x -= GARG_CHARGE_SPEED;
					p_object->ai.garg.dist_traveled++;
				}
				else
				{
					p_object->ai.garg.looktimes = 0;
					p_object->ai.garg.timer = 0;
					p_object->ai.garg.state = GARG_LOOK;
				}

				// if Garg is about to fall while charged make him jump
				if( TileProperty[mp_Map->at((p_object->x>>CSF)-garg_width/2, (p_object->y>>CSF)+garg_height+1)].bdown )
				{
					p_object->ai.garg.state = GARG_JUMP;
					p_object->ai.garg.jumpheight = GARG_JUMP_HEIGHT<<CSF;
				}
			}
			else
			{  // garg is charging right
				p_object->sprite = GARG_WALK_RIGHT + p_object->ai.garg.walkframe;
				if (!p_object->blockedr)
				{
					p_object->x += GARG_CHARGE_SPEED;
					p_object->ai.garg.dist_traveled++;
				}
				else
				{
					p_object->ai.garg.looktimes = 0;
					p_object->ai.garg.timer = 0;
					p_object->ai.garg.state = GARG_LOOK;
				}

				// if Garg is about to fall while charged make him jump
				if( TileProperty[mp_Map->at((p_object->x>>CSF)+garg_width/2, (p_object->y>>CSF)+garg_height+1)].bdown )
				{
					p_object->ai.garg.state = GARG_JUMP;
					p_object->ai.garg.jumpheight = GARG_JUMP_HEIGHT<<CSF;
				}
			}

			// walk animation
			if (p_object->ai.garg.timer > GARG_CHARGE_ANIM_TIME)
			{
				p_object->ai.garg.walkframe ^= 1;
				p_object->ai.garg.timer = 0;
			} else p_object->ai.garg.timer++;
			break;
	}
}
