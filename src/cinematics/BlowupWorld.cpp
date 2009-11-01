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
void SetVibrateTime(int vibetime, char pausegame, stLevelControl *levelcontrol)
{
	vibe_xamt = vibe_yamt = 0;
	if (levelcontrol->episode==3)
		vibe_strength = VIBRATE_MORTIMERSTRENGTH;
	else
		vibe_strength = VIBRATE_MINSTRENGTH;
	
	vibe_getworsetimer = 0;
	levelcontrol->vibratetime = vibetime;
	levelcontrol->vibratepause = pausegame;
}

/*void static vibescroller(void)
 {
 int i;
 // scroll the map to make it look like it's vibrating
 if (vibe_xdir)
 for(i=0;i<vibe_xamt;i++) map_scroll_right();
 else
 for(i=0;i<vibe_xamt;i++) map_scroll_left();
 
 if (vibe_ydir)
 for(i=0;i<vibe_yamt;i++) map_scroll_down();
 else
 for(i=0;i<vibe_yamt;i++) map_scroll_up();
 }
 
 // makes the map vibrate like an earthquake
 void do_vibration(void)
 {
 if (Menu_IsVisible()) return;
 
 // undo the scroll from last time
 vibe_xdir ^= 1; vibe_ydir ^= 1;
 vibescroller();
 
 levelcontrol.vibratetime--;
 
 if (levelcontrol.vibratetime)
 {
 // randomize a new amount to offset this frame
 vibe_xamt = (rnd() % vibe_strength);
 vibe_yamt = (rnd() % vibe_strength);
 vibe_xdir = rnd() & 1;
 vibe_ydir = rnd() & 1;
 
 vibescroller();
 
 if (++vibe_getworsetimer > VIBRATE_GETWORSETIME)
 {
 vibe_getworsetimer = 0;
 if (vibe_strength < VIBRATE_MAXSTRENGTH) vibe_strength++;
 }
 }
 else		// vibration is over, blow up the world
 {
 if (levelcontrol.episode==2)	// vibration also used in ep3 end level
 {
 dispmsgstring("EP2_UhOhString", 1);
 message_SetDismissalCallback(blow_up_world);
 }
 }
 }
 void blow_up_world(void) { endlevel(HIT_TANTALUS_SWITCH); }
 
 
 
 // start x,y map scroll position
 #define TANTALUS_X       0
 #define TANTALUS_Y       0
 #define TANTALUS_SPRITE  58
 
 #define SHOT_SPD_X   (21*1.5)
 #define SHOT_SPD_Y   (9*1.5)
 
 int shot_x, shot_y, shot_obj, shot_frame, shot_frametimer;
 int fire_timer;
 char tant_hitearth;
 void seq_tantalus_start(void)
 {
 int i;
 int x,y;
 lprintf(">seq_tantalus_start()\n");
 overlay.tantalus = 1;
 sound_stop_all();
 
 initgame();
 showmapatpos(81, TANTALUS_X, TANTALUS_Y);
 
 AllPlayersInvisible();
 
 // there are no objects in this scene
 for(i=0;i<MAX_OBJECTS;i++) delete_object(i);
 
 // place keen next to his ship
 numplayers = 1;
 objects[player[0].useObject].exists = 1;
 highest_objslot = player[0].useObject+1;
 
 // place the player (which is actually the tantalus ray) at the mouth
 // of the vorticon mothership
 if (map_findtile(593, &x, &y))
 { // found the tile
 shot_x = ((x<<4)-1)<<CSF;
 shot_y = ((y<<4)-0)<<CSF;
 shot_frame = 0;
 shot_frametimer = 0;
 }
 else
 {
 crash("seq_tantalus_start(): unable to find tile 593.");
 return;
 }
 
 player[0].x = shot_x;
 player[0].y = shot_y;
 fire_timer = 300;
 tant_hitearth = 0;
 
 levelcontrol.dontscroll = 1;
 fade(FADE_IN, FADE_NORM);
 }
 
 
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
 
 
 int timer, spawnedcount;
 void static tant_moveshot(void)
 {
 int tl;
 levelcontrol.dontscroll = 0;
 player[0].x = shot_x;
 player[0].y = shot_y;
 
 if (fire_timer > 0)
 {
 fire_timer--;
 if (!fire_timer)
 {
 shot_obj = spawn_object(shot_x, shot_y, OBJ_RAY);
 objects[shot_obj].onscreen = 1;
 sound_play(SOUND_KEEN_FIRE, PLAY_FORCE);
 }
 else return;
 }
 
 objects[shot_obj].sprite = TANTALUS_SPRITE + shot_frame;
 if (++shot_frametimer > 45)
 {
 shot_frametimer = 0;
 shot_frame ^= 1;
 }
 
 objects[shot_obj].x = shot_x;
 objects[shot_obj].y = shot_y;
 
 tl = getmaptileat((shot_x>>CSF)+(sprites[TANTALUS_SPRITE].xsize/2), \
 (shot_y>>CSF)+(sprites[TANTALUS_SPRITE].ysize/2));
 if (tl==586)
 {	  // it hit center of earth
 tant_hitearth = 1;
 delete_object(shot_obj);
 srnd(300);
 spawn_object(shot_x, shot_y, OBJ_EXPLOSION);
 sound_play(SOUND_EARTHPOW, PLAY_NOW);
 timer = spawnedcount = 0;
 }
 else
 {
 shot_x += SHOT_SPD_X;
 shot_y += SHOT_SPD_Y;
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
