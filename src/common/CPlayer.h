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

// scroll triggers
#define SCROLLTRIGGERRIGHT     210
#define SCROLLTRIGGERLEFT      130
#define SCROLLTRIGGERUP        100
#define SCROLLTRIGGERDOWN      140

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
	void setDatatoZero();
	void setDefaultStartValues();
	void setMapData(CMap *p_map){ mp_map=p_map; }
	void setPhysics(CPhysicsSettings *physics) { mp_PhysicsSettings = physics; }

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
	void kill();
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
	int m_player_number;
	// Pointer to the Objects
	std::vector<CObject> *mp_object;

	char godmode;

	// used on world map only
	char hideplayer;
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
	int psupportingtile, psupportingobject, lastsupportingobject;
	char psliding;
	char psemisliding;
	bool ppogostick;
	int pfrozentime,pfrozenframe,pfrozenanimtimer;
	bool pfiring, plastfire;

	char playcontrol[PA_MAX_ACTIONS];

	unsigned char dpadcount, dpadlastcount;

	unsigned int ankhtime, ankhshieldobject;

	stInventory inventory;

	bool m_godmode, m_cheats_enabled;
	bool *mp_levels_completed;
	bool m_showStatusScreen;
	char level_done;
	bool beingteleported;

	CMap *mp_map;
	stOption *mp_option;

private:
	CStatusScreen *mp_StatusScr;
	CPhysicsSettings *mp_PhysicsSettings;

	bool lastpogo;

	// Level control specific functions, especially for exit
	int exitXpos;
	int level_done_timer;

	// defined under CPlayerItems.cpp
	bool getGoodie(int px, int py);
	void procGoodie(int t, int mpx, int mpy);
	void riseBonus(int spr, int x, int y);
	void getBonuspoints(int numpts, int mpx, int mpy);
	void incScore(int numpts);
	void openDoor(int doortile, int doorsprite, int mpx, int mpy);
	void giveAnkh();
};

#endif /* CPLAYER_H_ */
