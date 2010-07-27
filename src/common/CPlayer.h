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
#include "options.h"
#include <vector>
#include <string>

enum level_triggers{
	LVLTRIG_NONE, LVLTRIG_TANTALUS_RAY, LVLTRIG_BRIDGE, LVLTRIG_LIGHT
};

///
// Class definition starts here!
///
class CPlayer : public CObject {
public:

	// direction defines used for various things
	enum e_playingmodes{
		NONE, WORLDMAP, LEVELPLAY
	};

	CPlayer(const char &Episode, short &Level, char &Difficulty,
			 bool *mp_level_completed, stOption *mp_option,
			 std::vector<CObject*> &m_Object, CMap &map);
	void setDatatoZero();
	void setDefaultStartValues();
	void setMapData(CMap *p_map){ mp_Map=p_map; }
	void setupforLevelPlay();
	bool getLevelTrigger();

	// World Map specific
	void processWorldMap();
	void setWorldMapdir();
	void selectFrameOnWorldMap();
	void AllowEnterLevelonWM();
	void verifySolidLevels();
	bool isWMSolid(int xb, int yb);
	void InertiaAndFriction_Y();
	void MountNessieIfAvailable();
	void UnmountNessie();
	int getNewObject();

	// In Level specific
	void processInLevel();
	void touchedExit(int mpx);
	void walkbehindexitdoor();
	void kill(bool force);
	void kill();
	void dieanim();
	void setDir();
	void getgoodies();
	void keencicle();
	void TogglePogo_and_Switches();
	void JumpAndPogo();
	void Playerfalling();
	void raygun();
	void bump( CObject &theObject, direction_t direction );
	void push( CObject &theObject );
	void checkSolidDoors();
	void SelectFrame();
	int pollLevelTrigger();
	void getShotByRay();

	// Used for both situations
	void InertiaAndFriction_X();
	void Walking();
	void WalkingAnimation();
	void StatusBox();
	void ProcessInput();
	void ProcessExitLevel();

	bool checkObjSolid();

	void draw();
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

	unsigned int w;
	unsigned int h;

	e_playingmodes m_playingmode;
	char m_episode;
	char m_level;
	int m_difficulty;

	bool godmode;

	// used on world map only
	bool hideplayer;
	bool mounted;

	short treshold;		// This is used for analog devices like joysticks
	signed int pinertia_y;

	unsigned long mapplayx;
	signed int mapplayy;

	bool pfalling;
	unsigned char plastfalling,pfallspeed;

	unsigned char pwalking,playspeed;
	unsigned char pslowingdown;
	unsigned char pwalkframe,pwalkframea;
	int pwalkanimtimer;
	unsigned char pwalkincreasetimer, pfriction_timer_y;
	unsigned char playpushed_decreasetimer;

	jumpstate pjumping;
	unsigned char pjumptime, pjumpupspeed_decrease, pjumpdir;
	unsigned char pjumpframe, pjumpanimtimer;
	int pjumpupspeed;
	unsigned char pjumpnormaltime, pjumpupdecreaserate;
	bool pjustjumped;
	bool pjustfell;
	unsigned char pjumpfloattimer;
	size_t ankhtime;

	direction_t pdir,pshowdir,lastpdir;

	char pfireframetimer;
	bool inhibitwalking;

	int ctrltimer, alttimer;
	char keyprocstate;
	char wm_lastenterstate;

	char pdie, pdieframe, pdietimer;
	int pdietillfly;
	signed int pdie_xvect;
	bool psliding;
	bool psemisliding;
	bool ppogostick;
	bool pogofirsttime;
	int pfrozentime,pfrozenframe,pfrozenanimtimer;
	bool pfiring, plastfire;

	char playcontrol[PA_MAX_ACTIONS];

	bool object_chosen;
	unsigned char dpadcount, dpadlastcount;

	stInventory inventory;

	bool m_cheats_enabled;
	bool *mp_levels_completed;
	bool m_showStatusScreen;
	char level_done;
	bool beingteleported;

	std::string hintstring;
	bool hintused;

	stOption *mp_option;

private:
	// defined under CPlayerItems.cpp
	bool getGoodie(int px, int py);
	void procGoodie(int tile, int mpx, int mpy);
	void riseBonus(int spr, int x, int y);
	void getBonuspoints(int numpts, int mpx, int mpy);
	void incScore(int numpts);
	void openDoor(int doortile, int doorsprite, int mpx, int mpy);
	void giveAnkh();

	CStatusScreen *mp_StatusScr;

	bool lastpogo;
 	bool bumped;

	// Level control specific functions, especially for exit
	int exitXpos;
	int level_done_timer;

	level_triggers m_Level_Trigger;

	static size_t m_playerID;

};

#endif /* CPLAYER_H_ */
