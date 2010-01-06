#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"

enum garg_states{
	GARG_LOOK, GARG_MOVE, GARG_CHARGE,
	GARG_JUMP, GARG_DYING, GARG_DEAD
};

#define GARG_MINTRAVELDIST          1000
#define GARG_LOOK_PROB              100
#define GARG_WALK_SPEED             12
#define GARG_WALK_ANIM_TIME         12
#define GARG_WALK_SPEED_FAST        20
#define GARG_WALK_ANIM_TIME_FAST    7
#define GARG_CHARGE_SPEED           73
#define GARG_CHARGE_ANIM_TIME       7
#define GARG_JUMP_TIME       		160
#define GARG_JUMP_SPEED				48

#define GARG_LOOK_TIME  17
#define GARG_NUM_LOOKS  3

// amount of time keen must be on same level before garg will charge
#define GARG_SAME_LEVEL_TIME     50

#define GARG_STAND         60
#define GARG_WALK_RIGHT    64
#define GARG_WALK_LEFT     66
#define GARG_DYING_FRAME   68
#define GARG_DEAD_FRAME    69

#define GARGDIE_START_INERTIA      -80

// Reference to ../misc.cpp
unsigned int rnd(void);

void CObjectAI::garg_ai(CObject &object, CPlayer *p_player, bool hardmode)
{
	unsigned int i;
    stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	if (object.needinit)
	{  // first time initialization
		object.ai.garg.state = GARG_LOOK;
		object.ai.garg.looktimes = GARG_NUM_LOOKS+1;
		object.ai.garg.timer = 0;
		object.ai.garg.lookframe = 0;
		object.ai.garg.keenonsameleveltimer = 0;
		object.ai.garg.about_to_charge = 0;
		object.ai.garg.detectedPlayer = 0;
		object.ai.garg.detectedPlayerIndex = 0;
		object.canbezapped = 1;
		object.needinit = 0;
	}
	if (object.ai.garg.state==GARG_DEAD) return;

	// kill player on touch
	if (object.ai.garg.state!=GARG_DYING && object.touchPlayer)
		killplayer(object.touchedBy);

	// did the garg get shot?
	if (object.zapped)
	{
		// die, you stupid garg, die!
		object.ai.garg.state = GARG_DYING;
		object.canbezapped = 0;
		object.sprite = GARG_DYING_FRAME;
		object.zapped=0;
		object.ai.garg.gargdie_inertia_y = GARGDIE_START_INERTIA;
		object.moveUp(10);
		object.inhibitfall = 1;
		g_pSound->playStereofromCoord(SOUND_GARG_DIE, PLAY_NOW, object.scrx);
	}

	switch(object.ai.garg.state)
	{
		case GARG_DYING:
			object.moveDown(object.ai.garg.gargdie_inertia_y);
			object.ai.garg.gargdie_inertia_y+=16;

			if (object.ai.garg.gargdie_inertia_y >= 0 && object.blockedd)
			{
				object.sprite = GARG_DEAD_FRAME;
				object.inhibitfall = 0;
				object.ai.garg.state = GARG_DEAD;
			}
			break;
		case GARG_LOOK:
			if (object.ai.garg.looktimes>GARG_NUM_LOOKS)
			{
				// try to head towards Keen...
				if (p_player[object.ai.garg.detectedPlayerIndex].getXPosition() < object.getYPosition())
					object.ai.garg.movedir = LEFT;
				else
					object.ai.garg.movedir = RIGHT;

				if (!object.ai.garg.about_to_charge && rnd()%3==1)
				// 25% prob, go the other way (but always charge towards player)
				object.ai.garg.movedir ^= 1;

				// however if we're blocked on one side we must go the other way
				if (object.blockedl)
					object.ai.garg.movedir = RIGHT;
				else if (object.blockedr)
					object.ai.garg.movedir = LEFT;

				object.ai.garg.timer = 0;
				object.ai.garg.walkframe = 0;
				object.ai.garg.dist_traveled = 0;
				if (object.ai.garg.about_to_charge)
				{
					object.ai.garg.state = GARG_CHARGE;
					object.ai.garg.about_to_charge = 0;
				}
				else object.ai.garg.state = GARG_MOVE;
			}

			// look animation
			if (!object.ai.garg.timer)
			{
				object.sprite = GARG_STAND + object.ai.garg.lookframe;
				object.ai.garg.looktimes++;

				if (++object.ai.garg.lookframe>3)
				{
					object.ai.garg.lookframe=0;
				}
				object.ai.garg.timer = GARG_LOOK_TIME;
			} else object.ai.garg.timer--;
			break;
		case GARG_MOVE:
			// is keen on same level?
			object.ai.garg.detectedPlayer = 0;
			for(i=0;i<1;i++) //TODO cycle through players
			{
				if (p_player[i].getYPosition() >= object.getYPosition()-(8<<STC))
				{
					if ((p_player[i].getYDownPos()) <= (object.getYDownPos()+(1<<CSF)))
					{
						object.ai.garg.detectedPlayer = 1;
						object.ai.garg.detectedPlayerIndex = i;
						break;
					}
				}
			}

			if (object.ai.garg.detectedPlayer)
			{
				object.ai.garg.keenonsameleveltimer++;
				if (object.ai.garg.keenonsameleveltimer > GARG_SAME_LEVEL_TIME)
				{ // charge!!
					object.ai.garg.keenonsameleveltimer = 0;
					object.ai.garg.timer = 0;
					object.ai.garg.looktimes = 1;
					object.ai.garg.about_to_charge = 1;
					object.ai.garg.state = GARG_LOOK;
				}
			} else object.ai.garg.keenonsameleveltimer = 0;

			// every now and then go back to look state
			if (object.ai.garg.dist_traveled > GARG_MINTRAVELDIST)
			{
				if (rnd()%GARG_LOOK_PROB==(GARG_LOOK_PROB/2))
				{
					object.ai.garg.looktimes = 0;
					object.ai.garg.timer = 0;
					object.ai.garg.state = GARG_LOOK;
					break;
				}
			}

			if (object.ai.garg.movedir==LEFT)
			{  // garg is walking left
				object.sprite = GARG_WALK_LEFT + object.ai.garg.walkframe;

				if (!object.blockedl)
				{
					if (hardmode)
						object.moveLeft(GARG_WALK_SPEED_FAST);
					else
						object.moveLeft(GARG_WALK_SPEED);
					object.ai.garg.dist_traveled++;
				}
				else
				{
					object.ai.garg.looktimes = 0;
					object.ai.garg.timer = 0;
					object.ai.garg.state = GARG_LOOK;
				}
			}
			else
			{  // garg is walking right
				object.sprite = GARG_WALK_RIGHT + object.ai.garg.walkframe;
				if (!object.blockedr)
				{
					if (hardmode)
						object.moveRight(GARG_WALK_SPEED_FAST);
					else
						object.moveRight(GARG_WALK_SPEED);
					object.ai.garg.dist_traveled++;
				}
				else
				{
					object.ai.garg.looktimes = 0;
					object.ai.garg.timer = 0;
					object.ai.garg.state = GARG_LOOK;
				}
			}

			// walk animation
			if (object.ai.garg.timer > GARG_WALK_ANIM_TIME ||
			   (object.ai.garg.timer > GARG_WALK_ANIM_TIME && hardmode))
			{
				object.ai.garg.walkframe ^= 1;
				object.ai.garg.timer = 0;
			} else object.ai.garg.timer++;
			break;
		case GARG_JUMP:
			if( object.ai.garg.jumptime > 0 )
				object.ai.garg.jumptime--;
			else
				object.ai.garg.state = GARG_CHARGE;

			if(TileProperty[mp_Map->at((object.getXMidPos())>>CSF, (object.getYDownPos()+1)>>CSF)].bup) // There is floor
				object.ai.garg.state = GARG_CHARGE;
			else
			{
				object.moveUp(GARG_JUMP_SPEED);
			}

		// In this case continue with charge
		case GARG_CHARGE:
			if (object.ai.garg.movedir==LEFT)
			{  // garg is charging left
				object.sprite = GARG_WALK_LEFT + object.ai.garg.walkframe;
				if (!object.blockedl)
				{
					object.moveLeft(GARG_CHARGE_SPEED);
					object.ai.garg.dist_traveled++;
				}
				else
				{
					object.ai.garg.looktimes = 0;
					object.ai.garg.timer = 0;
					object.ai.garg.state = GARG_LOOK;
				}
			}
			else
			{  // garg is charging right
				object.sprite = GARG_WALK_RIGHT + object.ai.garg.walkframe;
				if (!object.blockedr)
				{
					object.moveRight(GARG_CHARGE_SPEED);
					object.ai.garg.dist_traveled++;
				}
				else
				{
					object.ai.garg.looktimes = 0;
					object.ai.garg.timer = 0;
					object.ai.garg.state = GARG_LOOK;
				}
			}

			// if Garg is about to fall while charged make him jump
			if( !TileProperty[mp_Map->at((object.getXMidPos())>>CSF, (object.getYDownPos()+1)>>CSF)].bup )
			{
				object.ai.garg.state = GARG_JUMP;
				object.ai.garg.jumptime = GARG_JUMP_TIME;
			}

			// walk animation
			if (object.ai.garg.timer > GARG_CHARGE_ANIM_TIME)
			{
				object.ai.garg.walkframe ^= 1;
				object.ai.garg.timer = 0;
			} else object.ai.garg.timer++;
			break;
	}
}
