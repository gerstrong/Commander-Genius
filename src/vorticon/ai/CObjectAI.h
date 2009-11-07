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
#include "../../graphics/CGfxEngine.h"
#include <vector>

class CObjectAI {
public:
	CObjectAI(CMap *p_map, std::vector<CObject> *p_objvect, CPlayer *p_player,
			  stOption *p_options, int NumPlayers, int episode, char difficulty);
	virtual ~CObjectAI();
	
	// main functions
	void process();
	
private:
	
	// main AI functions
	bool checkforAIObject( CObject *p_object );
	void performSpecialAIType( CObject *p_object );
	void performCommonAI( CObject *p_object );
	void deleteObj(CObject *p_object);
	
	// ep1
	void yorp_ai(CObject *p_object, CPlayer *p_player, bool hardmode);
	/*void garg_ai(int o, bool hardmode);
	 void vort_ai(int o, stLevelControl *p_levelcontrol);
	 void butler_ai(int o, bool hardmode);
	 void tank_ai(int o, bool hardmode);
	 void icechunk_ai(int o);
	 void icebit_ai(int o);
	 void icecannon_ai(int o);
	 void rope_ai(int o);
	 
	 // ep2
	 void scrub_ai(int o, stLevelControl levelcontrol);
	 void tankep2_ai(int o, bool hardmode);
	 void platform_ai(int o, stLevelControl levelcontrol);
	 void vortelite_ai(int o, bool darkness);
	 void se_ai(int o, stLevelControl *p_levelcontrol);
	 void explosion_ai(int o);
	 void earthchunk_ai(int o);
	 void spark_ai(int o, int *p_sparks_left);
	 
	 // ep3
	 void foob_ai(int o, bool hardmode);
	 void ninja_ai(int o, bool hardmode);
	 void meep_ai(int o, stLevelControl levelcontrol);
	 void sndwave_ai(int o, bool hardmode);
	 void mother_ai(int o, stLevelControl levelcontrol);
	 void fireball_ai(int o, bool hard);
	 void ballandjack_ai(int o);
	 void platvert_ai(int o);
	 void nessie_ai(int o);
	 
	 // Common Objects
	 void autoray_ai(int o);
	 void gotpoints_ai(int o);*/
	void ray_ai( CObject *p_object, bool automatic_raygun, char pShotSpeed=0 );
	void door_ai( CObject *p_object, char DoorOpenDir );
	void teleporter_ai(CObject *p_object);
	
	// Variables
	CMap *mp_Map;
	std::vector<CObject> *mp_Objvect;
	CPlayer *mp_Player;
	stOption *mp_Options;
	int m_Episode;
	int m_NumPlayers;
	char m_difficulty;
};

#endif /* COBJECTAI_H_ */
