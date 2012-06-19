/*
 * CSpriteObjectAI.h
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#ifndef CSPRITEOBJECTAI_H_
#define CSPRITEOBJECTAI_H_

#include "common/CMap.h"
#include "common/CSpriteObject.h"
#include "common/options.h"
#include "common/CPlayer.h"
#include "common/CBehaviorEngine.h"
#include "graphics/CGfxEngine.h"
#include <vector>

class CSpriteObjectAI
{
public:
	CSpriteObjectAI(CMap *p_map, std::vector<CSpriteObject*> &objvect, std::vector<CPlayer> &Player,
			 int NumPlayers, int episode, int level, bool &dark);

	// main functions
	void process();

	virtual ~CSpriteObjectAI();
private:

	// main AI functions
	bool checkforAIObject( CSpriteObject &object );
	void deleteAllObjects();

	void icechunk_ai(CSpriteObject &object);
	void smash(CSpriteObject &object);
	void icebit_ai(CSpriteObject &object);
	void icecannon_ai(CSpriteObject &object);
	void rope_ai(CSpriteObject &object);
	void rope_movestone(CSpriteObject &object);

	// ep2
	void baby_ai(CSpriteObject &object, int episode, bool hard);
	void baby_jump(CSpriteObject &object, int big);

	void scrub_ai(CSpriteObject &object);
	void walkLeft(CSpriteObject &object, int mx, int my);
	void walkRight(CSpriteObject &object, int mx, int my);
	void walkUp(CSpriteObject &object);
	void walkDown(CSpriteObject &object);
	void fall(CSpriteObject &object);
	void preparetoFall(CSpriteObject &object);
	void Scrub_TurnOnCansupportWhereNotKicked(CSpriteObject &object);

	void tankep2_ai(CSpriteObject &object, bool hardmode);
	void tank2_fire(CSpriteObject &object);
	void tank_searchplayers(CSpriteObject &object);

	void platform_ai(CSpriteObject &object);

	void vortelite_ai(CSpriteObject &object, bool darkness);
	void vortelite_initiatejump(CSpriteObject &object);

	// Sector Effector Members
	void se_ai(CSpriteObject &object);
	void se_extend_plat(CSpriteObject &object, bool &PlatExtending);
	void se_retract_plat(CSpriteObject &object, bool &PlatExtending);
	void se_mortimer_arm(CSpriteObject &object);
	void se_mortimer_spark(CSpriteObject &object);
	void se_mortimer_heart(CSpriteObject &object);
	void se_mortimer_zapsup(CSpriteObject &object);
	void se_mortimer_leg_left(CSpriteObject &object);
	void se_mortimer_leg_right(CSpriteObject &object);
	void se_mortimer_randomzaps(CSpriteObject &object);
	void set_mortimer_surprised(bool yes);

	void explosion_ai(CSpriteObject &object);
	void earthchunk_ai(CSpriteObject &object);

	 // ep3
	 void foob_ai(CSpriteObject &object, bool hardmode);
	 void ninja_ai(CSpriteObject &object, bool hardmode);
	 void meep_ai(CSpriteObject& object);
	 void sndwave_ai(CSpriteObject& object, bool hardmode);
	 void mother_ai(CSpriteObject& object, bool hardmode);
	 void fireball_ai(CSpriteObject &object, bool hard);

	 void ballandjack_ai(CSpriteObject& object);
	 bool BJ_BlockedD(CSpriteObject &object);

	 void platvert_ai(CSpriteObject& object);

	 void nessie_ai(CSpriteObject& object);
	 void move_nessie(CSpriteObject& object);
	 void nessie_find_next_checkpoint(CSpriteObject& object);

	 // Common Objects
	void autoray_ai(CSpriteObject &object);
	void gotpoints_ai(CSpriteObject &Object);
	void ray_ai( CSpriteObject &object, bool automatic_raygun, char pShotSpeed=0 );
	void door_ai( CSpriteObject &object, char DoorOpenDir );
	void teleporter_ai(CSpriteObject &object);

	void killplayer(int theplayer);
	void SetAllCanSupportPlayer(CSpriteObject &object, bool state);
	void kill_all_intersecting_tile(int mpx, int mpy, CSpriteObject &theObject);

	// Variables
	CMap *mp_Map;
	std::vector<CSpriteObject*> &m_Objvect;
	std::vector<CPlayer> &m_Player;
	stOption *mp_Options;
	int m_Level;
	int m_Episode;
	int m_gunfiretimer;
	bool &m_dark;
};

#endif /* CSPRITEOBJECTAI_H_ */
