#include "CIceCannon.h"
#include "../../spritedefines.h"
#include "../../../keen.h"
#include "../../../sdl/sound/CSound.h"

// the chunks of ice shot out by an ice cannon (ep1)
const int ICECHUNK_SPEED = 60;
const int ICECHUNK_STRAIGHT_SPEED = 80;
const int ICECHUNK_WAIT_TIME = 19;

unsigned int rnd(void);

CIceCannon::CIceCannon(CMap *p_map, Uint32 x, Uint32 y,
	std::vector<CPlayer>& Player, std::vector<CObject*>& Object,
	int vector_x, int vector_y ) :
CObject(p_map,x,y, OBJ_ICECANNON),
timer(0),
m_gunfiretimer(0),
m_Player(Player),
m_Object(Object)
{
	this->vector_x = vector_x;
	this->vector_y = vector_y;

	inhibitfall = true;
	canbezapped = false;
    sprite = BLANKSPRITE;
}

// the ice cannon itself
void CIceCannon::process()
{
	 // keep spawner object invisible and properly positioned

	if(m_gunfiretimer<GUNFIRE_TIMER_EP1) m_gunfiretimer++;
	else
	{
		int newpos_x = getXPosition()+(vector_x)*512;
		int newpos_y = getYPosition()+(vector_y+1)*512;
		CIceChunk *chunk = new CIceChunk(mp_Map, newpos_x, newpos_y,
								vector_x, vector_y, m_Player, m_Object);
		m_Object.push_back(chunk);
		m_gunfiretimer = 0;
	}
}

CIceChunk::CIceChunk(CMap *p_map, Uint32 x, Uint32 y, Uint32 vx, Uint32 vy,
		std::vector<CPlayer>& Player, std::vector<CObject*>& Object) :
CObject(p_map, x, y, OBJ_ICECHUNK),
vector_x(vx),
vector_y(vy),
m_Player(Player),
m_Object(Object)
{
	int speed;

	canbezapped = false;
	inhibitfall = true;

	if (vector_x && vector_y)
		speed = ICECHUNK_SPEED;
	else
		speed = ICECHUNK_STRAIGHT_SPEED;

	veloc_x = speed * vector_x;
	veloc_y = speed * vector_y;
	g_pSound->playStereofromCoord(SOUND_CANNONFIRE, PLAY_NOW, getXPosition());
}

void CIceChunk::process()
{
	// freeze the player if it touches him
	if (touchPlayer)
	{
		CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();
		// make him start sliding in the direction of the impact
		if (vector_x > 0)
		{
			m_Player[touchedBy].pdir = m_Player[touchedBy].pshowdir = RIGHT;
			m_Player[touchedBy].xinertia = Physics.player.max_x_speed;
			m_Player[touchedBy].bump(Physics.player.max_x_speed/2, true);
		}
		else if (vector_x < 0)
		{
			m_Player[touchedBy].pdir = m_Player[touchedBy].pshowdir = LEFT;
			m_Player[touchedBy].xinertia = -Physics.player.max_x_speed;
			m_Player[touchedBy].bump(-Physics.player.max_x_speed/2, true);
		}
		else	// perfectly vertical ice cannons
		{
			const int UPDNCANNON_PUSHAMT = 16;
			if (m_Player[touchedBy].xinertia < UPDNCANNON_PUSHAMT)
			{
				if (rnd()&1)
					m_Player[touchedBy].xinertia = UPDNCANNON_PUSHAMT;
				else
					m_Player[touchedBy].xinertia = -UPDNCANNON_PUSHAMT;
			}
		}

		m_Player[touchedBy].freeze();
		smash();
		return;
	}

	// smash the chunk if it hits something
	if (vector_x > 0)
	{
		if (blockedr) { smash(); return; }
	}
	else if (vector_x < 0)
	{
		if (blockedl) { smash(); return; }
	}

	if (vector_y > 0)
	{
		if (blockedd) { smash(); return; }
	}
	else if (vector_y < 0)
	{
		if (blockedu) { smash(); return; }
	}

	// fly through the air
	moveXDir(veloc_x);
	moveYDir(veloc_y);
}

void CIceChunk::smash()
{
	if (onscreen)
	{
		g_pSound->playStereofromCoord(SOUND_CHUNKSMASH, PLAY_NOW, getXPosition());
		CIceBit *chunk;

		// upleft
		chunk = new CIceBit(mp_Map, getXPosition(), getYPosition(), -1, -1);
		m_Object.push_back(chunk);

		// upright
		chunk = new CIceBit(mp_Map, getXPosition(), getYPosition(), 1, -1);
		m_Object.push_back(chunk);

		// downleft
		chunk = new CIceBit(mp_Map, getXPosition(), getYPosition(), -1, 1);
		m_Object.push_back(chunk);

		// downright
		chunk = new CIceBit(mp_Map, getXPosition(), getYPosition(), 1, 1);
		m_Object.push_back(chunk);
	}
	exists = false;
}


// the little pieces that break off of an OBJ_ICECHUNK when it hits
// a wall or a player. (Ep1)
#define ICEBIT_SPEED        80

CIceBit::CIceBit(CMap *p_map, Uint32 x, Uint32 y, Uint32 vec_x, Uint32 vec_y ) :
CObject(p_map, x, y, OBJ_ICEBIT),
vector_x(vec_x),
vector_y(vec_y)
{
	m_gunfiretimer=0; timer=0;
	veloc_x = ICEBIT_SPEED * vector_x;
	veloc_y = ICEBIT_SPEED * vector_y;
	timer = GUNFIRE_TIMER_EP1;
	inhibitfall = true;
	canbezapped = false;
	solid = false;
}

void CIceBit::process()
{
	moveXDir(veloc_x);
	moveYDir(veloc_y);

	if (timer <= 0)
		exists = false;
	else
		timer--;
}


