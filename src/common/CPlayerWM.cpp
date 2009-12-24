/*
 * CPlayerWM.cpp
 *
 *  Created on: 07.10.2009
 *      Author: gerstrong
 */

#include "CPlayer.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CInput.h"
#include "../keen.h"

///
// Process Part
///

////
// Process the stuff of the player when playing on the world map
void CPlayer::processWorldMap()
{
    inhibitwalking = false;
    inhibitfall = false;
	
    StatusBox();
	
   	if(!hideplayer) ProcessInput();
   	setWorldMapdir();
	
   	setWMblockedlrud();
   	AllowEnterLevelonWM();
	
    if(!hideplayer && !beingteleported)	Walking();
   	WalkingAnimation();
	
   	InertiaAndFriction_X();
   	InertiaAndFriction_Y();
	
   	selectFrameOnWorldMap();
}

// select proper player direction
void CPlayer::setWorldMapdir()
{
	bool exception=false;
	dpadcount = 0;
	if (playcontrol[PA_X] < 0) { pdir = LEFT; dpadcount++; }
	if (playcontrol[PA_X] > 0) { pdir = RIGHT; dpadcount++; }
	if (playcontrol[PA_Y] < 0) { pdir = UP; dpadcount++; }
	if (playcontrol[PA_Y] > 0) { pdir = DOWN; dpadcount++; }
	
	if ( pshowdir==UP && !playcontrol[PA_Y] < 0) exception = true;
	if ( pshowdir==DOWN && !playcontrol[PA_Y] > 0) exception = true;
	if ( pshowdir==LEFT && !playcontrol[PA_X] < 0) exception = true;
	if ( pshowdir==RIGHT && !playcontrol[PA_X] > 0) exception = true;
	if ((playcontrol[PA_Y] < 0) && (playcontrol[PA_Y] > 0)) pshowdir = DOWN;
	if ((playcontrol[PA_X] < 0) && (playcontrol[PA_X] > 0)) pshowdir = RIGHT;
	
	if (dpadcount==1 || dpadlastcount==0 || exception)
		pshowdir = pdir;

	dpadlastcount = dpadcount;
}

// set blockedl and blockedr...is Keen up against a solid object?
void CPlayer::setWMblockedlrud()
{
	blockedl = blockedr = false;
	blockedu = blockedd = false;
	
	// cheat: holding down TAB will turn off clipping. or if you are in godmode
	if ((m_cheats_enabled && g_pInput->getHoldedKey(KTAB)) || godmode) return;
	
	// R
	if (isWMSolid((goto_x>>5)+8, (goto_y>>5)+1))
	{ blockedr = 1; }
	else if (isWMSolid((goto_x>>5)+8, (goto_y>>5)+8))
	{ blockedr = 1; }
	else if (isWMSolid((goto_x>>5)+8, (goto_y>>5)+13))
	{ blockedr = 1; }
	
	// L
	if (isWMSolid((goto_x>>5)+0, (goto_y>>5)+1))
	{ blockedl = 1; }
	else if (isWMSolid((goto_x>>5)+0, (goto_y>>5)+8))
	{ blockedl = 1; }
	else if (isWMSolid((goto_x>>5)+0, (goto_y>>5)+13))
	{ blockedl = 1; }
	
	// U
	if (isWMSolid((goto_x>>5)+1, (goto_y>>5)-1))
	{ blockedu = 1; }
	else if (isWMSolid((goto_x>>5)+4, (goto_y>>5)-1))
	{ blockedu = 1; }
	else if (isWMSolid((goto_x>>5)+7, (goto_y>>5)-1))
	{ blockedu = 1; }
	
	// D
	if (isWMSolid((goto_x>>5)+1, (goto_y>>5)+14))
	{ blockedd = 1; }
	else if (isWMSolid((goto_x>>5)+4, (goto_y>>5)+14))
	{ blockedd = 1; }
	else if (isWMSolid((goto_x>>5)+7, (goto_y>>5)+14))
	{ blockedd = 1; }
}

// tell me, if the player tries to use an object on the map like entering the level
int CPlayer::getNewObject()
{
	int xb, yb;
	int lvl;
	
    if ( !object_chosen && (playcontrol[PA_JUMP] || playcontrol[PA_POGO]) )
    {   // trying to enter a level (or use a teleporter, etc)
    	object_chosen = true;
		
        // get level/object marker beneath player
        xb = ((x+w/2)>>STC);
        yb = ((y+h/2)>>STC);
        lvl = mp_map->getObjectat(xb>>4, yb>>4);
        if (!lvl)
        {
			yb = (y>>CSF)+8;
			lvl = mp_map->getObjectat(xb>>4, yb>>4);
        }
        return lvl;
    }

    if (!playcontrol[PA_JUMP] && !playcontrol[PA_POGO])
    	object_chosen = false;

    return 0;
}

void CPlayer::AllowEnterLevelonWM()
{
	/*int lvl;
	 int x,y,i,o;
	 int destx, desty;
	 int telfrom, teldest, telsnap;
	 
	 stLevelControl *p_levelcontrol;
	 
	 // 3859 <-> 3889
	 // 3911 -> 3955
	 // 3877 <-> 3922
	 
	 // 4032 -> 3842
	 // 3842 -> 3877
	 // 4063 -> 4094
	 // 4094 -> 4072
	 // 4072 -> 3980
	 // 3980 -> 4032 nosnap
	 
	 // table of teleporters (mostly for ep3). player will teleport from
	 // a teleporter in a teleport_from entry, to the teleporter
	 // in the matching teleport_dest entry. Thus 3859 leads to 3889, etc.
	 // if the snap entry is 1 the screen will snap to center on the new
	 // position, otherwise it will smoothly scroll over to it
	 #define NUM_TPORTS    20
	 int teleport_from[NUM_TPORTS+1] = {4014,4032,3842,4063,4094,4072,3980,3859,3889,3911,3877,3922,3947,4025,3988,3955,41,38,0};
	 int teleport_dest[NUM_TPORTS+1] = {4072,3842,3877,4094,4072,3980,4032,3889,3859,3955,3922,3877,4025,3988,3911,3889,38,41,0};
	 int teleport_snap[NUM_TPORTS+1] = {1   ,1   ,1   ,1   ,1   ,1   ,0   ,0   ,0   ,1   ,1   ,1   ,1   ,0   ,1   ,1   ,0 ,0 ,0};
	 
	 p_levelcontrol = &(pCKP->Control.levelcontrol);
	 
	 if (player[cp].hideplayer) return;  // don't execute function while teleporting
	 
	 if ((player[cp].playcontrol[PA_JUMP] || player[cp].playcontrol[PA_POGO]) && !player[cp].wm_lastenterstate)
	 {   // trying to enter a level (or use a teleporter, etc)
	 
	 // get level/object marker beneath player
	 x = (player[cp].x>>CSF)+4;
	 y = (player[cp].y>>CSF)+4;
	 lvl = map.objectlayer[x>>4][y>>4];
	 if (!lvl)
	 {
	 y = (player[cp].y>>CSF)+8;
	 lvl = map.objectlayer[x>>4][y>>4];
	 }
	 
	 x = x >> 4 << 4;
	 y = y >> 4 << 4;
	 
	 if (lvl)
	 {
	 // handle special map objects, and standard levels too
	 
	 // look through the teleporter tables and see if this is
	 // a teleporter
	 telfrom = 0;
	 for(i=0;i<NUM_TPORTS;i++)
	 {
	 if (teleport_from[i]==lvl)
	 { // we found it! it IS a teleporter!
	 telfrom = teleport_from[i];
	 teldest = teleport_dest[i];
	 telsnap = teleport_snap[i];
	 break;
	 }
	 else if (teleport_from[i]==0)
	 { // nope. it's not a teleporter.
	 break;
	 }
	 }
	 
	 // need to teleport somewhere?
	 if (telfrom)
	 {
	 if (map_findobject(teldest, &destx, &desty))
	 {
	 o = spawn_object(x<<CSF,y<<levels_completedCSF,OBJ_TELEPORTER);
	 objects[o].ai.teleport.direction = TELEPORTING_OUT;
	 objects[o].ai.teleport.destx = destx << 4 << CSF;
	 objects[o].ai.teleport.desty = desty << 4 << CSF;
	 objects[o].ai.teleport.whichplayer = cp;
	 objects[o].ai.teleport.NoExitingTeleporter = 0;
	 objects[o].ai.teleport.snap = telsnap;
	 if (m_episode==3)
	 {
	 objects[o].ai.teleport.baseframe = TELEPORT_BASEFRAME_EP3;
	 objects[o].ai.teleport.idleframe = TELEPORT_IDLEFRAME_EP3;
	 }
	 else
	 { // the teleporter in ep1
	 objects[o].ai.teleport.baseframe = TELEPORT_GRAY_BASEFRAME_EP1;
	 objects[o].ai.teleport.idleframe = TELEPORT_GRAY_IDLEFRAME_EP1;
	 }
	 g_pSound->playStereofromCoord(SOUND_TELEPORT, PLAY_NOW, objects[player[cp].useObject].scrx);
	 
	 player[cp].hideplayer = 1;
	 }
	 }
	 else
	 {
	 switch(lvl)
	 {
	 case NESSIE_PATH: break;
	 case NESSIE_PAUSE: break;
	 case NESSIE_MOUNTPOINT: break;
	 
	 // ep1 bonus teleporter
	 case LVLS_TELEPORTER_BONUS:
	 o = spawn_object(x<<CSF,y<<CSF,OBJ_TELEPORTER);
	 objects[o].ai.teleport.direction = TELEPORTING_OUT;
	 objects[o].ai.teleport.destx = TELEPORT_BONUS_DESTX;
	 objects[o].ai.teleport.desty = TELEPORT_BONUS_DESTY;
	 objects[o].ai.teleport.whichplayer = cp;
	 objects[o].ai.teleport.baseframe = TELEPORT_RED_BASEFRAME_EP1;
	 objects[o].ai.teleport.idleframe = TELEPORT_RED_IDLEFRAME_EP1;
	 objects[o].ai.teleport.NoExitingTeleporter = 1;
	 objects[o].ai.teleport.snap = 1;
	 player[cp].hideplayer = 1;
	 break;
	 
	 case LVLS_SHIP:
	 if (m_episode==1)
	 {
	 YourShipNeedsTheseParts(pCKP);
	 }
	 else
	 {
	 ShipEp3(pCKP);
	 }
	 break;
	 
	 default:      // a regular level
	 
	 p_levelcontrol->command = LVLC_CHANGE_LEVEL;
	 p_levelcontrol->chglevelto = (lvl & 0x7fff);
	 endlevel(1, &(pCKP->Control.levelcontrol));
	 g_pMusicPlayer->stop();
	 g_pSound->playStereofromCoord(SOUND_ENTER_LEVEL, PLAY_NOW, objects[player[cp].useObject].scrx);
	 
	 break;
	 
	 } // end switch(level)
	 } // end isn't a teleporter
	 } // end if(lvl)
	 } // end trying to enter a level
	 
	 player[cp].wm_lastenterstate = (player[cp].playcontrol[PA_JUMP] || player[cp].playcontrol[PA_POGO]);*/
}

bool CPlayer::isWMSolid(int xb, int yb)
{
	int level_coordinates;
	
	// Now check if the levels must block the player
	level_coordinates = mp_map->getObjectat(xb>>4, yb>>4);
	
	if (level_coordinates & 0x8000)
	{
		if( mp_levels_completed[level_coordinates & 0x7fff] ) // check if level is done, but can be replayed
		{	  return false;	}
		
		if( g_pInput->getHoldedKey(KTAB) && g_pInput->getHoldedKey(KSHIFT) )
		{	  return false;	}
		else
		{	  return true;	}
	}
	return false;
}

void CPlayer::selectFrameOnWorldMap()
{
    // select base frame for current direction
    if (pshowdir==RIGHT) playframe = PMAPRIGHTFRAME;
    else if (pshowdir==LEFT) playframe = PMAPLEFTFRAME;
    else if (pshowdir==UP) playframe = PMAPUPFRAME;
    else if (pshowdir==DOWN) playframe = PMAPDOWNFRAME;
	
    // episode 3 map frames start at 31, ep1&2 at 32
    if (m_episode==3) playframe--;
	
    // add in walk frame if walking
    if (pwalking) playframe += pwalkframe;
}

void CPlayer::AllowMountUnmountNessie()
{
	/*int objmarker;
	 if (!player[cp].mounted)
	 {  // not mounted. find out if he's trying to mount
	 // if the upper quarter of the map (mortimer's castle mount point)
	 // he's trying to mount if he's on a NESSIE_MOUNTPOINT object marker
	 // and he's going right and is blockedr and/or is going down and is
	 // blockedd. in the bottom quarter (secret island mount point)
	 // it's up and blockedu.
	 
	 // make sure he's on a mount point
	 objmarker = getlevelat((player[cp].x>>CSF)+4, (player[0].y>>CSF)+9);
	 if (objmarker != NESSIE_MOUNTPOINT) return;
	 
	 // is he trying to mount?
	 if (player[cp].y>>CSF>>4 < map.ysize>>2)
	 {  // at mortimer's castle mount point
	 if ( (player[cp].playcontrol[PA_X] > 0 && player[cp].blockedr) ||
	 (player[cp].playcontrol[PA_Y] > 0 && player[cp].blockedd))
	 {
	 // YES! if nessie is at that mount point, mount her!!
	 MountNessieIfAvailable(cp);
	 }
	 }
	 else
	 {  // at secret island mount point
	 if (player[cp].playcontrol[PA_Y] < 0 && player[cp].blockedu)
	 {
	 MountNessieIfAvailable(cp);
	 }
	 }
	 }
	 else
	 {  // mounted. find out if he's trying to unmount.
	 if (objects[NessieObjectHandle].ai.nessie.pausetimer)
	 {  // nessie is paused
	 if (objects[NessieObjectHandle].y>>CSF>>4 < map.ysize>>2)
	 {  // nessie is at mortimer's castle mount point
	 if (player[cp].playcontrol[PA_Y] < 0)
	 {
	 // unmount nessie
	 objects[NessieObjectHandle].ai.nessie.mounted[cp] = 0;
	 player[cp].mounted = 0;
	 player[cp].hideplayer = 0;
	 }
	 }
	 else if (objects[NessieObjectHandle].y>>CSF>>4 > map.ysize>>1)
	 {  // nessie is at secret island mount point
	 if (player[cp].playcontrol[PA_Y] > 0)
	 {
	 // unmount nessie
	 objects[NessieObjectHandle].ai.nessie.mounted[cp] = 0;
	 player[cp].mounted = 0;
	 player[cp].hideplayer = 0;
	 player[cp].y += (18<<CSF);
	 player[cp].x += (8<<CSF);
	 }
	 }
	 }
	 
	 }*/
}

