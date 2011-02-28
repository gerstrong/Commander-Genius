/*
 * CObjectAI.h
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#ifndef COBJECTAI_H_
#define COBJECTAI_H_

#include "keen.h"
#include "common/CMap.h"
#include "common/CObject.h"
#include "common/options.h"
#include "common/CPlayer.h"
#include "common/CBehaviorEngine.h"
#include "graphics/CGfxEngine.h"
#include <vector>

class CObjectAI {
public:
	CObjectAI(CMap *p_map, std::vector<CObject*> &objvect, std::vector<CPlayer> &Player,
			 int NumPlayers, int episode, int level,
			 Uint8& difficulty, bool &dark);

	// main functions
	void process();

	virtual ~CObjectAI();
private:

	// main AI functions
	bool checkforAIObject( CObject &object );
	void deleteAllObjects();

	void icechunk_ai(CObject &object);
	void smash(CObject &object);
	void icebit_ai(CObject &object);
	void icecannon_ai(CObject &object);
	void rope_ai(CObject &object);
	void rope_movestone(CObject &object);

	// ep2
	void baby_ai(CObject &object, int episode, bool hard);
	void baby_jump(CObject &object, int big);

	void scrub_ai(CObject &object);
	void walkLeft(CObject &object, int mx, int my);
	void walkRight(CObject &object, int mx, int my);
	void walkUp(CObject &object);
	void walkDown(CObject &object);
	void fall(CObject &object);
	void preparetoFall(CObject &object);
	void Scrub_TurnOnCansupportWhereNotKicked(CObject &object);

	void tankep2_ai(CObject &object, bool hardmode);
	void tank2_fire(CObject &object);
	void tank_searchplayers(CObject &object);

	void platform_ai(CObject &object);

	void vortelite_ai(CObject &object, bool darkness);
	void vortelite_initiatejump(CObject &object);

	// Sector Effector Members
	void se_ai(CObject &object);
	void se_extend_plat(CObject &object, bool &PlatExtending);
	void se_retract_plat(CObject &object, bool &PlatExtending);
	void se_mortimer_arm(CObject &object);
	void se_mortimer_spark(CObject &object);
	void se_mortimer_heart(CObject &object);
	void se_mortimer_zapsup(CObject &object);
	void se_mortimer_leg_left(CObject &object);
	void se_mortimer_leg_right(CObject &object);
	void se_mortimer_randomzaps(CObject &object);
	void set_mortimer_surprised(bool yes);

	void explosion_ai(CObject &object);
	void earthchunk_ai(CObject &object);

	 // ep3
	 void foob_ai(CObject &object, bool hardmode);
	 void ninja_ai(CObject &object, bool hardmode);
	 void meep_ai(CObject& object);
	 void sndwave_ai(CObject& object, bool hardmode);
	 void mother_ai(CObject& object, bool hardmode);
	 void fireball_ai(CObject &object, bool hard);

	 void ballandjack_ai(CObject& object);
	 bool BJ_BlockedD(CObject &object);

	 void platvert_ai(CObject& object);

	 void nessie_ai(CObject& object);
	 void move_nessie(CObject& object);
	 void nessie_find_next_checkpoint(CObject& object);

	 // Common Objects
	void autoray_ai(CObject &object);
	void gotpoints_ai(CObject &Object);
	void ray_ai( CObject &object, bool automatic_raygun, char pShotSpeed=0 );
	void door_ai( CObject &object, char DoorOpenDir );
	void teleporter_ai(CObject &object);

	void killplayer(int theplayer);
	void SetAllCanSupportPlayer(CObject &object, bool state);
	void kill_all_intersecting_tile(int mpx, int mpy, CObject &theObject);

	// Variables
	CMap *mp_Map;
	std::vector<CObject*> &m_Objvect;
	std::vector<CPlayer> &m_Player;
	stOption *mp_Options;
	int m_Level;
	int m_Episode;
	Uint8& m_difficulty;
	int m_gunfiretimer;
	bool &m_dark;
	unsigned int m_bgtile; // Used as holder for bridges

};

#endif /* COBJECTAI_H_ */
