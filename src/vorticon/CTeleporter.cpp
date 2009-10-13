/*
 * CTeleporter.cpp
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 *
 *  This class was part of the player structure,
 *  but as it got too complex, I decided to separate that teleporter code.
 */

#include "CTeleporter.h"

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

CTeleporter::CTeleporter() {
	// TODO Auto-generated constructor stub

}

bool CTeleporter::readTeleporterInfo()
{
    // handle special map objects, and standard levels too

    // look through the teleporter tables and see if this is
    // a teleporter
    /*telfrom = 0;
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
    }*/
	return false;
}

void CTeleporter::teleportPlayer()
{
    // need to teleport somewhere?
    /*if (telfrom)
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

    // ep1 bonus teleporter
    // TODO: This must be coded more extensive
	case LVLS_TELEPORTER_BONUS:
	o = spawn_object(x<<CSF,y<<CSF,OBJ_TELEPORTER);
	m_Object[o].ai.teleport.direction = TELEPORTING_OUT;
	m_Object[o].ai.teleport.destx = TELEPORT_BONUS_DESTX;
	m_Object[o].ai.teleport.desty = TELEPORT_BONUS_DESTY;
	m_Object[o].ai.teleport.whichplayer = cp;
	m_Object[o].ai.teleport.baseframe = TELEPORT_RED_BASEFRAME_EP1;
	m_Object[o].ai.teleport.idleframe = TELEPORT_RED_IDLEFRAME_EP1;
	m_Object[o].ai.teleport.NoExitingTeleporter = 1;
	m_Object[o].ai.teleport.snap = 1;
	mp_Player[cp].hideplayer = 1;
	break;
    */
}

CTeleporter::~CTeleporter() {
	// TODO Auto-generated destructor stub
}
