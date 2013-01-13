/*
 * CPlayerLevel.h
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#ifndef CPLAYERLEVEL_H_
#define CPLAYERLEVEL_H_

#include "CPlayerBase.h"

namespace galaxy {



struct KeenState
{
	KeenState() :
		jumpTimer(0), poleGrabTime(0),
		jumpIsPressed(false), jumpWasPressed(false),
		pogoIsPressed(false), pogoWasPressed(false)
		{}

	int jumpTimer;
	int poleGrabTime;
	bool jumpIsPressed;
	bool jumpWasPressed;
	bool pogoIsPressed;
	bool pogoWasPressed;
};


class CPlayerLevel : public CPlayerBase
{
public:
	CPlayerLevel(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y,
			std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtrs, direction_t facedir,
			CInventory &l_Inventory, stCheat &Cheatmode,
			const size_t offset);


	void prepareToShoot();
	/**
	 * \brief	handles the input when keen standing on ground.
	 */
	void handleInputOnGround();


	/**
	 * \description Read the Input of the Player and sets the variables accordingly
	 * 				This function is overloaded. Most of the process is done by the
	 * 				CPlayerBase class.
	 */
	void processInput();


	// Checks if player can fall through a tile
	bool canFallThroughTile();

	/**
	 *  \brief This will center Keens view after he looked up or down.
	 */
	void centerView();

	bool onPole()
	{
	  return getActionNumber(A_KEEN_POLE) ||
		getActionNumber(A_KEEN_POLE_CLIMB) ||
		getActionNumber(A_KEEN_POLE_SLIDE) ||
		getActionNumber(A_KEEN_POLE_SHOOT) ||
		getActionNumber(A_KEEN_POLE_SHOOTUP) ||
		getActionNumber(A_KEEN_POLE_SHOOTDOWN);	
	}
	
	void push( CGalaxySpriteObject &theObject );

	/**
	 * Input handles
	 */

	/**
	 * \description This is the main process cycle
	 */
	void process();


	/**
	 * \description Simple process to test if Keen is a moon and can pull off his pants.
	 */	
	bool moonTiledetected();

	/**
	 * \description Simple process called while Keen is standing
	 */
	void processStanding();

	/**
	 * \description Simple process called while Keen is reading the book
	 */
	void processReadingBook();
	
	/**
	 * \description Simple process called while Keen is pulling off the pants
	 */	
	void processPants();


	/**
	 * \description Verify if Keen can stick to the pole and climb on it!
	 */
	bool verifyforPole();



	/**
	 * \description This will make keen stand in a pose with the gun and shoot if he has bullets
	 */
	void processShootWhileStanding();


	/**
	 * \description Makes Keen stand. In Keen 5 he stands different on platforms
	 */
	void makeHimStand();


	/**
	 * \description Simple process called while Keen is running
	 */
	void processRunning();



	/**
	 * \description verifies whether there is a cliff and Keen can hang on it.
	 * 				If that the case, the function places him properly
	 * 				and set the process to processCliffHanging
	 * \return 		if true, the cliff was detected and Keen will go into cliff
	 * 				hanging mode.
	 */
	bool checkandtriggerforCliffHanging();


	/*
	 * \description Keen hangs here on a cliff and it might decide
	 * 				whether to fall or climb up
	 */
	void processCliffHanging();


	/*
	 * \description This will make Keen climb the cliff up and stand
	 * 				on the ground. After that it returns to stand action
	 */
	void processCliffClimbingUp();

	void processCliffClimbingOntoFloor();



	/**
	 * \description Simple function which just process moving in x direction
	 * 				of the player. It is called by other functions like
	 * 				processJumping or processFalling. processRunning not,
	 * 				because it has to stop running when players gets blocked
	 * 				to left or right.
	 */
	void processMovingHorizontal();



	/**
	 * \brief Checks if Keen must fall.
	 */
	void verifyFalling();



	/**
	 * \description Simple process called while Keen is falling down
	 */
	void processFalling();



	/**
	 * \description Firing Routine for Keen
	 */
	void processFiring();


	/**
	 * This function is processed by falling and jumping. It just manages the Shoot in air routines
	 */
	void shootInAir();

	void verifyJumpAndFall();

	void processJumping();

	void processPogoCommon();
	void processPogoBounce();
	void processPogo();

	void processLooking();


	/**
	 * \brief		This function will try to spawn a shot. If Keen has bullets it will spawn
	 * 				otherwise, the empty clicking sound is heard
	 * \param	pos	Coordinates where the shot should be spawned
	 */
	void tryToShoot( const VectorD2<int> &pos, const int xDir, const int yDir );



	/**
	 * This function will be processed while Keen is looking up
	 */
	void processLookingUp();

	/**
	 * This function will process everything that happens when player presses up
	 */
	void processPressUp();

	/**
	 * This function will ensure that Keen keeps ducking
	 */
	void processLookingDown();

	/**
	 * This function will do the cycle when Keen is on the pole
	 */

	void performPoleHandleInput();

	void processPoleClimbingSit();

	void processPoleClimbingUp();

	void processPoleClimbingDown();

	void processExiting();

	/**
	 * This function will be process will keen is pressing a switch. Placing gem uses
	 * a different process called processPlaceGem()
	 * He just slides to that item or tile
	 */
	void processSliding();

	/**
	 * This function will process the going into a door
	 */
	void processEnterDoor();


	/**
	 * This function will open/close bridges in Keen Galaxy
	 * \param lx CSFed Coordinate where the switch has been triggered
	 */
	void PressBridgeSwitch(const Uint32 lx, const Uint32 ly);

	void PressPlatformSwitch(const Uint32 lx, const Uint32 ly);
	void openDoorsTile();

	// Checks if Keen is using the pogo. Skypest needs to know that,
	// because it might get squashed.
	bool isPogoing()
	{
		return (mp_processState == (void (CPlayerBase::*)()) &CPlayerLevel::processPogo );
	}



	bool m_jumpdownfromobject;
	bool mPlacingGem;
	bool mExitTouched;

private:

	/** \brief Special code when Keen moving down... */
	void processMoveBitDown();

	KeenState state;

	bool m_ptogglingswitch;
	int m_jumpheight;

	bool m_pogotoggle;

	int m_fire_recharge_time;
	bool m_EnterDoorAttempt;
	int m_hangtime;
	int mPoleGrabTime;
	int mClimbSpeedX;
	int mExitDoorTimer;
	
	std::vector< std::shared_ptr<CGalaxySpriteObject> > &mObjectPtrs;
};

}

#endif /* CPLAYERLEVEL_H_ */
