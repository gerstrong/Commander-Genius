/*
 * CPlayer.h
 *
 *  Created on: 05.10.2009
 *      Author: gerstrong
 */

#ifndef CPLAYER_H_
#define CPLAYER_H_

#include <SDL/SDL.h>
#include "inventory.h"
#include "CObject.h"
#include "CMap.h"
#include "CStatusScreen.h"
#include "Playerdefines.h"
#include "options.h"

///
// Class definition starts here!
///
class CPlayer {
public:

	// direction defines used for various things
	enum e_directions{
	RIGHT,LEFT,	UP,	DOWN
	};

	enum e_playingmodes{
		NONE, WORLDMAP, LEVELPLAY
	};

	CPlayer();
	void setMapData(CMap *p_map){	mp_map = p_map;	}

	// World Map specific
	void processWorldMap();
	void setWorldMapdir();
	void selectFrameOnWorldMap();
	void setWMblockedlrud();
	void AllowEnterLevelonWM();
	bool isWMSolid(int xb, int yb);
	void InertiaAndFriction_Y();
	void AllowMountUnmountNessie();
	int getNewObject();

	// In Level specific
	void processInLevel();
	void setDir();
	void getgoodies();
	void playpushed();
	void keencicle();
	void Jump();
	void TogglePogo_and_Switches();
	void JumpAndPogo();
	void raygun();
	void ankh();
	void SelectFrame();

	// Used for both situations
	void InertiaAndFriction_X();
	void Walking();
	void WalkingAnimation();
	void StatusBox();
	void ProcessInput();
	bool drawStatusScreen();

	virtual ~CPlayer();

	///
	// variables
	// these coordinates are CSFed
	unsigned long x;
	unsigned int y;
	unsigned long goto_x;
	unsigned int goto_y;

	unsigned int w;
	unsigned int h;

	char m_playingmode;
	char m_episode;
	int m_player_number;
	// Pointer to the Object type assign to this player
	CObject *mp_object;

	char godmode;

	// used on world map only
	char hideplayer;
	char mounted;

	short treshold;		// This is used for analog devices like joysticks
	signed int pinertia_y;

	unsigned long mapplayx;
	signed int mapplayy;

	unsigned char playframe;

	unsigned char pfalling,plastfalling,pfallspeed,pfallspeed_increasetimer;

	unsigned char pwalking,playspeed;
	unsigned char pslowingdown;
	unsigned char pwalkframe,pwalkframea,pwalkanimtimer;
	unsigned char pwalkincreasetimer, pfriction_timer_x, pfriction_timer_y;
	signed int pinertia_x,pboost_x,playpushed_x;
	int chargedjump;
	unsigned char playpushed_decreasetimer;
	bool widejump;

	bool blockedl,blockedr,blockedu,blockedd;
	unsigned int blockedby;

	unsigned char pjumping, pjumptime, pjumpupspeed_decreasetimer, pjumpdir;
	unsigned char pjumpframe, pjumpanimtimer, pjumpupspeed;
	unsigned char pjumpnormaltime, pjumpupdecreaserate, pjustjumped;
	unsigned char pjustfell;
	unsigned char pjumpfloattimer;

	unsigned char pdir,pshowdir,lastpdir;

	char pfiring,pfireframetimer;
	bool inhibitwalking, inhibitfall;

	int ctrltimer, alttimer;
	char keyprocstate;
	char wm_lastenterstate;

	char pdie, pdieframe, pdietimer;
	int pdietillfly;
	signed int pdie_xvect;
	int psupportingtile, psupportingobject, lastsupportingobject;
	char psliding;
	char psemisliding;
	bool ppogostick;
	int pfrozentime,pfrozenframe,pfrozenanimtimer;

	// New values
	char playcontrol[PA_MAX_ACTIONS];

	char x_friction;
	char y_friction;

	// End new values

	unsigned char dpadcount, dpadlastcount;

	unsigned int ankhtime, ankhshieldobject;

	stInventory inventory;

	bool m_godmode, m_cheats_enabled;
	bool *mp_levels_completed;
	bool m_showStatusScreen;

	CMap *mp_map;
	stOption *mp_option;

private:
	CStatusScreen *mp_StatusScr;
};

#endif /* CPLAYER_H_ */
