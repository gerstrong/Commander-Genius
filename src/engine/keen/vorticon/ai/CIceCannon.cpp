#include "CIceCannon.h"
#include "engine/spritedefines.h"
#include "sdl/sound/CSound.h"

// the chunks of ice shot out by an ice cannon (ep1)
const int ICECHUNK_SPEED = 60;
const int ICECHUNK_STRAIGHT_SPEED = 80;
const int ICECHUNK_WAIT_TIME = 19;

unsigned int rnd(void);

const int GUNFIRE_TIMER_EP1 = 64;

const int SILENT_DIST = 20<<CSF;


CIceCannon::CIceCannon(CMap *p_map, Uint32 x, Uint32 y,
	int vector_x, int vector_y ) :
CVorticonSpriteObject(p_map,x,y, OBJ_ICECANNON),
mTimer(0),
silent(false)
{
    this->vector_x = vector_x;
    this->vector_y = vector_y;

    inhibitfall = true;
    sprite = BLANKSPRITE;
    blockedd = true;
}

bool CIceCannon::isNearby(CVorticonSpriteObject &theObject)
{       
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
	int distx = player->getXPosition() - getXPosition();
	if(distx<0)
	    distx = -distx;
	
	int disty = player->getYPosition() - getYPosition();
	if(disty<0)
	    disty = -disty;
	
	if( disty < SILENT_DIST && distx < SILENT_DIST )
	{
	    silent = false;	
	}
    }

    return true;
}

// the ice cannon itself
void CIceCannon::process()
{
	// keep spawner object invisible and properly positioned
	if( mTimer >= GUNFIRE_TIMER_EP1 )
	{
		int newpos_x = getXPosition();
		int newpos_y = getYPosition();
		if(vector_x > 0) newpos_x += 512;
		CIceChunk *chunk = new CIceChunk(mp_Map, newpos_x, newpos_y,vector_x, vector_y);
		if(!silent)
		{
		    playSound(SOUND_CANNONFIRE);
		    silent = true;
		}

		spawnObj(chunk);
		mTimer = 0;
	}
	else
	{
	    mTimer++;
	}
}

CIceChunk::CIceChunk(CMap *p_map, Uint32 x, Uint32 y, Uint32 vx, Uint32 vy) :
CVorticonSpriteObject(p_map, x, y, OBJ_ICECHUNK),
vector_x(vx),
vector_y(vy),
silent(true)
{
	int speed;

	inhibitfall = true;

	if (vector_x && vector_y)
		speed = ICECHUNK_SPEED;
	else
		speed = ICECHUNK_STRAIGHT_SPEED;

	veloc_x = speed * vector_x;
	veloc_y = speed * vector_y;
}



void CIceChunk::getTouchedBy(CVorticonSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
        // freeze the player if it touches him
        CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();
        // make him start sliding in the direction of the impact

        const int iceInertia = Physics.player.max_x_speed;

        if (vector_x > 0)
        {
            player->pDir.x = player->pShowDir.x = RIGHT;
            player->xinertia = iceInertia;
            player->bump( RIGHT, iceInertia );
        }
        else if (vector_x < 0)
        {
            player->pDir.x = player->pShowDir.x = LEFT;
            player->xinertia = -iceInertia;
            player->bump( LEFT, iceInertia );
        }
        else	// perfectly vertical ice cannons
        {
            const int UPDNCANNON_PUSHAMT = 16;
            if (player->xinertia < UPDNCANNON_PUSHAMT)
            {
                if (rnd()&1)
                    player->xinertia = UPDNCANNON_PUSHAMT;
                else
                    player->xinertia = -UPDNCANNON_PUSHAMT;
            }
        }

        player->freeze();
        smash();
    }
}



void CIceChunk::process()
{
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

bool CIceChunk::isNearby(CVorticonSpriteObject &theObject)
{       
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
	int distx = player->getXPosition() - getXPosition();
	if(distx<0)
	    distx = -distx;
	
	int disty = player->getYPosition() - getYPosition();
	if(disty<0)
	    disty = -disty;
	
	if( disty < SILENT_DIST && distx < SILENT_DIST )
	{
	    silent = false;	
	}
    }

    return true;
}

void CIceChunk::smash()
{
	if (onscreen)
	{
	    if(!silent)
		playSound(SOUND_CHUNKSMASH);
	    
	    silent = true;
	    CIceBit *chunk;

		// upleft
		chunk = new CIceBit(mp_Map, getXPosition(), getYPosition(), -1, -1);
		spawnObj(chunk);

		// upright
		chunk = new CIceBit(mp_Map, getXPosition(), getYPosition(), 1, -1);
		spawnObj(chunk);

		// downleft
		chunk = new CIceBit(mp_Map, getXPosition(), getYPosition(), -1, 1);
		spawnObj(chunk);

		// downright
		chunk = new CIceBit(mp_Map, getXPosition(), getYPosition(), 1, 1);
		spawnObj(chunk);
	}
	exists = false;
}


// the little pieces that break off of an OBJ_ICECHUNK when it hits
// a wall or a player. (Ep1)
const int ICEBIT_SPEED = 80;
const int ICESHARD_TIME = 25;

CIceBit::CIceBit(CMap *p_map, Uint32 x, Uint32 y, Uint32 vec_x, Uint32 vec_y ) :
CVorticonSpriteObject(p_map, x, y, OBJ_ICEBIT),
vector_x(vec_x),
vector_y(vec_y)
{
	veloc_x = ICEBIT_SPEED * vector_x;
	veloc_y = ICEBIT_SPEED * vector_y;
	timer = ICESHARD_TIME;
	inhibitfall = true;
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


