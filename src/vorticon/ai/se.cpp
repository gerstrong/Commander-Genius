
#include "CObjectAI.h"
#include "ray.h"
#include "se.h"

#include "../spritedefines.h"
#include "../../sdl/sound/CSound.h"
#include "../../CLogFile.h"
#include "../../graphics/effects/CVibrate.h"

#define ANKH_SHIELD_FRAME       61

// "Sector Effector" object (The name comes from D3D)...it's basically
// an object which can do a number of different things depending on it's
// .ai.se.type attribute, usually it affects the map or the enviorment
// around it. Used where it wasn't worth it to create a whole new object
// (or where I was too lazy to do it).

// this also contains the AI for the Spark object

int mortimer_surprisedcount = 0;

void CObjectAI::se_ai(CObject &object)
{
	switch(object.ai.se.type)
	{
	case SE_EXTEND_PLATFORM: se_extend_plat(object, PlatExtending ); break;
	case SE_RETRACT_PLATFORM: se_retract_plat(object, PlatExtending ); break;
	case SE_ANKHSHIELD: se_ankhshield(object, m_Episode); break;
	case SE_MORTIMER_ARM: se_mortimer_arm(object); break;
	case SE_MORTIMER_LEG_LEFT: se_mortimer_leg_left(object); break;
	case SE_MORTIMER_LEG_RIGHT: se_mortimer_leg_right(object); break;
	case SE_MORTIMER_SPARK: se_mortimer_spark(object); break;
	case SE_MORTIMER_HEART: se_mortimer_heart(object); break;
	case SE_MORTIMER_ZAPSUP: se_mortimer_zapsup(object); break;
	case SE_MORTIMER_RANDOMZAPS: se_mortimer_randomzaps(object); break;

	default:
		g_pLogFile->ftextOut("Invalid sector effector type %d", object.ai.se.type);
		break;
	}

}

void CObjectAI::se_extend_plat(CObject &object, bool &PlatExtending)
{
#define PLAT_EXTEND_RATE        3

	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	if (object.needinit)
	{
		object.ai.se.timer = 0;
		object.inhibitfall = true;
		object.canbezapped = false;
		object.sprite = BLANKSPRITE;

		// if the platform is already extended, turn ourselves
		// into an se_retract_plat()
		if ( mp_Map->at(object.ai.se.platx, object.ai.se.platy) == TILE_EXTENDING_PLATFORM )
		{
			object.ai.se.type = SE_RETRACT_PLATFORM;
			se_retract_plat(object, PlatExtending);
			return;
		}

		// figure out which direction the bridge is supposed to go
		if (!TileProperty[mp_Map->at(object.ai.se.platx+1, object.ai.se.platy)].bleft)
			object.ai.se.dir = RIGHT;
		else
			object.ai.se.dir = LEFT;

		// get the background tile from the tile above the starting point
		if(object.ai.se.dir == RIGHT)
			m_bgtile = mp_Map->at(object.ai.se.platx+1, object.ai.se.platy);
		else
			m_bgtile = mp_Map->at(object.ai.se.platx-1, object.ai.se.platy);

		object.needinit = false;
	}

	if (!object.ai.se.timer)
	{
		if (object.ai.se.dir==RIGHT &&
				!TileProperty[mp_Map->at(object.ai.se.platx, object.ai.se.platy)].bleft)
		{
			mp_Map->changeTile(object.ai.se.platx, object.ai.se.platy, TILE_EXTENDING_PLATFORM);
			object.ai.se.platx++;
			kill_all_intersecting_tile(object.ai.se.platx, object.ai.se.platy);
			object.ai.se.timer = PLAT_EXTEND_RATE;
		}
		else if(object.ai.se.dir==LEFT &&
				!TileProperty[mp_Map->at(object.ai.se.platx, object.ai.se.platy)].bright)
		{
			mp_Map->changeTile(object.ai.se.platx, object.ai.se.platy, TILE_EXTENDING_PLATFORM);
			object.ai.se.platx--;
			kill_all_intersecting_tile(object.ai.se.platx, object.ai.se.platy);
			object.ai.se.timer = PLAT_EXTEND_RATE;
		}
		else
		{
			deleteObj(object);
			PlatExtending = false;
			return;
		}
	}
	else object.ai.se.timer--;
}

void CObjectAI::se_retract_plat(CObject &object, bool &PlatExtending)
{
	if (object.needinit)
	{
		// figure out which direction the bridge is supposed to go
		if(object.ai.se.platx-1 > MAX_OBJECTS)
			return;

		if (mp_Map->at(object.ai.se.platx-1, object.ai.se.platy) != TILE_EXTENDING_PLATFORM)
			object.ai.se.dir = LEFT;
		else if(mp_Map->at(object.ai.se.platx+1, object.ai.se.platy) != TILE_EXTENDING_PLATFORM)
			object.ai.se.dir = RIGHT;
		else
			object.ai.se.dir = LEFT;

		// scan across until we find the end of the platform--that will
		// be where we will start (remove platform in oppisote direction
		// it was extended)
		do
		{
			if (mp_Map->at(object.ai.se.platx, object.ai.se.platy) != TILE_EXTENDING_PLATFORM)
			{ // we've found the end of the platform
				break;
			}
			if (object.ai.se.dir==LEFT)
			{
				if (object.ai.se.platx==mp_Map->m_width)
				{
					g_pLogFile->ftextOut("SE_RETRACT_PLATFORM: Failed to find end of platform when scanning right.");
					return;
				}
				object.ai.se.platx++;
			}
			else
			{ // platform will be removed in a right-going direction
				if (object.ai.se.platx==0)
				{
					g_pLogFile->ftextOut("SE_RETRACT_PLATFORM: Failed to find end of platform when scanning left.");
					return;
				}
				object.ai.se.platx--;
			}
		} while(1);

		// when we were scanning we went one tile too far, go back one
		if (object.ai.se.dir==LEFT) object.ai.se.platx--;
		else object.ai.se.platx++;

		object.needinit = false;
	}

	if (!object.ai.se.timer)
	{
		if (mp_Map->at(object.ai.se.platx, object.ai.se.platy) == TILE_EXTENDING_PLATFORM)
		{
			mp_Map->setTile(object.ai.se.platx, object.ai.se.platy, m_bgtile, true);

			if (object.ai.se.dir==RIGHT)
				object.ai.se.platx++;
			else
				object.ai.se.platx--;

			object.ai.se.timer = PLAT_EXTEND_RATE;
		}
		else
		{
			deleteObj(object);
			PlatExtending = false;
		}
	}
	else object.ai.se.timer--;
}


// AI for the Spark object in the Tantalus Ray Machine's of ep2
void CObjectAI::spark_ai(CObject &object, int &sparks_left)
{
	int mx,my,x,y;

#define SPARK_BASEFRAME         OBJ_SPARK_DEFSPRITE_EP2
#define SPARK_ANIMRATE          5

enum spark_actions{
SPARK_ANIMATE, SPARK_BLOWUP1, SPARK_BLOWUP2, SPARK_BLOWUP3
};

#define SPARK_BLOW_DELAY        25

#define BG_GREY					143

	if (object.needinit)
	{
		object.ai.se.state = SPARK_ANIMATE;
		object.ai.se.timer = 0;
		object.canbezapped = 1;
		object.inhibitfall = 1;
		object.needinit = 0;

		object.ai.se.mx = object.getXPosition() >> CSF;
		object.ai.se.my = object.getYPosition() >> CSF;
	}

	if (object.ai.se.state==SPARK_ANIMATE)
	{
		object.sprite = SPARK_BASEFRAME + object.ai.se.frame;
	}
	else
	{
		object.sprite = BLANKSPRITE;
	}

	if (object.touchPlayer)
	{
		killplayer(object.touchedBy);
	}

	switch(object.ai.se.state)
	{
	case SPARK_ANIMATE:
		if (object.ai.se.timer > SPARK_ANIMRATE)
		{
			object.ai.se.frame++;
			if (object.ai.se.frame > 3) object.ai.se.frame = 0;
			object.ai.se.timer = 0;
		} else object.ai.se.timer++;

		if (object.zapped)
		{
			g_pSound->playStereofromCoord(SOUND_SHOT_HIT,PLAY_NOW, object.getXPosition());

			// break the glass and blow out the electric arcs
			mp_Map->setTile(object.ai.se.mx - 2, object.ai.se.my, 492, true);
			mp_Map->setTile(object.ai.se.mx - 1, object.ai.se.my, 546, true);
			mp_Map->setTile(object.ai.se.mx, object.ai.se.my, 547, true);
			mp_Map->setTile(object.ai.se.mx + 1, object.ai.se.my, 548, true);
			mp_Map->setTile(object.ai.se.mx + 2, object.ai.se.my, 492, true);
			// remove the unneeded dome tiles
			mp_Map->setTile(object.ai.se.mx - 1, object.ai.se.my-1, BG_GREY, true);
			mp_Map->setTile(object.ai.se.mx, object.ai.se.my-1, BG_GREY, true);
			mp_Map->setTile(object.ai.se.mx + 1, object.ai.se.my-1, BG_GREY, true);
			// break the switch
			mp_Map->setTile(object.ai.se.mx - 3, object.ai.se.my + 4, 506, true);

			// meltdown!
			object.ai.se.state = SPARK_BLOWUP1;
			object.ai.se.timer = 0;
			object.ai.se.blowy = 0;
		}
		break;
	case SPARK_BLOWUP1:
		// one by one blow out the purple thingies below the device
		if (object.ai.se.timer > SPARK_BLOW_DELAY)
		{
			object.ai.se.timer = 0;
			mx = object.ai.se.mx;
			my = object.ai.se.my+3+object.ai.se.blowy;
			mp_Map->setTile(mx, my, 505, true);
			// spawn a ZAP! or a ZOT!
			CObject newobject(mp_Map, m_Objvect.size());
			newobject.spawn(mx<<CSF, my<<CSF, OBJ_RAY, m_Episode);
			newobject.ai.ray.state = RAY_STATE_SETZAPZOT;
			newobject.ai.ray.owner = object.m_index;
			newobject.inhibitfall = 1;
			newobject.needinit = 0;
			newobject.ai.ray.dontHitEnable = 0;
			m_Objvect.push_back(newobject);
			g_pSound->playStereofromCoord(SOUND_SHOT_HIT,PLAY_NOW, newobject.getXPosition());

			object.ai.se.blowy++;
			if (object.ai.se.blowy >= 3)
			{
				object.ai.se.state = SPARK_BLOWUP2;
				object.ai.se.blowx = 0;
			}
		}
		else object.ai.se.timer++;
		break;
	case SPARK_BLOWUP2:
		// blow out the glowing cells
		if (object.ai.se.timer > SPARK_BLOW_DELAY)
		{
			if (object.ai.se.blowx >= 4)
			{
				// done blowing up the glowcells
				// static the targeting display
				mx = object.ai.se.mx - 7;
				my = object.ai.se.my + 2;
				for(y=0;y<3;y++)
				{
					for(x=0;x<3;x++)
					{
						mp_Map->setTile(mx+x,my+y,533, true);
					}
				}
				deleteObj(object);
				sparks_left--;
				return;
			}

			object.ai.se.timer = 0;
			mx = object.ai.se.mx + object.ai.se.blowx + 3;
			for(y=0;y<3;y++)
			{
				my = object.ai.se.my+3+y;
				mp_Map->setTile(mx, my, 549, true);
				// spawn a ZAP! or a ZOT!
				CObject newobject(mp_Map, m_Objvect.size());
				newobject.spawn(mx<<CSF, my<<CSF, OBJ_RAY, m_Episode);
				newobject.ai.ray.owner = object.m_index;
				newobject.ai.ray.state = RAY_STATE_SETZAPZOT;
				newobject.inhibitfall = true;
				newobject.needinit = false;
				g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, newobject.getXPosition());
				m_Objvect.push_back(newobject);
				
			}


			object.ai.se.blowx++;
		}
		else object.ai.se.timer++;
		break;
	}  // end of state switch for SE_SPARK

}

void CObjectAI::se_ankhshield(CObject &object, int episode)
{
#define ANKH_FLICKER_DELAY       3

	if (object.needinit)
	{
		object.ai.se.frame = 0;
		object.ai.se.timer = 0;
		object.inhibitfall = 1;
		object.canbezapped = 0;
		object.needinit = 0;

		object.ai.se.state = ANKH_STATE_NOFLICKER;
	}

	switch(object.ai.se.state)
	{
	case ANKH_STATE_NOFLICKER:
		object.sprite = ANKH_SHIELD_FRAME + (object.ai.se.frame&1);
		break;
	case ANKH_STATE_FLICKERFAST:
		if (object.ai.se.frame&1)
			object.sprite = BLANKSPRITE;
		else
		{
			if (object.ai.se.frame&2)
				object.sprite = ANKH_SHIELD_FRAME+1;
			else
				object.sprite = ANKH_SHIELD_FRAME;
		}
		break;
	case ANKH_STATE_FLICKERSLOW:
		if (object.ai.se.frame>4)
			object.sprite = BLANKSPRITE;
		else
			object.sprite = (m_Episode==3) ? ANKH_SHIELD_FRAME : YORPSHIELD_SPRITE;
		break;
	}

	if (object.ai.se.timer > ANKH_FLICKER_DELAY)
	{
		object.ai.se.frame++;
		if (object.ai.se.frame>8) object.ai.se.frame = 0;
		object.ai.se.timer = 0;
	}
	else object.ai.se.timer++;
}

#define ARM_GO          0
#define ARM_WAIT        1

#define ARM_MOVE_SPEED   10
#define ARM_WAIT_TIME    8
void CObjectAI::se_mortimer_arm(CObject &object)
{
	int mx,my;
	if (object.needinit)
	{
		object.ai.se.dir = DOWN;
		object.ai.se.state = ARM_GO;
		object.ai.se.timer = 0;
		object.inhibitfall = 1;
		object.needinit = 0;
	}

	switch(object.ai.se.state)
	{
	case ARM_GO:
		// vertical arm 618 620 619
		// pole 597
		// polka dot background 169
		if (object.ai.se.timer > ARM_MOVE_SPEED)
		{
			mx = object.getXPosition() >> CSF;
			my = object.getYPosition() >> CSF;

			if (object.ai.se.dir==DOWN)
			{
				// reached bottom?
				if (mp_Map->at(mx, my+3) == 471)
				{
					object.ai.se.timer = 0;
					object.ai.se.state = ARM_WAIT;
				}
				else
				{
					// add to the pole
					mp_Map->setTile(mx, my+1, 597, true);
					// create left side of pincher
					mp_Map->setTile(mx-1, my+1, 618, true);
					mp_Map->setTile(mx-1, my+2, 620, true);
					mp_Map->setTile(mx-1, my+3, 619, true);
					// create right side of pincher
					mp_Map->setTile(mx+1, my+1, 618, true);
					mp_Map->setTile(mx+1, my+2, 620, true);
					mp_Map->setTile(mx+1, my+3, 619, true);
					// erase the top of the pincher we don't need anymore
					mp_Map->setTile(mx-1, my, 169, true);
					mp_Map->setTile(mx+1, my, 169, true);
					object.moveDown(1<<CSF);
				}
			}
			else
			{  // arm going up

				// reached top?
				if (mp_Map->at(mx, my+1)==619)
				{
					object.ai.se.timer = 0;
					object.ai.se.state = ARM_WAIT;
				}
				else
				{
					// create left side of pincher
					mp_Map->changeTile(mx-1, my+1, 618);
					mp_Map->changeTile(mx-1, my+2, 620);
					mp_Map->changeTile(mx-1, my+3, 619);
					// create right side of pincher
					mp_Map->changeTile(mx+1, my+1, 618);
					mp_Map->changeTile(mx+1, my+2, 620);
					mp_Map->changeTile(mx+1, my+3, 619);
					// erase the bottom of the pincher we don't need anymore
					mp_Map->changeTile(mx-1, my+4, 169);
					mp_Map->changeTile(mx+1, my+4, 169);
					// erase the pole
					mp_Map->changeTile(mx, my+2, 169);

					object.moveUp(1<<CSF);
				}
			}
			object.ai.se.timer = 0;
		}
		else object.ai.se.timer++;
		break;
	case ARM_WAIT:
		if (object.ai.se.timer > ARM_WAIT_TIME)
		{
			if (object.ai.se.dir==DOWN)
			{
				object.ai.se.dir = UP;
				object.moveUp(2<<CSF);
			}
			else
			{
				object.ai.se.dir = DOWN;
				object.moveDown(1<<CSF);
			}

			object.ai.se.state = ARM_GO;
			object.ai.se.timer = 0;
		}
		else object.ai.se.timer++;
		break;
	}
}

#define MORTIMER_SPARK_BASEFRAME        114

#define MORTIMER_LEFT_ARM_X             5
#define MORTIMER_RIGHT_ARM_X            17
#define MORTIMER_ARMS_YSTART            7
#define MORTIMER_ARMS_YEND              18

#define ARMS_DESTROY_RATE        3

#define MSPARK_IDLE              0
#define MSPARK_DESTROYARMS       1
void CObjectAI::se_mortimer_spark(CObject &object)
{
	int x,mx;
	if (object.needinit)
	{
		object.ai.se.state = MSPARK_IDLE;
		object.ai.se.timer = 0;
		object.ai.se.frame = 0;
		object.inhibitfall = 1;
		object.canbezapped = 1;
		object.needinit = 0;
	}

	switch(object.ai.se.state)
	{
	case MSPARK_IDLE:
		object.sprite = MORTIMER_SPARK_BASEFRAME + object.ai.se.frame;

		// deadly to the touch
		if (object.touchPlayer)
		{
			killplayer(object.touchedBy);
		}

		if (object.ai.se.timer > SPARK_ANIMRATE)
		{
			object.ai.se.frame++;
			if (object.ai.se.frame > 3) object.ai.se.frame = 0;
			object.ai.se.timer = 0;
		}
		else object.ai.se.timer++;

		if (object.zapped)
		{
			set_mortimer_surprised(true);
			g_pGfxEngine->pushEffectPtr(new CVibrate(200));

			// if there are any sparks left, destroy the spark,
			// else destroy mortimer's arms
			for(std::vector<CObject>::iterator obj = m_Objvect.begin()
					; obj != m_Objvect.end() ; obj++)
			{
				if (obj->m_type==OBJ_SECTOREFFECTOR &&\
						obj->ai.se.type==SE_MORTIMER_SPARK &&\
						obj->exists)
				{
					if (obj->m_index!=object.m_index)
					{	// other sparks still exist
						object.ai.se.type = SE_MORTIMER_RANDOMZAPS;
						object.needinit = true;
						return;
					}
				}
			}
			// keen just destroyed the last spark

			// destroy mortimer's arms
			object.sprite = BLANKSPRITE;

			// destroy the sector effectors controlling his arms
			for(std::vector<CObject>::iterator obj = m_Objvect.begin()
					; obj != m_Objvect.end() ; obj++)
			{
				if (obj->m_type==OBJ_SECTOREFFECTOR && \
						obj->ai.se.type==SE_MORTIMER_ARM)
				{
					deleteObj(*obj);
				}
			}
			// go into a state where we'll destroy mortimer's arms
			object.ai.se.state = MSPARK_DESTROYARMS;
			object.ai.se.my = MORTIMER_ARMS_YSTART;
			object.ai.se.timer = 0;
		}
		break;
	case MSPARK_DESTROYARMS:
		if (!object.ai.se.timer)
		{
			g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.getXPosition());
			for(x=0;x<3;x++)
			{
				mx = MORTIMER_LEFT_ARM_X+x;
				if (mp_Map->at(mx, object.ai.se.my) != 169)
				{
					mp_Map->setTile(mx, object.ai.se.my, 169, true);
					// spawn a ZAP! or a ZOT!
					CObject newobject(mp_Map, m_Objvect.size());
					newobject.spawn(((mx<<4)+4)<<STC, object.ai.se.my<<4<<STC, OBJ_RAY, m_Episode);
					newobject.ai.ray.state = RAY_STATE_SETZAPZOT;
					newobject.ai.ray.direction = DOWN;
					newobject.inhibitfall = true;
					newobject.needinit = false;
					m_Objvect.push_back(newobject);
				}

				mx = MORTIMER_RIGHT_ARM_X+x;
				if (mp_Map->at(mx, object.ai.se.my) != 169)
				{
					mp_Map->setTile(mx, object.ai.se.my, 169, true);
					// spawn a ZAP! or a ZOT!
					CObject newobject(mp_Map, m_Objvect.size());
					newobject.spawn(((mx<<4)+4)<<STC, object.ai.se.my<<4<<STC, OBJ_RAY, m_Episode);
					newobject.ai.ray.state = RAY_STATE_SETZAPZOT;
					newobject.ai.ray.direction = DOWN;
					newobject.inhibitfall = true;
					newobject.needinit = false;
					m_Objvect.push_back(newobject);
				}

			}
			object.ai.se.timer = ARMS_DESTROY_RATE;
			object.ai.se.my++;
			if (object.ai.se.my > MORTIMER_ARMS_YEND)
			{
				deleteObj(object);
				set_mortimer_surprised(false);
			}
		}
		else object.ai.se.timer--;
		break;
	}
}

#define MORTIMER_HEART_BASEFRAME        146
#define HEART_ANIMRATE                  4

#define HEART_IDLE              0
#define HEART_ZAPSRUNUP         1
#define HEART_ZAPSRUNDOWN       2

#define MORTIMER_MACHINE_YSTART         3
#define MORTIMER_MACHINE_YEND           18
#define MORTIMER_MACHINE_YENDNOLEGS     14

#define MORTIMER_MACHINE_XSTART         8
#define MORTIMER_MACHINE_XEND           17

#define MACHINE_DESTROY_RATE            3
#define MORTIMER_ZAPWAVESPACING        50
#define MORTIMER_NUMZAPWAVES             5

#define ZAPSUP_NORMAL           0
#define ZAPSUP_ABOUTTOFADEOUT   1
void CObjectAI::se_mortimer_heart(CObject &object)
{
	int x;

	if (object.needinit)
	{
		object.ai.se.timer = 0;
		object.ai.se.frame = 0;
		object.ai.se.state = HEART_IDLE;
		object.inhibitfall = 1;
		object.canbezapped = 1;
		object.needinit = 0;
		mortimer_surprisedcount = 0;
	}

	switch(object.ai.se.state)
	{
	case HEART_IDLE:
		object.sprite = MORTIMER_HEART_BASEFRAME + object.ai.se.frame;

		if (object.ai.se.timer > HEART_ANIMRATE)
		{
			object.ai.se.frame ^= 1;
			object.ai.se.timer = 0;
		}
		else object.ai.se.timer++;

		if (object.zapped)
		{
			object.sprite = BLANKSPRITE;
			set_mortimer_surprised(true);

			// destroy Mortimer's machine
			g_pGfxEngine->pushEffectPtr(new CVibrate(10000));

			// kill all enemies
			std::vector<CObject>::iterator it_obj = m_Objvect.begin();
			for( ; it_obj!=m_Objvect.end() ; it_obj++ )
			{
				if(it_obj->m_type == OBJ_SECTOREFFECTOR &&
						it_obj->ai.se.type == SE_MORTIMER_HEART ) continue;
				else
				{
					deleteObj(*it_obj);
				}
			}

			set_mortimer_surprised(true);
			// have waves of zaps run up mortimer's machine
			object.ai.se.timer = 0;
			object.ai.se.state = HEART_ZAPSRUNUP;
			object.ai.se.counter = 0;
		}
		break;

	case HEART_ZAPSRUNUP:
		if (!object.ai.se.timer)
		{	// spawn another wave of zaps
			int x = object.getXPosition();
			int y = object.getYPosition();

			CObject newobject(mp_Map);
			newobject.spawn( x, y, OBJ_SECTOREFFECTOR, m_Episode);
			newobject.ai.se.type = SE_MORTIMER_ZAPSUP;
			newobject.ai.se.my = MORTIMER_MACHINE_YEND;
			newobject.ai.se.timer = 0;
			newobject.ai.se.destroytiles = 0;
			newobject.ai.se.state = ZAPSUP_NORMAL;
			newobject.hasbeenonscreen = false;

			object.ai.se.timer = MORTIMER_ZAPWAVESPACING;
			if (object.ai.se.counter > MORTIMER_NUMZAPWAVES)
			{
				newobject.ai.se.destroytiles = true;
				deleteObj(object);
			}
			else object.ai.se.counter++;

			m_Objvect.push_back(newobject);
		}
		else object.ai.se.timer--;
		break;
	case HEART_ZAPSRUNDOWN:
		if (!object.ai.se.timer)
		{
			for(x=MORTIMER_MACHINE_XSTART;x<MORTIMER_MACHINE_XEND;x++)
			{
				// delete the tile
				mp_Map->setTile(x,object.ai.se.my,169);
				// spawn a ZAP! or a ZOT!
				CObject newobject(mp_Map);
				newobject.spawn(((x<<4)+4)<<STC, object.ai.se.my<<4<<STC, OBJ_RAY, m_Episode);
				newobject.ai.ray.state = RAY_STATE_SETZAPZOT;
				newobject.ai.ray.direction = DOWN;
				newobject.inhibitfall = true;
				newobject.needinit = false;
				m_Objvect.push_back(newobject);
			}

			object.ai.se.timer = MACHINE_DESTROY_RATE;
			if (object.ai.se.my > MORTIMER_MACHINE_YEND)
			{
				deleteObj(object);
			}
			else object.ai.se.my++;
		}
		else object.ai.se.timer--;
		break;
	}
}

#define TIME_AFTER_DESTROY_BEFORE_FADEOUT       500
void CObjectAI::se_mortimer_zapsup(CObject &object)
{
	int x;

	if (!object.ai.se.timer)
	{
		if (object.ai.se.state==ZAPSUP_ABOUTTOFADEOUT)
		{
			m_Player[0].level_done = LEVEL_DONE_FADEOUT;
			deleteObj(object);
			return;
		}

		g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.getXPosition());
		for(x=MORTIMER_MACHINE_XSTART;x<MORTIMER_MACHINE_XEND;x++)
		{
			// spawn a ZAP! or a ZOT!
			CObject newobject(mp_Map, m_Objvect.size());
			newobject.spawn(((x<<4)+4)<<STC, object.ai.se.my<<4<<STC, OBJ_RAY, m_Episode);
			newobject.ai.ray.state = RAY_STATE_SETZAPZOT;
			newobject.ai.ray.direction = DOWN;
			newobject.inhibitfall = true;
			newobject.needinit = false;
			m_Objvect.push_back(newobject);

			if (object.ai.se.destroytiles)
			{
				// delete the tile
				mp_Map->changeTile(x,object.ai.se.my,169);
			}
		}

		object.ai.se.timer = MACHINE_DESTROY_RATE;
		if (object.ai.se.my <= MORTIMER_MACHINE_YSTART)
		{
			if (object.ai.se.destroytiles)
			{
				// last wave, prepare to initiate level fadeout
				object.ai.se.timer = TIME_AFTER_DESTROY_BEFORE_FADEOUT;
				object.ai.se.state = ZAPSUP_ABOUTTOFADEOUT;

				// if a keen is standing on that machine, make him fall!
				std::vector<CPlayer>::iterator player = m_Player.begin();
				for(; player != m_Player.end() ; player++)
				{
					player->pfalling = true;
				}

				return;
			}
			else
			{
				deleteObj(object);
				object.ai.se.timer = 0;
			}
		}
		else object.ai.se.my--;
	}
	else object.ai.se.timer--;
}

#define LEG_GO          0
#define LEG_WAIT        1

#define LEFTLEG_MOVE_SPEED   15
#define LEFTLEG_WAIT_TIME    36

#define RIGHTLEG_MOVE_SPEED   20
#define RIGHTLEG_WAIT_TIME    40
void CObjectAI::se_mortimer_leg_left(CObject &object)
{
	int mx,my;
	if (object.needinit)
	{
		object.ai.se.dir = UP;
		object.ai.se.state = LEG_GO;
		object.ai.se.timer = 0;
		object.inhibitfall = true;
		object.needinit = false;
	}

	switch(object.ai.se.state)
	{
	case LEG_GO:
		// leg tiles 621 623 622
		// pole 597
		// polka dot background 169
		// bottom marker for leg 430
		// top marker for leg 420
		if (object.ai.se.timer > LEFTLEG_MOVE_SPEED)
		{
			mx = object.getXPosition() >> CSF;
			my = object.getYPosition() >> CSF;

			if (object.ai.se.dir==DOWN)
			{
				// reached bottom?
				if (mp_Map->at(mx, my+1) == 430)
				{
					object.ai.se.timer = 0;
					object.ai.se.state = LEG_WAIT;
					g_pSound->playStereofromCoord(SOUND_FOOTSLAM, PLAY_NOW, object.getXPosition());
				}
				else
				{
					// create the leg
					mp_Map->setTile(mx-3,my+1,621, true);
					mp_Map->setTile(mx-2,my+1,623, true);
					mp_Map->setTile(mx-1,my+1,623, true);
					mp_Map->setTile(mx-0,my+1,622, true);
					// erase the tiles above us that used to be the leg
					mp_Map->setTile(mx-3,my,169, true);
					mp_Map->setTile(mx-2,my,169, true);
					mp_Map->setTile(mx-1,my,169, true);
					mp_Map->setTile(mx-0,my,597, true);         // add to pole

					object.moveDown(1<<CSF);
				}
			}
			else
			{  // leg going up

				// reached top?
				if (mp_Map->at(mx, my-1) == 420)
				{
					object.ai.se.timer = 0;
					object.ai.se.state = LEG_WAIT;
				}
				else
				{
					// create the leg
					mp_Map->setTile(mx-3,my-1,621, true);
					mp_Map->setTile(mx-2,my-1,623, true);
					mp_Map->setTile(mx-1,my-1,623, true);
					mp_Map->setTile(mx-0,my-1,622, true);
					// erase the tiles beneath us that used to be the leg
					mp_Map->setTile(mx-3,my,169, true);
					mp_Map->setTile(mx-2,my,169, true);
					mp_Map->setTile(mx-1,my,169, true);
					mp_Map->setTile(mx-0,my,169, true);

					object.moveUp(1<<CSF);
				}
			}
			object.ai.se.timer = 0;
		}
		else object.ai.se.timer++;
		break;
	case LEG_WAIT:
		if (object.ai.se.timer > LEFTLEG_WAIT_TIME)
		{
			if (object.ai.se.dir==DOWN)
			{
				object.ai.se.dir = UP;
			}
			else
			{
				object.ai.se.dir = DOWN;
			}

			object.ai.se.state = LEG_GO;
			object.ai.se.timer = 0;
		}
		else object.ai.se.timer++;
		break;
	}
}

void CObjectAI::se_mortimer_leg_right(CObject &object)
{
	int mx,my;
	if (object.needinit)
	{
		object.ai.se.dir = UP;
		object.ai.se.state = LEG_GO;
		object.ai.se.timer = 0;
		object.inhibitfall = 1;
		object.needinit = false;
	}

	if (object.touchPlayer)
	{
		killplayer(object.touchedBy);
	}

	switch(object.ai.se.state)
	{
	case LEG_GO:
		// leg tiles 621 623 622
		// pole 597
		// polka dot background 169
		// bottom marker for leg 430
		// top marker for leg 420
		if (object.ai.se.timer > RIGHTLEG_MOVE_SPEED)
		{
			mx = object.getXPosition() >> CSF;
			my = object.getYPosition() >> CSF;

			if (object.ai.se.dir==DOWN)
			{
				// reached bottom?
				if (mp_Map->at(mx, my+1) == 430)
				{
					object.ai.se.timer = 0;
					object.ai.se.state = LEG_WAIT;
					g_pSound->playStereofromCoord(SOUND_FOOTSLAM, PLAY_NOW, object.getXPosition());
				}
				else
				{
					// create the leg
					mp_Map->setTile(mx+3,my+1,622, true);
					mp_Map->setTile(mx+2,my+1,623, true);
					mp_Map->setTile(mx+1,my+1,623, true);
					mp_Map->setTile(mx+0,my+1,621, true);
					// erase the tiles above us that used to be the leg
					mp_Map->setTile(mx+3,my,169, true);
					mp_Map->setTile(mx+2,my,169, true);
					mp_Map->setTile(mx+1,my,169, true);
					mp_Map->setTile(mx+0,my,597, true);         // add to pole

					object.moveDown(1<<CSF);
				}
			}
			else
			{  // leg going up

				// reached top?
				if (mp_Map->at(mx, my-1) == 420)
				{
					object.ai.se.timer = 0;
					object.ai.se.state = LEG_WAIT;
				}
				else
				{
					// create the leg
					mp_Map->setTile(mx+3,my-1,622, true);
					mp_Map->setTile(mx+2,my-1,623, true);
					mp_Map->setTile(mx+1,my-1,623, true);
					mp_Map->setTile(mx+0,my-1,621, true);
					// erase the tiles beneath us that used to be the leg
					mp_Map->setTile(mx+3,my,169, true);
					mp_Map->setTile(mx+2,my,169, true);
					mp_Map->setTile(mx+1,my,169, true);
					mp_Map->setTile(mx+0,my,169, true);

					object.moveUp(1<<CSF);
				}
			}
			object.ai.se.timer = 0;
		}
		else object.ai.se.timer++;
		break;
	case LEG_WAIT:
		if (object.ai.se.timer > RIGHTLEG_WAIT_TIME)
		{
			if (object.ai.se.dir==DOWN)
			{
				object.ai.se.dir = UP;
			}
			else
			{
				object.ai.se.dir = DOWN;
			}

			object.ai.se.state = LEG_GO;
			object.ai.se.timer = 0;
		}
		else object.ai.se.timer++;
		break;
	}
}

#define NUM_RANDOM_ZAPS         30
#define TIME_BETWEEN_ZAPS       2
void CObjectAI::se_mortimer_randomzaps(CObject &object)
{
	int x,y;
	if (object.needinit)
	{
		object.sprite = BLANKSPRITE;
		object.ai.se.counter = 0;
		object.ai.se.timer = 0;
		object.needinit = 0;
	}

	if (!object.ai.se.timer)
	{
		x = rand()%((MORTIMER_MACHINE_XEND*16)-(MORTIMER_MACHINE_XSTART*16))+(MORTIMER_MACHINE_XSTART*16);
		y = rand()%((MORTIMER_MACHINE_YENDNOLEGS*16)-(MORTIMER_MACHINE_YSTART*16))+(MORTIMER_MACHINE_YSTART*16);

		// spawn a ZAP! or a ZOT!
		CObject newobject(mp_Map, m_Objvect.size());
		newobject.spawn(x<<CSF, y<<CSF, OBJ_RAY, m_Episode);
		newobject.ai.ray.state = RAY_STATE_SETZAPZOT;
		newobject.inhibitfall = true;
		newobject.needinit = false;
		m_Objvect.push_back(newobject);

		object.ai.se.timer = TIME_BETWEEN_ZAPS;
		if (object.ai.se.counter > NUM_RANDOM_ZAPS)
		{
			set_mortimer_surprised(false);
			deleteObj(object);
		}
		else object.ai.se.counter++;
	}
	else object.ai.se.timer--;
}

void CObjectAI::set_mortimer_surprised(bool yes)
{
	if (yes)
	{
		mortimer_surprisedcount++;
	}
	else
	{
		if (mortimer_surprisedcount>0) mortimer_surprisedcount--;
	}

	if (mortimer_surprisedcount)
	{
		//12,6 -> 610 -- give mortimer his "surprised" face
		// deanimate mortimer's hands
		mp_Map->setTile(12,6,610, true);
		mp_Map->setTile(11,6,613, true);
		mp_Map->setTile(13,6,615, true);
	}
	else
	{
		// give mortimer his normal face again
		mp_Map->setTile(12,6,607, true);
		mp_Map->setTile(11,6,613, true);
		mp_Map->setTile(13,6,616, true);
	}
}
