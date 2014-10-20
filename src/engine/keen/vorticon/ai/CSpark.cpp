/*
 * CSpark.cpp
 *
 *  Created on: 09.07.2010
 *      Author: gerstrong
 */

#include "CSpark.h"
#include "CRay.h"

#include "sdl/audio/Audio.h"

#define SPARK_BASEFRAME         OBJ_SPARK_DEFSPRITE_EP2
#define SPARK_ANIMRATE          5

CSpark::CSpark(CMap *pmap, Uint32 x, Uint32 y) :
CVorticonSpriteObject(pmap, x, y, OBJ_SPARK, 0),
timer(0), frame(0),
blowx(0), blowy(0)
{
	state = SPARK_ANIMATE;
	canbezapped = 1;
	inhibitfall = 1;

}

#define SPARK_BLOW_DELAY        25

#define BG_GREY					143

// AI for the Spark object in the Tantalus Ray Machine's of ep2
void CSpark::process()
{
	int mx,my,x,y;

	mx = getXPosition() >> CSF;
	my = getYPosition() >> CSF;

	if (state==SPARK_ANIMATE)
	{
		sprite = SPARK_BASEFRAME + frame;
	}
	else
	{
		sprite = BLANKSPRITE;
	}

	switch(state)
	{
	case SPARK_ANIMATE:
		if (timer > SPARK_ANIMRATE)
		{
			frame++;
			if (frame > 3) frame = 0;
			timer = 0;
		} else timer++;

		if ( mHealthPoints <= 0 && state == SPARK_ANIMATE )
		{
			playSound(SOUND_SHOT_HIT);

			// break the glass and blow out the electric arcs
			mp_Map->setTile(mx - 2, my, 492, true);
			mp_Map->setTile(mx - 1, my, 546, true);
			mp_Map->setTile(mx, my, 547, true);
			mp_Map->setTile(mx + 1, my, 548, true);
			mp_Map->setTile(mx + 2, my, 492, true);
			// remove the unneeded dome tiles
			mp_Map->setTile(mx - 1, my-1, BG_GREY, true);
			mp_Map->setTile(mx, my-1, BG_GREY, true);
			mp_Map->setTile(mx + 1, my-1, BG_GREY, true);
			// break the switch
			mp_Map->setTile(mx - 3, my + 4, 506, true);

			// meltdown!
			state = SPARK_BLOWUP1;
			timer = 0;
			blowy = 0;
		}
		break;
	case SPARK_BLOWUP1:
		// one by one blow out the purple thingies below the device
		if (timer > SPARK_BLOW_DELAY)
		{
			timer = 0;
			my = my+3+blowy;
			mp_Map->setTile(mx, my, 505, true);
			// spawn a ZAP! or a ZOT!
            CRay *newobject = new CRay(mp_Map, mx<<CSF, my<<CSF, CENTER, DOWN, getSpriteVariantId());
			newobject->state = CRay::RAY_STATE_SETZAPZOT;
			newobject->setOwner(m_type, m_index);
			gEventManager.add( new EventSpawnObject(newobject) );
            gEventManager.add( new AddPointsToAllPlayers(10000) );
			playSound(SOUND_SHOT_HIT);

			blowy++;
			if (blowy >= 3)
			{
				state = SPARK_BLOWUP2;
				blowx = 0;
			}
		}
		else timer++;
		break;
	case SPARK_BLOWUP2:
		// blow out the glowing cells
		if (timer > SPARK_BLOW_DELAY)
		{
			if (blowx >= 4)
			{
				// done blowing up the glowcells
				// static the targeting display
				mx = mx - 7;
				my = my + 2;
				for(y=0;y<3;y++)
				{
					for(x=0;x<3;x++)
					{
						mp_Map->setTile(mx+x,my+y,533, true);
					}
				}
				exists = false;
				return;
			}

			timer = 0;
			mx = mx + blowx + 3;
			for(y=3;y<6;y++)
			{
				//my = my+3+y;
				//my = my+y;
				mp_Map->setTile(mx, my+y, 549, true);
				// spawn a ZAP! or a ZOT!
                CRay *newobject = new CRay(mp_Map, mx<<CSF, (my+y)<<CSF, CENTER, DOWN, getSpriteVariantId());
				newobject->setOwner(m_type ,m_index);
				newobject->state = CRay::RAY_STATE_SETZAPZOT;
				playSound(SOUND_SHOT_HIT);
				gEventManager.add(new EventSpawnObject(newobject));
			}

			blowx++;
		}
		else timer++;
		break;
	default: break;
	}  // end of state switch for SE_SPARK
}
