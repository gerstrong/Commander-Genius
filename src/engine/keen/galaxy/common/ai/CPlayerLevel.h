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
    int jumpTimer = 0;
    int poleGrabTime = 0;
    bool jumpIsPressed = false;
    bool jumpWasPressed = false;
    bool pogoIsPressed = false;
    bool pogoWasPressed = false;
};


class CPlayerLevel : public CPlayerBase
{
public:
    CPlayerLevel(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y,
                 std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtrs, direction_t facedir,
                 CInventory &l_Inventory,
                 const size_t offset,
                 const int playerIdx,
                 const int spriteTableIdx);


    virtual void serialize(GsKit::ptree &) override;

    virtual void deserialize(GsKit::ptree &) override;


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
    void processInput() override;


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
     * @brief checkConveyorBelt Checks if Keen is standing or walking on a conveyor belt and returns the transport direction
     * @return 1 for right, -1 for left, 0 for nothing
     */
    int checkConveyorBelt();


    /**
         * Input handles
         */

    /**
         * \description This is the main process cycle
         */
    void process() override;


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
     * @brief evalVertPogoInertia   Calculates the inertia required for a pogo jump. yinertia is set in that process
     * @return
     */
    int evalVertPogoInertia();

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

    // Make Keen get stunned. This one is very special,
    // as it doesn't belong to the current actions of Keen and only happen in Keen 6
    bool stun();

    void processStunned();



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
    void processFalling() override;



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
         * This function will be processed while Keen is looking up
         */
    void processLookingUp();

    /**
     * @brief verifySwitches Is Player trying to flip a switch?
     * @return
     */
    bool verifySwitches();

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

    void processPoleSlidingDown();


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
    void toggleBridge(const Uint32 newX, const Uint32 newY);
    void PressBridgeSwitch(const Uint32 lx, const Uint32 ly);

    void PressPlatformSwitch(const Uint32 lx, const Uint32 ly);

    /**
     * @brief openDoorsTile     Replace the tiles which open a side door, usually connected with a slot where to insert a gem
     */
    void openDoorsTile();

    void exchangeZapper(const int mapx, const int mapy, const int offset, const int stopZapperTile);
    void disableZapper(const Uint32 lx, const Uint32 ly);
    void enableZapper(const Uint32 lx, const Uint32 ly);

    bool verifyAndToggleZapper(const int lx, const int ly);
    bool verifyAndToggleBridge(const int lx, const int ly);

    /**
     * @brief TurnGiantSwitchOff    Giant switches only appear in Keen 6
     * @param x
     * @param y
     */
    void TurnGiantSwitchOff(const int x, const int y);
    void TurnGiantSwitchOn(const int x, const int y);

    // Checks if Keen is using the pogo. Skypest needs to know that,
    // because it might get squashed.
    bool isPogoing()
    {
        return (mp_processState == (void (CPlayerBase::*)()) &CPlayerLevel::processPogo );
    }


    int checkSolidU(int x1, int x2, int y1, const bool push_mode=false ) override;
    int checkSolidD( int x1, int x2, int y2, const bool push_mode=false ) override;


    bool mPlacingGem = false;

private:

    /** \brief Special code when Keen moving down... */
    void processMoveBitDown() override;

    void getTouchedBy(CSpriteObject &theObject) override;

    KeenState mActionState;

    int m_jumpheight = 0;

    bool m_pogotoggle = false;

    bool m_EnterDoorAttempt = false;
    int m_hangtime = 0;
    int mPoleGrabTime = 0;
    int mClimbSpeedX;
    int mExitDoorTimer = 0;
    int mStunTimer = 0;

    std::vector< std::shared_ptr<CGalaxySpriteObject> > &mObjectPtrs;
};

}

#endif /* CPLAYERLEVEL_H_ */
