/*
 * CPlayerLevel.h
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#ifndef CPLAYERLEVEL_H_
#define CPLAYERLEVEL_H_

#include "common/CObject.h"
#include "common/Playerdefines.h"
#include "common/Cheat.h"
#include "engine/CEvent.h"
#include "engine/CCamera.h"
#include "engine/galaxy/CInventory.h"

namespace galaxy {

#define A_KEEN_STAND			0
#define A_KEEN_BORED			1
#define A_KEEN_QUESTION			2
#define A_KEEN_MOON				8
#define A_KEEN_BOOK_OPEN		11
#define A_KEEN_BOOK_READ		15
#define A_KEEN_BOOK_CLOSE		18
#define A_KEEN_LOOKUP			21
#define A_KEEN_LOOKDOWN			23
#define A_KEEN_ACTION_1			26
#define A_KEEN_ACTION_2			27
#define A_KEEN_ACTION_3			28
#define A_KEEN_DIE				29
#define A_KEEN_SHOOT			33
#define A_KEEN_ACTION_4			37
#define A_KEEN_SLIDE			40
#define A_KEEN_ENTER_DOOR		41
#define A_KEEN_POLE				46
#define A_KEEN_POLE_CLIMB		47
#define A_KEEN_POLE_SLIDE		50
#define A_KEEN_POLE_SHOOT		54
#define A_KEEN_POLE_SHOOTUP		56
#define A_KEEN_POLE_SHOOTDOWN	58
#define A_KEEN_RUN				60
#define A_KEEN_POGO				64
#define A_KEEN_JUMP				67
#define A_KEEN_FALL				69
#define A_KEEN_JUMP_SHOOT		71
#define A_KEEN_JUMP_SHOOTUP		74
#define A_KEEN_JUMP_SHOOTDOWN	77
#define A_KEEN_HANG				80
#define A_KEEN_CLIMB			82

class CPlayerLevel : public CObject {
public:
	CPlayerLevel(CMap *pmap, Uint32 x, Uint32 y,
			std::vector<CObject*>& ObjectPtrs, direction_t facedir,
			CInventory &l_Inventory, stCheat &Cheatmode);

	// Checks if player can fall through a tile
	bool canFallThroughTile();

	/**
	 * \brief The Player will get 1 UP when that function is launched
	 */
	void getAnotherLife(const int &lc_x, const int &lc_y);



	/**
	 * \description This is the main process cycle
	 */
	void process();




	/**
	 * \description Simple process called while Keen is standing
	 */
	void processStanding();



	/**
	 * \description Simple process called while Keen is running
	 */
	void processRunning();



	/**
	 * \description This will verify whether keen has to fall or not.
	 * 				This function must be called when he might be walking
	 * 				because it also checks the lower sloped tiles cases
	 *
	 * \todo 		This function should also be called by foes in some cases
	 */
	bool verifyForFalling();


	/**
	 * \description Simple function which just process moving in x direction
	 * 				of the player. It is called by other functions like
	 * 				processJumping or processFalling. processRunning not,
	 * 				because it has to stop running when players gets blocked
	 * 				to left or right.
	 */
	void processMovingHorizontal();


	/**
	 * \description Simple process called while Keen is falling down
	 */
	void processFalling();




	/*-----------------------------------------------------*/
	/* Some of the following function will be removed soon */
	/*-----------------------------------------------------*/

	void processInput();
	void processFiring();
	void processJumping();
	void processPogo();
	void processLooking();
	int processPressUp();
	void processExiting();
	void processEnterDoor();
	void processLevelMiscFlagsCheck();

	void PressBridgeSwitch(const Uint32 lx, const Uint32 ly);
	void PressPlatformSwitch(const Uint32 lx, const Uint32 ly);
	void openDoorsTile();
	void processPlaceGem();

	void processDying();
	void kill();

	CInventory &m_Inventory;

	bool m_jumpdownfromobject;

private:
	std::vector<CObject*>& m_ObjectPtrs;

	char m_playcontrol[PA_MAX_ACTIONS];

	bool m_pfiring;
	bool m_ptogglingswitch;
	int m_jumpheight;
	bool m_cliff_hanging;
	bool m_inair;

	int m_timer;
	bool m_pogotoggle;
	CCamera m_camera;

	bool m_dying;
	stCheat& m_Cheatmode;

	void (CPlayerLevel::*mp_processState)();
};

}

#endif /* CPLAYERLEVEL_H_ */
