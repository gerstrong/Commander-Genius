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
CObject(p_map,x,y),
m_Player(Player),
m_Object(Object)
{
	int speed;

	this->vector_x = vector_x;
	this->vector_y = vector_y;

	if (vector_x && vector_y)
		speed = ICECHUNK_SPEED;
	else
		speed = ICECHUNK_STRAIGHT_SPEED;

	veloc_x = speed * vector_x;
	veloc_y = speed * vector_y;
	inhibitfall = true;
	canbezapped = 0;
	needinit = 0;
}

// the ice cannon itself
void CIceCannon::process()
{
	 // keep spawner object invisible and properly positioned
	 sprite = BLANKSPRITE;
	 inhibitfall = 1;

	if(m_gunfiretimer<GUNFIRE_TIMER) m_gunfiretimer++;
	else
	{
		int newpos_x = getXPosition()+(vector_x)*512;
		int newpos_y = getYPosition()+(vector_y)*512;
		CIceChunk *chunk = new CIceChunk(mp_Map, newpos_x, newpos_y, m_Player, m_Object);
		chunk->vector_x = vector_x;
		chunk->vector_y = vector_y;
		m_Object.push_back(chunk);
	}
}

CIceChunk::CIceChunk(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CPlayer>& Player, std::vector<CObject*>& Object) :
CObject(p_map, x, y),
m_Player(Player),
m_Object(Object)
{}

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
		CIceBit *chunk = new CIceBit(mp_Map, getXPosition(), getYPosition());

		g_pSound->playStereofromCoord(SOUND_CHUNKSMASH, PLAY_NOW, getXPosition());
		chunk->solid = false;

		// upleft
		chunk->vector_x = -1;
		chunk->vector_y = -1;
		m_Object.push_back(chunk);

		// upright
		chunk->vector_x = 1;
		chunk->vector_y = -1;
		m_Object.push_back(chunk);

		// downleft
		chunk->vector_x = -1;
		chunk->vector_y = 1;
		m_Object.push_back(chunk);

		// downright
		chunk->vector_x = 1;
		chunk->vector_y = 1;
		m_Object.push_back(chunk);
	}
	exists = false;
}


// the little pieces that break off of an OBJ_ICECHUNK when it hits
// a wall or a player. (Ep1)
#define ICEBIT_SPEED        80

CIceBit::CIceBit(CMap *p_map, Uint32 x, Uint32 y) :
CObject(p_map, x, y)
{ m_gunfiretimer=0; timer=0; }

void CIceBit::process()
{
	if (needinit)
	{  // first time initialization
		veloc_x = ICEBIT_SPEED * vector_x;
		veloc_y = ICEBIT_SPEED * vector_y;
		timer = 40;
		inhibitfall = true;
		canbezapped = false;
		needinit = false;
	}

	timer--;

	moveXDir(veloc_x);
	moveYDir(veloc_y);

	if (!onscreen or !m_gunfiretimer or timer <= 0)
		exists = false;
}


