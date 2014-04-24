/*
 * CVorticonSpriteObjectAI.h
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#ifndef __CVORTICONSPRITEOBJECTAI_H_
#define __CVORTICONSPRITEOBJECTAI_H_

#include "common/CMap.h"
#include "common/CSpriteObject.h"
#include "common/options.h"
#include "CPlayer.h"
#include "engine/core/CBehaviorEngine.h"
#include "graphics/GsGraphics.h"
#include <vector>

class CVorticonSpriteObjectAI
{
public:
	CVorticonSpriteObjectAI(CMap *p_map, 
				std::vector< std::unique_ptr<CVorticonSpriteObject> > &objvect,
				std::vector<CPlayer> &Player,
				int NumPlayers, int episode, 
				int level, bool &dark);

    void pumpEvent(const CEvent *evPtr);

	// main functions
	void process();

private:

	// main AI functions
	/*bool checkforAIObject( CVorticonSpriteObject &object );

	void icechunk_ai(CVorticonSpriteObject &object);
	void smash(CVorticonSpriteObject &object);
	void icebit_ai(CVorticonSpriteObject &object);
	void icecannon_ai(CVorticonSpriteObject &object);
	void rope_ai(CVorticonSpriteObject &object);
	void rope_movestone(CVorticonSpriteObject &object);

	// ep2
	void baby_ai(CVorticonSpriteObject &object, int episode, bool hard);
	void baby_jump(CVorticonSpriteObject &object, int big);

	void scrub_ai(CVorticonSpriteObject &object);
	void walkLeft(CVorticonSpriteObject &object, int mx, int my);
	void walkRight(CVorticonSpriteObject &object, int mx, int my);
	void walkUp(CVorticonSpriteObject &object);
	void walkDown(CVorticonSpriteObject &object);
	void fall(CVorticonSpriteObject &object);
	void preparetoFall(CVorticonSpriteObject &object);
	void Scrub_TurnOnCansupportWhereNotKicked(CVorticonSpriteObject &object);

	void tankep2_ai(CVorticonSpriteObject &object, bool hardmode);
	void tank2_fire(CVorticonSpriteObject &object);
	void tank_searchplayers(CVorticonSpriteObject &object);

	void platform_ai(CVorticonSpriteObject &object);

	void vortelite_ai(CVorticonSpriteObject &object, bool darkness);
	void vortelite_initiatejump(CVorticonSpriteObject &object);*/

	// Sector Effector Members
	/*void se_ai(CVorticonSpriteObject &object);
	void se_extend_plat(CVorticonSpriteObject &object, bool &PlatExtending);
	void se_retract_plat(CVorticonSpriteObject &object, bool &PlatExtending);
	void se_mortimer_arm(CVorticonSpriteObject &object);
	void se_mortimer_spark(CVorticonSpriteObject &object);
	void se_mortimer_heart(CVorticonSpriteObject &object);
	void se_mortimer_zapsup_nearby(CVorticonSpriteObject &object);
	void se_mortimer_zapsup(CVorticonSpriteObject &object);
	void se_mortimer_leg_left(CVorticonSpriteObject &object);
	void se_mortimer_leg_right(CVorticonSpriteObject &object);
	void se_mortimer_randomzaps(CVorticonSpriteObject &object);
	void set_mortimer_surprised(bool yes);*/

	/*void explosion_ai(CVorticonSpriteObject &object);
	void earthchunk_ai(CVorticonSpriteObject &object);

	 // ep3
	 void foob_ai(CVorticonSpriteObject &object, bool hardmode);
	 void ninja_ai(CVorticonSpriteObject &object, bool hardmode);
	 void meep_ai(CVorticonSpriteObject& object);
	 void sndwave_ai(CVorticonSpriteObject& object, bool hardmode);
	 void mother_ai(CVorticonSpriteObject& object, bool hardmode);
	 void fireball_ai(CVorticonSpriteObject &object, bool hard);

	 void ballandjack_ai(CVorticonSpriteObject& object);
	 bool BJ_BlockedD(CVorticonSpriteObject &object);

	 void platvert_ai(CVorticonSpriteObject& object);

	 void nessie_ai(CVorticonSpriteObject& object);
	 void move_nessie(CVorticonSpriteObject& object);
	 void nessie_find_next_checkpoint(CVorticonSpriteObject& object);*/

	 // Common Objects
	/*void autoray_ai(CVorticonSpriteObject &object);
	void gotpoints_ai(CVorticonSpriteObject &Object);
	void ray_ai( CVorticonSpriteObject &object, bool automatic_raygun, char pShotSpeed=0 );
	void door_ai( CVorticonSpriteObject &object, char DoorOpenDir );
	void teleporter_ai(CVorticonSpriteObject &object);

	void killplayer(int theplayer);
	void kill_all_intersecting_tile(int mpx, int mpy, CVorticonSpriteObject &theObject);*/

	// Variables
	CMap *mp_Map;
	std::vector< std::unique_ptr<CVorticonSpriteObject> > &m_Objvect;
	std::vector<CPlayer> &m_Player;
	stOption *mp_Options;
	int m_Level;
	int m_Episode;
	int m_gunfiretimer;
	bool &m_dark;
};

#endif // __CVORTICONSPRITEOBJECTAI_H_
