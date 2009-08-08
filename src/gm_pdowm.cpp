/* GM_PDOWM.C
  Contains all of the gamepdo_wm_xxx functions...which are called from the
  main game loop. These functions perform some task that is done each
  time around the game loop and is related to the player when he is on
  the world map. Basically gamepdo.c but has slightly different versions
  of some functions to handle the world map.
*/

#include "keen.h"

#include "sdl/CInput.h"
#include "include/game.h"
#include "include/misc.h"
#include "include/gm_pdowm.h"
#include "include/gamepdo.h"
#include "sdl/sound/CSound.h"
#include "hqp/CMusic.h"

void AllowMountUnmountNessie(int cp);
void MountNessieIfAvailable(int cp);

// player handler mother-function, calls all
// needed "gamepdo" functions for player cp,
// this is the world map version
void gamepdo_wm_HandlePlayer(int cp, stCloneKeenPlus *pCKP)
{
    player[cp].inhibitwalking = 0;
    player[cp].inhibitfall = 0;

    gamepdo_StatusBox(cp, pCKP);

    gamepdo_ProcessInput(cp, pCKP);
    gamepdo_wm_setdir(cp, pCKP);

    gamepdo_wm_setblockedlrud(cp, pCKP);
    gamepdo_wm_AllowEnterLevel(cp, pCKP);

    gamepdo_walking(cp, pCKP);
    gamepdo_walkinganim(cp, pCKP);

    if (fade.mode==NO_FADE)
    {
      gamepdo_InertiaAndFriction_X(cp, pCKP);
      gamepdo_InertiaAndFriction_Y(cp, pCKP);
    }

    if (pCKP->Control.levelcontrol.episode==3)
    {
      AllowMountUnmountNessie(cp);
    }

    gamepdo_wm_SelectFrame(cp);

    // copy player's keytable to lastkeytable
    memcpy(&player[cp].lastkeytable, &player[cp].keytable, sizeof(player[cp].lastkeytable));
}

// select the appropriate player frame based on what he's doing
void gamepdo_wm_SelectFrame(int cp)
{
    // select base frame for current direction
    if (player[cp].pshowdir==RIGHT) player[cp].playframe = PMAPRIGHTFRAME;
    else if (player[cp].pshowdir==LEFT) player[cp].playframe = PMAPLEFTFRAME;
    else if (player[cp].pshowdir==UP) player[cp].playframe = PMAPUPFRAME;
    else if (player[cp].pshowdir==DOWN) player[cp].playframe = PMAPDOWNFRAME;

    // no walking animation if we're fading
    if (fade.mode != NO_FADE) return;

    // add in walk frame if walking
    if (player[cp].pwalking) player[cp].playframe += player[cp].pwalkframe;
}

// set blockedl and blockedr...is Keen up against a solid object?
void gamepdo_wm_setblockedlrud(int cp, stCloneKeenPlus *pCKP)
{
   player[cp].blockedl = player[cp].blockedr = 0;
   player[cp].blockedu = player[cp].blockedd = 0;

   // cheat: holding down TAB will turn off clipping. or if you are in godmode
   if ((pCKP->Option[OPT_CHEATS].value && g_pInput->getHoldedKey(KTAB)) || player[cp].godmode) return;

   // R
   if (wm_issolid((player[cp].x>>CSF)+8, (player[cp].y>>CSF)+1, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedr = 1; }
   else if (wm_issolid((player[cp].x>>CSF)+8, (player[cp].y>>CSF)+8, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedr = 1; }
   else if (wm_issolid((player[cp].x>>CSF)+8, (player[cp].y>>CSF)+13, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedr = 1; }

   // L
   if (wm_issolid((player[cp].x>>CSF)+0, (player[cp].y>>CSF)+1, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedl = 1; }
   else if (wm_issolid((player[cp].x>>CSF)+0, (player[cp].y>>CSF)+8, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedl = 1; }
   else if (wm_issolid((player[cp].x>>CSF)+0, (player[cp].y>>CSF)+13, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedl = 1; }

   // U
   if (wm_issolid((player[cp].x>>CSF)+1, (player[cp].y>>CSF)-1, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedu = 1; }
   else if (wm_issolid((player[cp].x>>CSF)+4, (player[cp].y>>CSF)-1, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedu = 1; }
   else if (wm_issolid((player[cp].x>>CSF)+7, (player[cp].y>>CSF)-1, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedu = 1; }

   // D
   if (wm_issolid((player[cp].x>>CSF)+1, (player[cp].y>>CSF)+14, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedd = 1; }
   else if (wm_issolid((player[cp].x>>CSF)+4, (player[cp].y>>CSF)+14, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedd = 1; }
   else if (wm_issolid((player[cp].x>>CSF)+7, (player[cp].y>>CSF)+14, pCKP->Control.levelcontrol.levels_completed))
      { player[cp].blockedd = 1; }

}

// select proper player direction
void gamepdo_wm_setdir(int cp, stCloneKeenPlus *pCKP)
{
int exception;
   player[cp].dpadcount = 0;
   if (player[cp].playcontrol[PA_X] < 0) { player[cp].pdir = LEFT; player[cp].dpadcount++; }
   if (player[cp].playcontrol[PA_X] > 0) { player[cp].pdir = RIGHT; player[cp].dpadcount++; }
   if (player[cp].playcontrol[PA_Y] < 0) { player[cp].pdir = UP; player[cp].dpadcount++; }
   if (player[cp].playcontrol[PA_Y] > 0) { player[cp].pdir = DOWN; player[cp].dpadcount++; }

   exception = 0;
   if (player[cp].pshowdir==UP && !player[cp].playcontrol[PA_Y] < 0) exception = 1;
   if (player[cp].pshowdir==DOWN && !player[cp].playcontrol[PA_Y] > 0) exception = 1;
   if (player[cp].pshowdir==LEFT && !player[cp].playcontrol[PA_X] < 0) exception = 1;
   if (player[cp].pshowdir==RIGHT && !player[cp].playcontrol[PA_X] > 0) exception = 1;
   if ((player[cp].playcontrol[PA_Y] < 0) && (player[cp].playcontrol[PA_Y] > 0)) player[cp].pshowdir = DOWN;
   if ((player[cp].playcontrol[PA_X] < 0) && (player[cp].playcontrol[PA_X] > 0)) player[cp].pshowdir = RIGHT;

   if (player[cp].dpadcount==1 || player[cp].dpadlastcount==0 || exception)
   {
     player[cp].pshowdir = player[cp].pdir;
   }
   player[cp].dpadlastcount = player[cp].dpadcount;
}

// handles inertia and friction for the Y direction
// (this is where the inertia is actually applied to playx)
void gamepdo_InertiaAndFriction_Y(int cp, stCloneKeenPlus *pCKP)
{
   if (player[cp].hideplayer)
   {
     player[cp].pinertia_y = 0;
     return;
   }

   // if we hit a solid object kill all inertia
   if (player[cp].pinertia_y > 0 && player[cp].blockedd)
   {
     player[cp].pinertia_y = 0;
   }
   else if (player[cp].pinertia_y < 0 && player[cp].blockedu)
   {
     player[cp].pinertia_y = 0;
   }

   // apply pinertia_y
   player[cp].y += player[cp].pinertia_y;

   // if we stopped walking (i.e. LRUD not held down) apply friction
    //if (!player[cp].keytable[KDOWN] && !player[cp].keytable[KUP])
    if (!player[cp].playcontrol[PA_Y])
    {
      // and apply friction to pinertia_y
      if (player[cp].pfriction_timer_y > PFRICTION_RATE_WM)
      {
        if (player[cp].pinertia_y < 0) player[cp].pinertia_y++;
        else if (player[cp].pinertia_y > 0) player[cp].pinertia_y--;
        player[cp].pfriction_timer_y = 0;
      }
      else player[cp].pfriction_timer_y++;
    }
}

void gamepdo_wm_AllowEnterLevel(int cp, stCloneKeenPlus *pCKP)
{
int lvl;
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

    if (fade.mode != NO_FADE) return;
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
          /* handle special map objects, and standard levels too */

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
                o = spawn_object(x<<CSF,y<<CSF,OBJ_TELEPORTER);
                objects[o].ai.teleport.direction = TELEPORTING_OUT;
                objects[o].ai.teleport.destx = destx << 4 << CSF;
                objects[o].ai.teleport.desty = desty << 4 << CSF;
                objects[o].ai.teleport.whichplayer = cp;
                objects[o].ai.teleport.NoExitingTeleporter = 0;
                objects[o].ai.teleport.snap = telsnap;
                if (p_levelcontrol->episode==3)
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
                if (p_levelcontrol->episode==1)
                {
                  YourShipNeedsTheseParts(pCKP);
                }
                else
                {
                  ShipEp3(pCKP);
                }
                break;

              default:      // a regular level
            	p_levelcontrol->chglevelto = (lvl & 0x7fff);
                endlevel(1, &(pCKP->Control.levelcontrol));
                g_pMusicPlayer->stop();
                g_pSound->playStereofromCoord(SOUND_ENTER_LEVEL, PLAY_NOW, objects[player[cp].useObject].scrx);

                break;

              } // end switch(level)
          } // end isn't a teleporter
        } // end if(lvl)
    } // end trying to enter a level

    player[cp].wm_lastenterstate = (player[cp].playcontrol[PA_JUMP] || player[cp].playcontrol[PA_POGO]);
}

char wm_issolid(int xb, int yb, int *levels_completed)
{

  // for map tiles solidl and solidr are always gonna be the same...
  // so we can get away with this.
  if ( TileProperty[getmaptileat(xb, yb)][BLEFT] ) return 1;
  //if (tiles[getmaptileat(xb, yb)].solidl) return 1;

  unsigned int level;
  level = getlevelat(xb, yb);

  if (level & 0x8000)
  {
	  if(levels_completed[map.objectlayer[xb>>4][yb>>4] & 0x7fff] && options[OPT_LVLREPLAYABILITY].value) // check if level is done, but can be replayed
		  return 0;

	  if(g_pInput->getHoldedKey(KTAB) && g_pInput->getHoldedKey(KLSHIFT))
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
  }
  return 0;
}

// if nessie is at a mount point near the player, mounts him!
void MountNessieIfAvailable(int cp)
{
char AtSameMountPoint;

   if (!player[cp].mounted)
   {
      // is nessie paused?
      if (objects[NessieObjectHandle].ai.nessie.pausetimer)
      {
         // is she at the same mount point the player is?
         if (player[cp].y>>CSF>>4 < map.ysize>>2 && objects[NessieObjectHandle].y>>CSF>>4 < map.ysize>>2) AtSameMountPoint = 1;    // both at mortimer's castle
         else if (player[cp].y>>CSF>>4 > map.ysize>>1 && objects[NessieObjectHandle].y>>CSF>>4 > map.ysize>>1) AtSameMountPoint = 1;    // both at secret island
         else AtSameMountPoint = 0;

         if (AtSameMountPoint)
         {
            objects[NessieObjectHandle].ai.nessie.mounted[cp] = 1;
            player[cp].mounted = 1;
            player[cp].hideplayer = 1;
         }
      }
   }
}

void AllowMountUnmountNessie(int cp)
{
int objmarker;
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

   }
}
