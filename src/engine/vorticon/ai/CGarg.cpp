#include "CGarg.h"
#include "../../../sdl/sound/CSound.h"

#include <vector>

CGarg::CGarg(CMap *p_map, std::vector<CPlayer> &m_vec_Player,
		Uint32 x, Uint32 y) :
CObject(p_map),
state(GARG_LOOK),
looktimes(GARG_NUM_LOOKS+1),
lookframe(0),
timer(0),
keenonsameleveltimer(0),
about_to_charge(0),
walkframe(0),
dist_traveled(0),
gargdie_inertia_y(0),
movedir(0),
detectedPlayer(0),
detectedPlayerIndex(0),
m_Player(m_vec_Player),
m_hardmode(p_map->m_Difficulty>1)
{
	canbezapped = true;
}

void CGarg::process()
{
	if (state==GARG_DEAD) return;

	// kill player on touch
	if (state!=GARG_DYING && touchPlayer)
		m_Player[touchedBy].kill();

	// did the garg get shot?
	if (HealthPoints <= 0)
	{
		// die, you stupid garg, die!
		state = GARG_DYING;
		canbezapped = false;
		sprite = GARG_DYING_FRAME;
		gargdie_inertia_y = GARGDIE_START_INERTIA;
		moveUp(10);
		inhibitfall = 1;
		g_pSound->playStereofromCoord(SOUND_GARG_DIE, PLAY_NOW, scrx);
	}

	// Check, if Garg is moving and collides
	if(state != GARG_LOOK && state != GARG_CHARGE)
	{
		if (movedir==LEFT)
		{  // garg is heading left
			sprite = GARG_WALK_LEFT + walkframe;

			if (!blockedl)
			{
				if (m_hardmode)
					moveLeft(GARG_WALK_SPEED_FAST);
				else
					moveLeft(GARG_WALK_SPEED);
				dist_traveled++;
			}
			else if(blockedd)
				movedir = RIGHT;
		}
		else
		{  // garg is heading right
			sprite = GARG_WALK_RIGHT + walkframe;
			if (!blockedr)
			{
				if (m_hardmode)
					moveRight(GARG_WALK_SPEED_FAST);
				else
					moveRight(GARG_WALK_SPEED);
				dist_traveled++;
			}
			else if(blockedd)
				movedir = LEFT;
		}
	}

	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();

	switch(state)
	{
	case GARG_DYING:
		moveDown(gargdie_inertia_y);
		gargdie_inertia_y+=16;

		if (gargdie_inertia_y >= 0 && blockedd)
		{
			sprite = GARG_DEAD_FRAME;
			inhibitfall = 0;
			state = GARG_DEAD;
		}
		break;
	case GARG_LOOK:
		if (looktimes>GARG_NUM_LOOKS)
		{
			// try to head towards Keen...
			if (m_Player[detectedPlayerIndex].getXPosition() < getXPosition())
				movedir = LEFT;
			else
				movedir = RIGHT;

			if (!about_to_charge && rnd()%3==1)
				// 25% prob, go the other way (but always charge towards player)
				movedir ^= 1;

			// however if we're blocked on one side we must go the other way
			if (blockedl)
				movedir = RIGHT;
			else if (blockedr)
				movedir = LEFT;

			timer = 0;
			walkframe = 0;
			dist_traveled = 0;
			if (about_to_charge)
			{
				state = GARG_CHARGE;
				about_to_charge = 0;
			}
			else state = GARG_MOVE;
		}

		// look animation
		if (!timer)
		{
			sprite = GARG_STAND + lookframe;
			looktimes++;

			if (++lookframe>3)
			{
				lookframe=0;
			}
			timer = GARG_LOOK_TIME;
		} else timer--;
		break;
	case GARG_MOVE:
	{
		// is keen on same level?
		detectedPlayer = 0;

		std::vector<CPlayer>::iterator it = m_Player.begin();
		for( size_t i=0; it != m_Player.end() ; it++ )
		{
			if ( it->getYPosition() >= getYUpPos() )
			{
				if ( it->getYDownPos() <= getYDownPos() )
				{
					detectedPlayer = 1;
					detectedPlayerIndex = i;
					break;
				}
			}
			i++;
		}

		if (detectedPlayer)
		{
			keenonsameleveltimer++;
			if (keenonsameleveltimer > GARG_SAME_LEVEL_TIME)
			{ // charge!!
				keenonsameleveltimer = 0;
				timer = 0;
				looktimes = 1;
				about_to_charge = 1;
				state = GARG_LOOK;
			}
		} else keenonsameleveltimer = 0;

		// every now and then go back to look state
		if (dist_traveled > GARG_MINTRAVELDIST)
		{
			if (rnd()%GARG_LOOK_PROB==(GARG_LOOK_PROB/2))
			{
				looktimes = 0;
				timer = 0;
				state = GARG_LOOK;
				break;
			}
		}

		// walk animation
		if (timer > GARG_WALK_ANIM_TIME ||
				(timer > GARG_WALK_ANIM_TIME && m_hardmode))
		{
			walkframe ^= 1;
			timer = 0;
		} else timer++;
	}
	break;
	case GARG_JUMP:
		if( jumptime > 0 )
			jumptime--;
		else
			state = GARG_CHARGE;

		if(TileProperties.at(mp_Map->at((getXMidPos())>>CSF, (getYDownPos()+1)>>CSF)).bup) // There is floor
			state = GARG_CHARGE;
		else
		{
			moveUp(GARG_JUMP_SPEED);
		}

		// In this case continue with charge
	case GARG_CHARGE:
		if (movedir==LEFT)
		{  // garg is charging left
			sprite = GARG_WALK_LEFT + walkframe;
			if (!blockedl)
			{
				moveLeft(GARG_CHARGE_SPEED);
				dist_traveled++;
			}
			else
			{
				looktimes = 0;
				timer = 0;
				state = GARG_MOVE;
			}
		}
		else
		{  // garg is charging right
			sprite = GARG_WALK_RIGHT + walkframe;
			if (!blockedr)
			{
				moveRight(GARG_CHARGE_SPEED);
				dist_traveled++;
			}
			else
			{
				looktimes = 0;
				timer = 0;
				state = GARG_MOVE;
			}
		}

		// if Garg is about to fall while charged make him jump
		if( !TileProperties.at(mp_Map->at((getXMidPos())>>CSF, (getYDownPos()+1)>>CSF)).bup )
		{
			state = GARG_JUMP;
			jumptime = GARG_JUMP_TIME;
		}

		// walk animation
		if (timer > GARG_CHARGE_ANIM_TIME)
		{
			walkframe ^= 1;
			timer = 0;
		} else timer++;
		break;
	}
}

CGarg::~CGarg()
{}
