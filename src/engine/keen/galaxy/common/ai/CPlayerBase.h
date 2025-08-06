/*
 * CPlayerBase.h
 *
 *  Created on: 19.06.2011
 *      Author: gerstrong
 *
 *  This is the base class for the Player. This class is very new and I thought of it for
 *  the following reason. We have Keen on the Map in level and when diving.
 *  Those AIs are different of course, but still many things in common like dying
 *  (See the puzzle Keen 5 mod versions by Mink) and collecting items.
 *  It will not only reduce the code of some repeating routines, but also
 *  provide better overview over the whole thing.
 */

#ifndef CPLAYERBASE_H_
#define CPLAYERBASE_H_

#include "../CGalaxySpriteObject.h"
#include "../CInventory.h"

#include "engine/core/Playerdefines.h"
#include "engine/core/CCamera.h"

#include <base/audio/music/CMusic.h>
#include <base/GsEvent.h>
#include <graphics/CDimDark.h>

#include <array>

struct EventEnterLevel : CEvent {
  EventEnterLevel(const int sprVar,
                  const uint16_t d) : data(d),
    mSprVar(sprVar) {}

  uint16_t data = 0;
  int mSprVar = 0;

};




struct EventExitLevel : CEvent {
  EventExitLevel(const uint16_t l, const bool s, const bool t, const bool lWho) :
    levelObject(l), sucess(s), teleport(t), who(lWho) {}

  const uint16_t levelObject;
  const bool sucess;
  const bool teleport;
  const int who;
  bool playSound = false;

};

struct EventMoveAllPlayersBut : CEvent {
    EventMoveAllPlayersBut(const int idx, const GsVec2D<int> &target) :
        mException(idx), mTarget(target) {}

    const int mException;
    const GsVec2D<int> mTarget;
};

struct EventExitLevelWithFoot : CEvent {
  EventExitLevelWithFoot(const uint16_t l, const int lWho) : levelObject(l), who(lWho) {}

  const uint16_t levelObject;
  const int who;

};


struct EventDieKeenPlayer  : CEvent
{
  EventDieKeenPlayer(const int id, const int lo, const std::string &ln) :
    playerID(id),
    levelObj(lo), levelName(ln){}

  const int playerID;
  const int levelObj;
  const std::string levelName;

};


struct RevivePlayer : CEvent
{
    RevivePlayer(const int playerIdx) :
        mPlayerIdx(playerIdx) {}

    const int mPlayerIdx;
};


namespace galaxy
{


  enum PLAYER_ACTIONS
  {
    A_KEEN_STAND = 0,
    A_KEEN_ON_PLAT = 1,
    A_KEEN_BORED = 2,
    A_KEEN_QUESTION = 3,
    A_KEEN_MOON = 4,
    A_KEEN_BOOK_OPEN = 5,
    A_KEEN_BOOK_READ = 6,
    A_KEEN_BOOK_CLOSE = 7,
    A_KEEN_LOOKUP = 8,
    A_KEEN_LOOKDOWN,
    A_KEEN_ACTION_1,
    A_KEEN_ACTION_2,
    A_KEEN_ACTION_3,
    A_KEEN_DIE,
    A_KEEN_DIE_ALT,
    A_KEEN_SHOOT,
    A_KEEN_SHOOT_UP,
    A_KEEN_ACTION_4,
    A_KEEN_SLIDE,
    A_KEEN_ENTER_DOOR,
    A_KEEN_POLE,
    A_KEEN_POLE_CLIMB,
    A_KEEN_POLE_SLIDE,
    A_KEEN_POLE_SHOOT,
    A_KEEN_POLE_SHOOTUP,
    A_KEEN_POLE_SHOOTDOWN,
    A_KEEN_RUN,
    A_KEEN_POGO_START,
    A_KEEN_POGO_UP,
    A_KEEN_POGO_HIGH,
    A_KEEN_JUMP,
    A_KEEN_JUMP_DOWN,
    A_KEEN_FALL,
    A_KEEN_JUMP_SHOOT,
    A_KEEN_JUMP_SHOOTUP,
    A_KEEN_JUMP_SHOOTDOWN,
    A_KEEN_HANG,
    A_KEEN_CLIMB,
    A_KEEN_RIDING_ON_FOOT,
    A_KEEN_STUNNED,
    NUM_KEEN_ACTIONS
  };


  class CPlayerBase : public CGalaxySpriteObject
  {
  public:
    /**
         * \description Basic Constructor. An Object always need the pointer to the Map of the level,
         * 				Coordinates as also the type of the Object
         */
    CPlayerBase(std::shared_ptr<CMap> pMap,
                const Uint16 foeID,
                Uint32 x,
                Uint32 y,
                direction_t facedir,
                CInventory &l_Inventory,
                const int playerID,
                const int spriteVar);


    /**
     * Pushes the players by the given object Reference
     */
    void push( CGalaxySpriteObject &theObject );

    /**
     * Overloaded getActionStatus which uses the episodes' map
     */
    bool getActionStatus(int16_t ActionNumber) override;

    /**
     * returns the Sprite ID from the given action
     */
    int getSpriteIDFromAction(const int16_t ActionNumber);

    /**
     * Overloaded setActionForce which uses the action for the player
     */
    void setActionForce(const size_t ActionNumber) override;

    /**
     * \brief The Player will get 1 UP when that function is launched
     */
    void getAnotherLife(const int lc_x, const int lc_y, const bool display, const bool alt);

    virtual void pumpEvent(const std::shared_ptr<CEvent> &evPtr) override;

    /**
     * \description Camera of the player gets performed here...
     */
    void processCamera();

    /**
     * @brief Player can always be visible and active, even if he is not seen.
     * @return
     */
    bool calcVisibility() override;

    void moveOtherPlayersTo(const GsVec2D<int> &target);

    /**
         * \description Read the Input of the Player and sets the variables accordingly
         */
    virtual void processInput();


    void processLevelMiscFlagsCheck();

    /**
         * \brief		This function will try to spawn a shot. If Keen has bullets it will spawn
         * 				otherwise, the empty clicking sound is heard
         * \param	pos	Coordinates where the shot should be spawned
         */
    void tryToShoot( const GsVec2D<int> &pos, const int xDir, const int yDir );


    /**
         * Produces the walking sound. To produce that sound correctly, it uses a special timer.
         * That's the reason we need that aiding function
         */
    void playWalkSound();


    bool processActionRoutine() override;

    /**
         * Make keen move with x/y speed move towards a target which is held in the variable "mTarget"
         * it only moves towards, this routines is exec'd several times in order achieve a smooth transition.
         */
    void guideToTarget(const GsVec2D<int> &speed);


    /**
     * Same here but with swimming. If moving set to true player is moving keen actually
         */
    void playSwimSound(const bool moving);

    void respawnImportantItem(const int itemId);

    bool touchedBottomOfMap();

    void processDead();
    virtual void processDying();

    void processExiting();


    /**
     * @brief kill          Kill Keen.
     * @param force         If true Keen may die even in god mode
     * @param noDieProcess  Do not show any dying animation, kill him directly!
     *                      (v.g. got eaten by Dopefish)
     */
    virtual void kill(const bool force = false,
                      const bool noDieProcess = false) override;

    /**
     * Happens when usually the Dopefish eats Keen
     */
    void getEaten();

    /**
     * Process the cycle when keen is getting eaten
     */
    void processGetEaten();

    CInventory &m_Inventory;

    CCamera m_camera;

    bool mDying = false;
    bool mExitTouched = false;

  protected:

    bool checkMapBoundaryL(const int x1) override;
    bool checkMapBoundaryR(const int x2) override;
    bool checkMapBoundaryU(const int y1) override;

    const int mPlayerNum;
    int mPlayerCtrlNum; // For the control input

    std::array<Sint16, PA_MAX_ACTIONS> mPlaycontrol;
    int m_timer;



    void (CPlayerBase::*mp_processState)() = nullptr;
    unsigned char m_walktimer;
    std::map< size_t, void (CPlayerBase::*)() > mActionMap;

    // Assignment for the same player for all the episodes
    static const int mEpisodeActionNumMap[3][NUM_KEEN_ACTIONS];

    GsVec2D<int> mTarget;

    int m_fire_recharge_time = 0;
    bool mReleasedShot = false;
};

}

#endif /* CPLAYERBASE_H_ */
