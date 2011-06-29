#include "CGarg.h"
#include "sdl/sound/CSound.h"

#include <vector>

CGarg::CGarg(CMap *p_map, std::vector<CPlayer> &m_vec_Player,
		Uint32 x, Uint32 y) :
CObject(p_map, x,y,OBJ_GARG),
state(GARG_LOOK),
looktimes(GARG_NUM_LOOKS+1),
lookframe(0),
timer(0),
keenonsameleveltimer(0),
about_to_charge(0),
walkframe(0),
dist_traveled(0),
movedir(0),
detectedPlayer(0),
detectedPlayerIndex(0),
m_Player(m_vec_Player),
m_hardmode(p_map->m_Difficulty > 2)
{
	canbezapped = true;
	if(mp_Map->m_Difficulty > 2)
		HealthPoints++;
}

void CGarg::process()
{
	// kill player on touch
	if (state!=GARG_DYING && touchPlayer)
		m_Player[touchedBy].kill();

	// did the garg get shot?
	if (HealthPoints <= 0 && state != GARG_DYING )
	{
		// die, you stupid garg, die!
		state = GARG_DYING;
		dying = true;
		sprite = GARG_DYING_FRAME;
		yinertia = GARGDIE_START_INERTIA;
		playSound(SOUND_GARG_DIE);
	}

	// Check, if Garg is moving and collides and change directions whenever needed
	if(state != GARG_LOOK &&  state != GARG_CHARGE && state != GARG_DYING)
	{
		// Set the proper frame
		sprite = (movedir==LEFT) ? GARG_WALK_LEFT : GARG_WALK_RIGHT;
		sprite += walkframe;

		// collides? Change direction
		if( movedir==LEFT && blockedl )
			movedir = RIGHT;
		else if( movedir==RIGHT && blockedr )
			movedir = LEFT;

		// Compute the speed
		xinertia = (m_hardmode) ? GARG_WALK_SPEED_FAST : GARG_WALK_SPEED;

		// Set the correct speed according to direction
		if(movedir == LEFT)
			xinertia = -xinertia;
		dist_traveled++;
	}

	switch(state)
	{
	case GARG_DYING:
		if ( blockedd && yinertia >= 0 )
		{
			sprite = GARG_DEAD_FRAME;
			dead = true;
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
		for( size_t i=0 ; it != m_Player.end() ; it++ )
		{
			if ( it->getYDownPos() >= getYDownPos()-(16<<STC) )
			{
				if ( it->getYDownPos() <= getYDownPos()+(16<<STC) )
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

		if( blockedd ) // There is floor
			state = GARG_CHARGE;

		charge();
		break;
	case GARG_CHARGE:
		charge();

		// if Garg is about to fall while charged make him jump
		if( !blockedd )
		{
			state = GARG_JUMP;
			yinertia = -(GARG_JUMP_SPEED);
			jumptime = GARG_JUMP_TIME;
		}

		break;
	default: break;
	}
}

void CGarg::charge()
{
	if (movedir==LEFT)
	{  // garg is charging left
		sprite = GARG_WALK_LEFT + walkframe;
		if (!blockedl)
		{
			xinertia = -GARG_CHARGE_SPEED;
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
			xinertia = GARG_CHARGE_SPEED;
			dist_traveled++;
		}
		else
		{
			looktimes = 0;
			timer = 0;
			state = GARG_MOVE;
		}
	}

	// walk animation
	if (timer > GARG_CHARGE_ANIM_TIME)
	{
		walkframe ^= 1;
		timer = 0;
	} else timer++;
}
