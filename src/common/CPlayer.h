/*
 * CPlayer.h
 *
 *  Created on: 05.10.2009
 *      Author: gerstrong
 */

#ifndef CPLAYER_H_
#define CPLAYER_H_

#include <SDL.h>
#include "inventory.h"
#include "CObject.h"
#include "CMap.h"
#include "CStatusScreen.h"
#include "Playerdefines.h"
#include "CPhysicsSettings.h"
#include "options.h"
#include <vector>
#include <string>

// scroll triggers
const int SCROLLTRIGGERRIGHT = 210;
const int SCROLLTRIGGERLEFT = 130;
const int SCROLLTRIGGERUP = 100;
const int SCROLLTRIGGERDOWN = 140;

enum level_triggers{
	LVLTRIG_NONE, LVLTRIG_TANTALUS_RAY, LVLTRIG_BRIDGE, LVLTRIG_LIGHT
};

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

	CPlayer(char &Episode, short &Level, char &Difficulty,
			 short &player_index, bool *mp_level_completed, stOption *mp_option,
			 std::vector<CObject> &m_Object);
	void setDatatoZero();
	void setDefaultStartValues();
	void setMapData(CMap *p_map){ mp_map=p_map; }
	void setPhysics(CPhysicsSettings *physics) { mp_PhysicsSettings = physics; }
	bool getLevelTrigger();

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
	void touchedExit();
	void walkbehindexitdoor();
	void kill(bool force=false);
	void dieanim();
	void setDir();
	void getgoodies();
	void playpushed();
	void keencicle();
	void TogglePogo_and_Switches();
	void JumpAndPogo();
	void raygun();
	void ankh();
	void bump( int pushamt, bool solid );
	void SelectFrame();
	int pollLevelTrigger();

	// Used for both situations
	void InertiaAndFriction_X();
	void Walking();
	void WalkingAnimation();
	void StatusBox();
	void ProcessInput();
	bool drawStatusScreen();
	bool scrollTriggers();
	void give_keycard(int doortile);
	void take_keycard(int doortile);
	void freeze();
	bool showGameHint(int mpx, int mpy);
	std::string pollHintMessage();

	virtual ~CPlayer();

	///
	// variables
	// these coordinates are CSFed
	int playerbaseframe;	// Standframe of the player normally. May be different for other players.
	unsigned long x;
	unsigned int y;
	unsigned long goto_x;
	unsigned int goto_y;

	unsigned int w;
	unsigned int h;

	char m_playingmode;
	char m_episode;
	char m_level;
	int m_difficulty;
	int m_player_number;

	// Pointer to the Objects
	std::vector<CObject> *mp_object;

	char godmode;

	// used on world map only
	bool hideplayer;
	char mounted;

	short treshold;		// This is used for analog devices like joysticks
	signed int pinertia_y;

	unsigned long mapplayx;
	signed int mapplayy;

	unsigned char playframe;

	unsigned char pfalling,plastfalling,pfallspeed;

	unsigned char pwalking,playspeed;
	unsigned char pslowingdown;
	unsigned char pwalkframe,pwalkframea;
	int pwalkanimtimer;
	unsigned char pwalkincreasetimer, pfriction_timer_x, pfriction_timer_y;
	signed int pinertia_x, playpushed_x;
	unsigned char playpushed_decreasetimer;

	bool blockedl,blockedr,blockedu,blockedd;
	unsigned int blockedby;

	unsigned char pjumping, pjumptime, pjumpupspeed_decrease, pjumpdir;
	unsigned char pjumpframe, pjumpanimtimer;
	int pjumpupspeed;
	unsigned char pjumpnormaltime, pjumpupdecreaserate, pjustjumped;
	unsigned char pjustfell;
	unsigned char pjumpfloattimer;

	unsigned char pdir,pshowdir,lastpdir;

	char pfireframetimer;
	bool inhibitwalking, inhibitfall;

	int ctrltimer, alttimer;
	char keyprocstate;
	char wm_lastenterstate;

	char pdie, pdieframe, pdietimer;
	int pdietillfly;
	signed int pdie_xvect;
	unsigned int psupportingtile, psupportingobject, lastsupportingobject;
	bool psliding;
	bool psemisliding;
	bool ppogostick;
	bool pogofirsttime;
	int pfrozentime,pfrozenframe,pfrozenanimtimer;
	bool pfiring, plastfire;

	char playcontrol[PA_MAX_ACTIONS];

	bool object_chosen;
	unsigned char dpadcount, dpadlastcount;

	unsigned int ankhtime, ankhshieldobject;

	stInventory inventory;

	bool m_cheats_enabled;
	bool *mp_levels_completed;
	bool m_showStatusScreen;
	char level_done;
	bool beingteleported;

	std::string hintstring;

	CMap *mp_map;
	stOption *mp_option;

private:
	CStatusScreen *mp_StatusScr;
	CPhysicsSettings *mp_PhysicsSettings;

	bool lastpogo;

	// Level control specific functions, especially for exit
	int exitXpos;
	int level_done_timer;

	int m_Level_Trigger;

	// defined under CPlayerItems.cpp
	bool getGoodie(int px, int py);
	void procGoodie(int tile, int mpx, int mpy);
	void riseBonus(int spr, int x, int y);
	void getBonuspoints(int numpts, int mpx, int mpy);
	void incScore(int numpts);
	void openDoor(int doortile, int doorsprite, int mpx, int mpy);
	void giveAnkh();
};

#endif /* CPLAYER_H_ */
