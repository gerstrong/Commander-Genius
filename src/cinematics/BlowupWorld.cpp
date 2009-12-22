/*
 * blowupworld.cpp
 *
 *  Created on: 06.08.2009
 *      Author: Caitlin Shaw
 *      Ported by: Gerstrong
 */

// cinematic of what happens when you hit the on switch on a tantalus ray
// (episode 2)

#include "../keen.h"

#define VIBRATE_MINSTRENGTH      1
#define VIBRATE_MORTIMERSTRENGTH 4
#define VIBRATE_MAXSTRENGTH      16
#define VIBRATE_GETWORSETIME	 150

int vibe_xamt, vibe_yamt, vibe_xdir, vibe_ydir;
int vibe_strength, vibe_getworsetimer;

/*
// start x,y map scroll position
#define TANTALUS_X       0
#define TANTALUS_Y       0
#define TANTALUS_SPRITE  58

#define SHOT_SPD_X   (21*1.5)
#define SHOT_SPD_Y   (9*1.5)

int shot_x, shot_y, shot_obj, shot_frame, shot_frametimer;
int fire_timer;
char tant_hitearth;

void seq_tantalus_run(void)
{
	player[0].hideplayer = 1;

	if (!levelcontrol.gameover)
	{
		if (!tant_hitearth)
		{
			tant_moveshot();
		}
		else
		{
			tant_earthexplode();
		}
	}
}



#define EARTHCHUNK_BIG_UP       64
#define EARTHCHUNK_BIG_DN       66
#define EARTHCHUNK_SMALL_UP     68
#define EARTHCHUNK_SMALL_DN     70
void tant_earthexplode(void)
{
	int i,o;
	gamedo_enemyai();

	if (!timer)
	{
		if (spawnedcount<16)
			o = spawn_object(shot_x+((rnd()%32)<<CSF), shot_y+((rnd()%32)<<CSF)-(8<<CSF), OBJ_EXPLOSION);
		switch(spawnedcount)
		{
		case 0: o = spawn_object(shot_x-(8<<CSF), shot_y-(8<<CSF), OBJ_EXPLOSION); break;
		case 1: o = spawn_object(shot_x+(24<<CSF), shot_y+(4<<CSF), OBJ_EXPLOSION); break;
		case 2: o = spawn_object(shot_x+(16<<CSF), shot_y-(8<<CSF), OBJ_EXPLOSION); break;
		case 3: o = spawn_object(shot_x+(24<<CSF), shot_y+(16<<CSF), OBJ_EXPLOSION); break;
		case 4: o = spawn_object(shot_x-(8<<CSF), shot_y+(4<<CSF), OBJ_EXPLOSION); break;
		case 5:
			o = spawn_object(shot_x-(8<<CSF), shot_y+(16<<CSF), OBJ_EXPLOSION);
			// spawn a bunch of small fragments of the earth to go flying off

			// spawn small earth chunks in all possible directions
			// (upleft/upright/dnleft/dnright)
			// up/left/down/right
			for(i=0;i<=9;i++)
			{
				o = spawn_object(shot_x+(14<<CSF), shot_y, OBJ_EARTHCHUNK);
				objects[o].ai.ray.direction = i;
				if (i > 4)
				{
					objects[o].sprite = EARTHCHUNK_SMALL_DN;
				}
				else
				{
					objects[o].sprite = EARTHCHUNK_SMALL_UP;
				}
			}

			break;
		case 6:
			o = spawn_object(shot_x+(16<<CSF), shot_y+(16<<CSF), OBJ_EXPLOSION);
			break;
		case 7: o = spawn_object(shot_x+(24<<CSF), shot_y-(8<<CSF), OBJ_EXPLOSION); break;
		case 8: o = spawn_object(shot_x+(16<<CSF), shot_y+(4<<CSF), OBJ_EXPLOSION); break;
		case 10:
			// spawn four big fragments of the earth to go flying off
			o = spawn_object(shot_x+(8<<CSF), shot_y, OBJ_EARTHCHUNK);
			objects[o].sprite = EARTHCHUNK_BIG_UP;
			objects[o].ai.ray.direction = EC_UPLEFT;
			o = spawn_object(shot_x+(8<<CSF), shot_y, OBJ_EARTHCHUNK);
			objects[o].sprite = EARTHCHUNK_BIG_UP;
			objects[o].ai.ray.direction = EC_UPRIGHT;
			o = spawn_object(shot_x+(8<<CSF), shot_y, OBJ_EARTHCHUNK);
			objects[o].sprite = EARTHCHUNK_BIG_DN;
			objects[o].ai.ray.direction = EC_DOWNRIGHT;
			o = spawn_object(shot_x+(8<<CSF), shot_y, OBJ_EARTHCHUNK);
			objects[o].sprite = EARTHCHUNK_BIG_DN;
			objects[o].ai.ray.direction = EC_DOWNLEFT;
			break;
		case 32:
			sound_play(SOUND_GAME_OVER, PLAY_NOW);
			SetGameOver();
			break;
		}
		spawnedcount++;
		timer = 60;
	}
	else timer--;
}*/
