/*
 * CObjectAI.h
 *
 *  Created on: 26.10.2009
 *      Author: gerstrong
 */

#ifndef COBJECTAI_H_
#define COBJECTAI_H_

#include "../../keen.h"
#include "../../game.h"
#include "../../common/CMap.h"
#include "../../common/CObject.h"
#include "../../common/options.h"
#include "../../common/CPlayer.h"
#include "../../common/CPhysicsSettings.h"
#include "../../graphics/CGfxEngine.h"
#include <vector>

class CObjectAI {
public:
	CObjectAI(CMap *p_map, std::vector<CObject> &objvect, CPlayer *p_player,
			 stOption *p_options, int NumPlayers, int episode, int level,
			 char difficulty, CPhysicsSettings &PhysicsSettings);

	// main functions
	void process();

	virtual ~CObjectAI();
private:

	// main AI functions
	bool checkforAIObject( CObject &object );
	void performSpecialAIType( CObject &object );
	void deleteObj(CObject &object);

	// ep1
	void yorp_ai(CObject &object, CPlayer *p_player, bool hardmode);
	void garg_ai(CObject &object, CPlayer *p_player, bool hardmode);
	void vort_ai(CObject &object, int level, int episode, char difficulty, bool dark);
	void vort_initiatejump(CObject &object);
	void butler_ai(CObject &object, char difficulty);
	void tank_ai(CObject &object, bool hardmode);
	bool tank_CanMoveLeft(CObject &object, int h);
	bool tank_CanMoveRight(CObject &object, int w, int h);

	void icechunk_ai(CObject &object);
	void smash(CObject &object);
	void icebit_ai(CObject &object);
	void icecannon_ai(CObject &object);
	void rope_ai(CObject &object);
	void rope_movestone(CObject &object);

	// ep2
	//void baby_ai(CObject &object, int episode, bool hard)
	//void scrub_ai(int o, stLevelControl levelcontrol);
	//void tankep2_ai(int o, bool hardmode);
	//void platform_ai(int o, stLevelControl levelcontrol);
	//void vortelite_ai(int o, bool darkness);
	//void se_ai(int o, stLevelControl *p_levelcontrol);
	//void explosion_ai(int o);
	//void earthchunk_ai(int o);
	//void spark_ai(int o, int *p_sparks_left);

	 /*// ep3
	 void foob_ai(int o, bool hardmode);
	 void ninja_ai(int o, bool hardmode);
	 void meep_ai(int o, stLevelControl levelcontrol);
	 void sndwave_ai(int o, bool hardmode);
	 void mother_ai(int o, stLevelControl levelcontrol);
	 void fireball_ai(int o, bool hard);
	 void ballandjack_ai(int o);
	 void platvert_ai(int o);
	 void nessie_ai(int o);*/

	 // Common Objects
	void autoray_ai(CObject &object);
	void gotpoints_ai(CObject &Object);
	void ray_ai( CObject &object, bool automatic_raygun, char pShotSpeed=0 );
	void door_ai( CObject &object, char DoorOpenDir );
	void teleporter_ai(CObject &object);

	void killplayer(int theplayer);
	void SetAllCanSupportPlayer(CObject &object, int state);
	void kill_all_intersecting_tile(int mpx, int mpy);

	// Variables
	CMap *mp_Map;
	std::vector<CObject> &m_Objvect;
	CPlayer *mp_Player;
	stOption *mp_Options;
	CPhysicsSettings m_PhysicsSettings;
	int m_Level;
	int m_Episode;
	int m_NumPlayers;
	char m_difficulty;
	int m_gunfiretimer;
};

#endif /* COBJECTAI_H_ */
