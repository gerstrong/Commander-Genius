/*
 * CPlayerBase.cpp
 *
 *  Created on: 19.06.2011
 *      Author: gerstrong
 */

#include "CPlayerBase.h"
#include "CItemEffect.h"
#include "CSpriteItem.h"
#include "CBullet.h"
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/audio/Audio.h>
#include <engine/core/CBehaviorEngine.h>
#include <fileio/KeenFiles.h>
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"


namespace galaxy {

const int CPlayerBase::mEpisodeActionNumMap[3][NUM_KEEN_ACTIONS] =
{
        // Episode 4
        {
                0,  // A_KEEN_STAND
                1,  // A_KEEN_ON_PLAT
                1,  // A_KEEN_BORED
                2,  // A_KEEN_QUESTION
                8,  // A_KEEN_MOON
                11, // A_KEEN_BOOK_OPEN
                15, // A_KEEN_BOOK_READ
                18, // A_KEEN_BOOK_CLOSE
                21, // A_KEEN_LOOKUP
                23, // A_KEEN_LOOKDOWN
                26, // A_KEEN_ACTION_1
                27, // A_KEEN_ACTION_2
                28, // A_KEEN_ACTION_3
                29, // A_KEEN_DIE
                30, // A_KEEN_DIE_ALT
                33, // A_KEEN_SHOOT
                35, // A_KEEN_SHOOT_UP
                37, // A_KEEN_ACTION_4
                40, // A_KEEN_SLIDE
                41, // A_KEEN_ENTER_DOOR
                46, // A_KEEN_POLE
                47, // A_KEEN_POLE_CLIMB
                50, // A_KEEN_POLE_SLIDE
                54, // A_KEEN_POLE_SHOOT
                56, // A_KEEN_POLE_SHOOTUP
                58, // A_KEEN_POLE_SHOOTDOWN
                60, // A_KEEN_RUN
                64, // A_KEEN_POGO_START
                65, // A_KEEN_POGO_UP
                66, // A_KEEN_POGO_HIGH
                67, // A_KEEN_JUMP
                68, // A_KEEN_JUMP_DOWN
                69, // A_KEEN_FALL
                71, // A_KEEN_JUMP_SHOOT
                74, // A_KEEN_JUMP_SHOOTUP
                77, // A_KEEN_JUMP_SHOOTDOWN
                80, // A_KEEN_HANG
                82, // A_KEEN_CLIMB
                94  // A_KEEN_RIDING_ON_FOOT
        },

        // Episode 5
        {
                0,  // A_KEEN_STAND
                1,  // A_KEEN_ON_PLAT
                2,  // A_KEEN_BORED
                3,  // A_KEEN_QUESTION
                8,  // A_KEEN_MOON
                11, // A_KEEN_BOOK_OPEN
                15, // A_KEEN_BOOK_READ
                18, // A_KEEN_BOOK_CLOSE
                19, // A_KEEN_LOOKUP
                21, // A_KEEN_LOOKDOWN
                26, // A_KEEN_ACTION_1
                27, // A_KEEN_ACTION_2
                28, // A_KEEN_ACTION_3
                27, // A_KEEN_DIE
                28, // A_KEEN_DIE_ALT
                29, // A_KEEN_SHOOT
                31, // A_KEEN_SHOOT_UP
                37, // A_KEEN_ACTION_4
                36, // A_KEEN_SLIDE
                39, // A_KEEN_ENTER_DOOR
                45, // A_KEEN_POLE
                46, // A_KEEN_POLE_CLIMB
                49, // A_KEEN_POLE_SLIDE
                53, // A_KEEN_POLE_SHOOT
                55, // A_KEEN_POLE_SHOOTUP
                57, // A_KEEN_POLE_SHOOTDOWN
                59, // A_KEEN_RUN
                63, // A_KEEN_POGO_START
                64, // A_KEEN_POGO_UP
                65, // A_KEEN_POGO_HIGH
                66, // A_KEEN_JUMP
                67, // A_KEEN_JUMP_DOWN
                68, // A_KEEN_FALL
                70, // A_KEEN_JUMP_SHOOT
                73, // A_KEEN_JUMP_SHOOTUP
                76, // A_KEEN_JUMP_SHOOTDOWN
                79, // A_KEEN_HANG
                81  // A_KEEN_CLIMB
        },

        // Episode 6
        {
                0,  // A_KEEN_STAND
                1,  // A_KEEN_ON_PLAT
                1,  // A_KEEN_BORED
                2,  // A_KEEN_QUESTION
                8,  // A_KEEN_MOON
                8,  // A_KEEN_BOOK_OPEN
                15, // A_KEEN_BOOK_READ
                16, // A_KEEN_BOOK_CLOSE
                18, // A_KEEN_LOOKUP
                20, // A_KEEN_LOOKDOWN
                23, // A_KEEN_ACTION_1
                24, // A_KEEN_ACTION_2
                25, // A_KEEN_ACTION_3
                26, // A_KEEN_DIE
                27, // A_KEEN_DIE_ALT
                28, // A_KEEN_SHOOT
                30, // A_KEEN_SHOOT_UP
                32, // A_KEEN_SLIDE_INTO_DOOR
                34, // A_KEEN_SLIDE
                36, // A_KEEN_ENTER_DOOR
                41, // A_KEEN_POLE
                42, // A_KEEN_POLE_CLIMB
                45, // A_KEEN_POLE_SLIDE
                49, // A_KEEN_POLE_SHOOT
                51, // A_KEEN_POLE_SHOOTUP
                53, // A_KEEN_POLE_SHOOTDOWN
                55, // A_KEEN_RUN
                59, // A_KEEN_POGO_START
                60, // A_KEEN_POGO_UP
                61, // A_KEEN_POGO_HIGH
                62, // A_KEEN_JUMP
                63, // A_KEEN_JUMP_DOWN
                64, // A_KEEN_FALL
                68, // A_KEEN_JUMP_SHOOT
                71, // A_KEEN_JUMP_SHOOTUP
                74, // A_KEEN_JUMP_SHOOTDOWN
                75, // A_KEEN_HANG
                77, // A_KEEN_CLIMB
                0,  // A_KEEN_RIDING_ON_FOOT (Unused)
                0   // A_KEEN_STUNNED (Unused)
        }

        // Keen Dreams
        //{}
};

CPlayerBase::
CPlayerBase(std::shared_ptr<CMap> pMap,
            const Uint16 foeID,
            Uint32 x,
            Uint32 y,
            direction_t facedir,
            CInventory &l_Inventory,
            const int playerIdx,
            const int spriteVar) :
    CGalaxySpriteObject(pMap, foeID, x, y, spriteVar),
    m_Inventory(l_Inventory),
    m_camera(pMap,x,y,this),
    mPlayerNum(playerIdx),
    mPlayerCtrlNum(playerIdx)
{
    mPlayerIdx = playerIdx;
    mActionMap[A_KEEN_DIE] = &CPlayerBase::processDying;
    mActionMap[A_KEEN_DIE_ALT] = &CPlayerBase::processDying;

    m_walktimer = 0;
    m_timer = 0;
    mDying = false;
    xDirection = facedir;

    mPlaycontrol.fill(0);
    m_camera.setPosition(m_Pos);

    mpMap->calcVisibleArea();

    mpMap->refreshVisibleArea();

    const auto fullFName = JoinPaths(gKeenFiles.gameDir, "player.lua");
    mLua.loadFile(fullFName);
}





// Processes the exiting of the player. Here all default cases are held
void CPlayerBase::processExiting()
{
    Uint32 x = getXMidPos();
    Uint32 y = getYMidPos();
    if( (((mpMap->m_width-2)<<CSF) < x || (2<<CSF) > x)  || ((2<<CSF) > y) )
      // If player leaves boundary, except for the lower part, which death
    {
        gMusicPlayer.stop();

        gEffectController.setupEffect(new CDimDark(8));

        auto evExit = new EventExitLevel(mpMap->getLevel(),
                                         true, false, mPlayerIdx);
        evExit->playSound = true;
        gEventManager.add( evExit );
        m_Inventory.Item.m_gem.clear();
        mExitTouched = true;
    }
}



void CPlayerBase::getAnotherLife(const int lc_x, const int lc_y, const bool display, const bool alt)
{
    m_Inventory.Item.m_lifes++;
    playSound( GameSound::EXTRA_LIFE );
    if(display)
    {
        const int ep = gBehaviorEngine.getEpisode();
        const int id = alt ? 12 : 10;
        CItemEffect *lifeUp = new CItemEffect(mpMap, 0, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[ep-4][id], FADEOUT);
        gEventManager.add( new EventSpawnObject( lifeUp ) );
    }
}



void CPlayerBase::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    CGalaxySpriteObject::pumpEvent(evPtr);
    m_camera.pumpEvent(evPtr);
}


void CPlayerBase::processCamera()
{
    auto &relVisBlend = gVideoDriver.mpVideoEngine->mRelativeBlendVisGameArea;

    relVisBlend = 0;

    const auto &optModern = gBehaviorEngine.mOptions[GameOption::MODERN];

    if (optModern.value)
    {
        relVisBlend.pos = {scrx-16, scry-32};
        relVisBlend.dim = {48, 64};
    }

    m_camera.process();
    m_camera.processEvents();
}

bool CPlayerBase::calcVisibility()
{
    return true;
}


void CPlayerBase::moveOtherPlayersTo(const GsVec2D<int> &target)
{
    gEventManager.add( new EventMoveAllPlayersBut(this->getPlayerIdx(), target) );
}


void CPlayerBase::processInput()
{
    // If player has the camlead, he can also reassign the input
    // by pressing Shift+Number
    if(mPlayerNum == m_camera.getLead() && gBehaviorEngine.numPlayers() > 1)
    {
        if(gInput.getHoldedKey(KSHIFT))
        {
            if(gInput.getPressedKey(KNUM1)) mPlayerCtrlNum = 0;
            if(gInput.getPressedKey(KNUM2)) mPlayerCtrlNum = 1;
            if(gInput.getPressedKey(KNUM3)) mPlayerCtrlNum = 2;
            if(gInput.getPressedKey(KNUM4)) mPlayerCtrlNum = 3;
        }
    }


    // Entry for every player
    mPlaycontrol[PA_X] = 0;
    mPlaycontrol[PA_Y] = 0;

    const auto isAnalog = gInput.isAnalog(mPlayerCtrlNum);

    const int maxMotionVal = 100;

    auto procAnalogMovements = [&](const int player,
                                   const int command,
                                   const int maxVal) -> int
    {
        if(!isAnalog || !gInput.isJoystickAssgmnt(player, command) )
            return maxVal;

        return gInput.getJoyValue(player, command, (maxVal<0) );
    };

    if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_LEFT))
    {
        const int newval = procAnalogMovements(mPlayerCtrlNum,
                                               IC_LEFT, -maxMotionVal);
        mPlaycontrol[PA_X] += newval;
    }
    else if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_RIGHT))
    {
        const int newval = procAnalogMovements(mPlayerCtrlNum,
                                               IC_RIGHT, maxMotionVal);
        mPlaycontrol[PA_X] += newval;
    }

    if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_DOWN))
    {
        const int newval = procAnalogMovements(mPlayerCtrlNum,
                                               IC_DOWN, maxMotionVal);
        mPlaycontrol[PA_Y] += newval;
    }

    else if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_UP))
    {
        const int newval = procAnalogMovements(mPlayerCtrlNum,
                                               IC_UP, -maxMotionVal);
        mPlaycontrol[PA_Y] += newval;
    }

    if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_UPPERLEFT))
    {
        mPlaycontrol[PA_X] -= maxMotionVal;
        mPlaycontrol[PA_Y] -= maxMotionVal;
    }
    else if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_UPPERRIGHT))
    {
        mPlaycontrol[PA_X] += maxMotionVal;
        mPlaycontrol[PA_Y] -= maxMotionVal;
    }
    else if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_LOWERLEFT))
    {
        mPlaycontrol[PA_X] -= maxMotionVal;
        mPlaycontrol[PA_Y] += maxMotionVal;
    }
    else if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_LOWERRIGHT))
    {
        mPlaycontrol[PA_X] += maxMotionVal;
        mPlaycontrol[PA_Y] += maxMotionVal;
    }

    mPlaycontrol[PA_JUMP] =
            gInput.getHoldedCommand(mPlayerCtrlNum, IC_JUMP) ? 1 : 0;
    mPlaycontrol[PA_POGO] =
            gInput.getHoldedCommand(mPlayerCtrlNum, IC_POGO) ? 1 : 0;

    mPlaycontrol[PA_RUN]  =
            gInput.getHoldedCommand(mPlayerCtrlNum, IC_RUN)  ? 1 : 0;

    // The possibility to charge jumps.
    // This is mainly used for the pogo. it is limited to 50
    if( mPlaycontrol[PA_JUMP] > 50) mPlaycontrol[PA_JUMP] = 50;

    // Two button firing process
    if(gInput.getTwoButtonFiring(mPlayerCtrlNum))
    {
        if(mPlaycontrol[PA_JUMP] && mPlaycontrol[PA_POGO])
        {
            mPlaycontrol[PA_FIRE] = 1;
            mPlaycontrol[PA_JUMP] = 0;
            mPlaycontrol[PA_POGO] = 0;
        }
        else if(mPlaycontrol[PA_FIRE])
        {
            mPlaycontrol[PA_FIRE] = 0;
            mPlaycontrol[PA_JUMP] = 0;
            mPlaycontrol[PA_POGO] = 0;
            gInput.flushCommand(IC_JUMP);
            gInput.flushCommand(IC_FIRE);
            gInput.flushCommand(IC_RUN);
            gInput.flushCommand(IC_POGO);
        }

    }
    else
    {
        mPlaycontrol[PA_FIRE] =
                gInput.getHoldedCommand(mPlayerCtrlNum, IC_FIRE) ? 1 : 0;
    }


    //// Special Keyboard Input
    // Quicksave
    if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_QUICKSAVE) )
    {
        gSaveGameController.prepareSaveGameQuick();
        galaxy::showMsg(0, "Quicksaving ...", nullptr, false, "SaveMsg.msg",
                        galaxy::CMessageBoxGalaxy::Alignment::UPPERRIGHT,
                        1000, false);
    }
    if(gInput.getHoldedCommand(mPlayerCtrlNum, IC_QUICKLOAD) )
    {
        if(gSaveGameController.hasQuickloadGame())
        {
            galaxy::showMsg(0, "Quickloading ...", nullptr, false, "LoadMsg.msg",
                            galaxy::CMessageBoxGalaxy::Alignment::UPPERRIGHT,
                            1000, false);
            gSaveGameController.prepareLoadGameQuick();
        }
    }
}






// Process the touching of certain tile, like items and hazards...
void CPlayerBase::processLevelMiscFlagsCheck()
{
    // Item which are taken must go into a data structure
    // animation should also be triggered

    stItemGalaxy &m_Item = m_Inventory.Item;

    int l_x = getXLeftPos();
    int l_y = getYUpPos();
    int l_w = getXRightPos() - getXLeftPos();
    int l_h = getYDownPos() - getYUpPos();


    // Deadly hazards! Here Keen dying routine will be triggered
    if(hitdetectWithTilePropertyRect(3, l_x, l_y, l_w, l_h, 2<<STC))
    {
        kill(false);
        return;
    }

    // Another property of the tiles may kill keen, also in god mode
    std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);
    // TODO: Workaround! It seems that the deadly tiles are 17 tiles behind. Not sure, why!
    const int tileIDl = mpMap->getPlaneDataAt(1, l_x, (l_y+l_h)+(1<<STC));
    const int tileIDr = mpMap->getPlaneDataAt(1, l_x+l_w, (l_y+l_h)+(1<<STC));
    if(Tile[tileIDl].bup == 9 && Tile[tileIDr].bup == 9 )
    {
        if(!gBehaviorEngine.mCheatmode.god)
        {
            kill(true);
        }
    }


    if(hitdetectWithTilePropertyRect(4, l_x, l_y, l_w, l_h, 2<<STC))
    {
      int dropanimation_sprite = 215;
      const int ep = gBehaviorEngine.getEpisode();

      if(ep == 5)
      {
        dropanimation_sprite = 225;
      }
      else if(ep == 6)
      {
        dropanimation_sprite = 224;
      }

      const int lc_x = l_x>>CSF;
      const int lc_y = l_y>>CSF;
      mpMap->setTile( lc_x, lc_y, 0, true, 1 );
      CItemEffect *iEffect = new CItemEffect(mpMap, 0, lc_x<<CSF, lc_y<<CSF, dropanimation_sprite, ANIMATE);
      spawnObj( iEffect );
      m_Item.m_drops++;

      if(m_Item.m_drops >= 100)
      {
        m_Inventory.addAchievementTask("Thirsty", 1);
        m_Item.m_drops = 0;
        getAnotherLife(lc_x, lc_y, true, true);
      }

      playSound( GameSound::GET_DROP );
    }


    if(mLua)
    {
        for( Uint32 i=21 ; i<=28 ; i++ )
        {
            if(hitdetectWithTilePropertyRect(i, l_x, l_y, l_w, l_h, 2<<STC))
            {
                int value = 0;
                mLua.runFunctionRetOneInt("defineSlipperyTile",
                                          int(i),
                                          value);

                if(value>0)
                {
                    xinertia = xDirection * 16;
                    moveXDir(xinertia*5);
                    return;
                }
            }
        }
    }

    /// Tile Items (Sprite-Items are handled in the CSpriteItem Class)
    // All the collectable items go from 21 to 28
    for( Uint32 i=21 ; i<=28 ; i++ )
    {
        if(hitdetectWithTilePropertyRect(i, l_x, l_y, l_w, l_h, 2<<STC))
        {
            const int lc_x = l_x>>CSF;
            const int lc_y = l_y>>CSF;
            const int ep = gBehaviorEngine.getEpisode();

            mpMap->setTile( lc_x, lc_y, 0, true, 1 );
            spawnObj( new CItemEffect(mpMap, 0, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[ep-4][4+i-21], FADEOUT) );
            switch(i)
            {            
            case 21: m_Item.m_points += 100;	playSound( GameSound::GET_BONUS );	break;
            case 22: m_Item.m_points += 200;	playSound( GameSound::GET_BONUS );	break;
            case 23: m_Item.m_points += 500;	playSound( GameSound::GET_BONUS );	break;
            case 24: m_Item.m_points += 1000;	playSound( GameSound::GET_BONUS );	break;
            case 25: m_Item.m_points += 2000;	playSound( GameSound::GET_BONUS );	break;
            case 26: m_Item.m_points += 5000;	playSound( GameSound::GET_BONUS );	break;
            case 27:
                getAnotherLife(lc_x, lc_y, true, false);
                m_Inventory.addAchievementTask("More than life", 100);
                break;
            case 28: m_Item.m_bullets += 5;	playSound( GameSound::GET_AMMO );	break;
            default: break;
            }

            if(m_Item.m_points >= 50000)
            {
                m_Inventory.addAchievementTask("Still not enough", 50000);
            }

            if(m_Item.m_points >= m_Item.m_lifeAt)
            {
                getAnotherLife(lc_x, lc_y, false, true);
                m_Item.m_lifeAt *= 2;
            }

        }
    }
}



void CPlayerBase::tryToShoot( const GsVec2D<int> &pos, const int xDir, const int yDir )
{
    if(m_Inventory.Item.m_bullets > 0)
    {
        spawnObj(new CBullet(mpMap, 0, pos.x, pos.y, xDir, yDir, mSprVar));

        CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();
        if(!Physics.player.infiniteAmmo)
            m_Inventory.Item.m_bullets--;

        m_Inventory.addAchievementTask("I'm not Duke!", 1);
    }
    else
    {
        playSound( GameSound::GUN_CLICK );
    }

    mReleasedShot = true;
}


// This new function will setup the sprite based on the Action format
bool CPlayerBase::processActionRoutine()
{
    mEndOfAction = false;
    setActionSprite();

    int h_mov = m_Action.h_anim_move<<1;

    const int pax = mPlaycontrol[PA_X];

    if(pax < 0)
    {
        h_mov = (h_mov*pax) / -100;
    }
    else if(pax > 0)
    {
        h_mov = (h_mov*pax) / 100;
    }

    if( m_Action.movement_param > 0 )
    {
        if(xDirection == LEFT )
            moveLeft( h_mov );
        else if(xDirection == RIGHT )
            moveRight( h_mov );

        if(yDirection == UP)
            moveUp( m_Action.v_anim_move<<1 );
        else if(yDirection == DOWN)
            moveDown( m_Action.v_anim_move<<1 );
    }

    if(mEndOfAction)
        return false;


    // Calculate this timer correctly to the applied LPS value
    if(m_Action.delay)
    {
        if( m_ActionTicker > m_Action.delay )
        {
            if(m_Action.Next_action)
            {
                m_Action.setNextActionFormat();
            }
            else
            {
                mEndOfAction = true;
            }

            m_ActionTicker = 0;
        }

        m_ActionTicker++;
    }

    return !mEndOfAction;
}

void CPlayerBase::guideToTarget(const GsVec2D<int> &speed)
{
    // Check x and y diff
    const int xDiff = mTarget.x - getXMidPos();
    const int yDiff = mTarget.y - getYMidPos();
    const int xAbsDiff = abs(xDiff);
    const int yAbsDiff = abs(yDiff);

    // obtained speed
    GsVec2D<int> obtSpeed = speed;

    while( xAbsDiff < obtSpeed.x )
        obtSpeed.x /= 2;

    while( yAbsDiff < obtSpeed.y )
        obtSpeed.y /= 2;

    // Give the speed its right direction.
    obtSpeed.x *= xDiff;
    obtSpeed.y *= yDiff;
    obtSpeed.x = (xAbsDiff != 0) ? obtSpeed.x / xAbsDiff : 0;
    obtSpeed.y = (yAbsDiff != 0) ? obtSpeed.y / yAbsDiff : 0;

    moveDir(obtSpeed);
}


const int MAX_WALKSOUNDTIMER = 27;

void CPlayerBase::playWalkSound()
{
    const int time = m_walktimer % MAX_WALKSOUNDTIMER;

    // Process walk timer. This is only for the walking sound
    if( time == MAX_WALKSOUNDTIMER/3 || time == (2*MAX_WALKSOUNDTIMER)/3 )
        playSound( GameSound::KEEN_WALK );
    else if( time == 0 )
        playSound( GameSound::KEEN_WALK2 );

    m_walktimer++;
}


void CPlayerBase::playSwimSound(const bool moving)
{

    int time = m_walktimer;

    // More frequency of swim sound when Keen is moving to somewhere
    if(moving)
    {
        time %= (MAX_WALKSOUNDTIMER/3);
    }
    else
    {
        time %= (MAX_WALKSOUNDTIMER/2);
    }

    // Process walk timer. This is only for the walking sound
    if( time == 0 )
    {
        playSound( GameSound::KEEN_SWIM );
    }

    m_walktimer++;
}






void CPlayerBase::getEaten()
{
    // Here were prepare Keen to die, setting the action to die
    if(!gBehaviorEngine.mCheatmode.god && !mDying)
    {
        mDying = true;

        yinertia = 0;
        dontdraw = true;
        solid = false;
        honorPriority = false;
        m_timer = 0;
        playSound( GameSound::KEEN_DIE,
                          SoundPlayMode::PLAY_NORESTART );

        mp_processState = &CPlayerBase::processGetEaten;
    }
}



void CPlayerBase::respawnImportantItem(const int itemId)
{
    int epOffset = 0;
    int itemOffset = 0;

    int episode = gBehaviorEngine.getEpisode();

    if(episode == 4)
    {
        epOffset = 118;
        itemOffset = 57;
    }
    else if(episode == 5)
    {
        epOffset = 122;
        itemOffset = 0x39;

        if(itemId == 4)
        {
            GsVec2D<int> where = mpMap->getSpriteOrigin(105);
            spawnObj( new galaxy::CSpriteItem(mpMap, 0x46, where.x, where.y, 105, 0) );
            return;
        }
    }
    else if(episode == 6)
    {
        epOffset = 118;
        itemOffset = 0x39;
    }

    const Uint32 newsprite = epOffset+2*itemId;

    // Now respawn the item
    GsVec2D<int> where = mpMap->getSpriteOrigin(itemId+itemOffset);
    spawnObj( new galaxy::CSpriteItem(mpMap, itemId+itemOffset, where.x, where.y, newsprite, 0) );
}


bool CPlayerBase::touchedBottomOfMap()
{
    int l_x = getXLeftPos();
    int l_y = getYUpPos();
    int l_w = getXRightPos() - getXLeftPos();
    int l_h = getYDownPos() - getYUpPos();

    const int ep = gBehaviorEngine.getEpisode();

    // Some tests will force Keen to death!!
    /*if(ep == 4)
    {
        // In Keen 4 most of the levels have that block and normally can only be touched at some time
        // if in godmode. In that case, we force Keen to die.
        bool bottomTouched = false;

        bottomTouched |= hitdetectWithTile(0x03CD, l_x, l_y, l_w, l_h, 2<<STC);

        if(bottomTouched)
        {
            return true;
        }
    }
    else*/ if(ep == 6)
    {
        // In Keen 6 some levels have that block and normally can only be touched in case
        // if in godmode. In that case, we force Keen to die.
        bool bottomTouched = false;

        bottomTouched |= hitdetectWithTile(0x03F2, l_x, l_y, l_w, l_h, 2<<STC);
        bottomTouched |= hitdetectWithTile(0x03F3, l_x, l_y, l_w, l_h, 2<<STC);

        if(bottomTouched)
        {
            return true;
        }
    }

    return false;
}


void CPlayerBase::processDead()
{
    if(getActionNumber() == A_KEEN_DIE)
    {
        setActionForce(A_KEEN_DIE_ALT);
    }
    else if(getActionNumber() == A_KEEN_DIE_ALT)
    {
        setActionForce(A_KEEN_DIE);
    }



    const int levelObj = mpMap->getLevel();
    const std::string &levelName = mpMap->getLevelName();

    m_camera.forbidLead(mPlayerNum);
    m_camera.cycleCamlead();
    gEventManager.add( new EventDieKeenPlayer(mPlayerNum,
                                              levelObj, levelName) );
    mIsDead = false;
    mDying = false;
    exists = false;

    if(m_Inventory.Item.m_gem.red)
        respawnImportantItem(0);
    if(m_Inventory.Item.m_gem.yellow)
        respawnImportantItem(1);
    if(m_Inventory.Item.m_gem.blue)
        respawnImportantItem(2);
    if(m_Inventory.Item.m_gem.green)
        respawnImportantItem(3);

    if(m_Inventory.Item.m_keycards)
        respawnImportantItem(4);

    m_Inventory.Item.m_gem.clear();
    m_Inventory.Item.m_keycards = 0;
}



const int DIE_FALL_SPEED = 7;
const int DIE_FALL_MAX_INERTIA = 150;
const int DIE_RIGHT_INERTIA = 40;

void CPlayerBase::processDying()
{
    moveYDir(yinertia);
    moveRight(DIE_RIGHT_INERTIA);
    yinertia += DIE_FALL_SPEED;

    if( m_camera.outOfSight() )
    {
        mIsDead = true;
        honorPriority = true;
    }
}




const int DIE_GETEATEN_TIME = 240;

void CPlayerBase::processGetEaten()
{
    if(m_timer >= DIE_GETEATEN_TIME)
    {
        mIsDead = true;
        solid = true;
        honorPriority = true;
        m_timer = 0;
    }
    else
    {
        m_timer++;
    }
}



void CPlayerBase::kill(const bool force,
                       const bool noDieProcess)
{
    if(getActionNumber(A_KEEN_ENTER_DOOR))
        return;

    if(gBehaviorEngine.mCheatmode.god && !force)
        return;

    const auto firstTime = !mDying;

    mDying = true;
    if(getActionStatus(A_KEEN_DIE))
    {
        setActionForce(A_KEEN_DIE_ALT);
    }
    else
    {
        setActionForce(A_KEEN_DIE);
    }


    if(!noDieProcess)
    {
        if(mp_processState == &CPlayerBase::processDying &&
           (yinertia < 0 && !firstTime))
            return;

        yinertia = -DIE_FALL_MAX_INERTIA;

    }
    else
    {
        mIsDead = true;
    }

    solid = false;
    honorPriority = false;
    playSound( GameSound::KEEN_DIE, SoundPlayMode::PLAY_NORESTART );
}


bool CPlayerBase::checkMapBoundaryR(const int x2)
{
    if( solid && !noclipping && x2 >= (int)((mpMap->m_width-1)<<CSF) )
        return true;

    return false;
}

bool CPlayerBase::checkMapBoundaryL(const int x1)
{
    if( solid && !noclipping && x1 <= (1<<CSF) )
        return true;

    return false;
}

bool CPlayerBase::checkMapBoundaryU(const int y1)
{
    if( y1 <= (1<<CSF) )
        return true;

    return false;
}

void CPlayerBase::push(CGalaxySpriteObject& theObject)
{
    if( mIsDead )
        return;

    int obj_lx = theObject.getXLeftPos();
    int obj_midx = theObject.getXMidPos();
    int obj_rx = theObject.getXRightPos();
    int lx = getXLeftPos();
    int midx = getXMidPos();
    int rx = getXRightPos();

    if( midx < obj_midx )
    {
        moveLeft(rx - obj_lx);
    }

    if( midx > obj_midx )
    {
        moveRight(obj_rx - lx);
    }
}



bool CPlayerBase::getActionStatus(int16_t ActionNumber)
{
    const int epID = gBehaviorEngine.getEpisode()-4;
    const int relOff = mEpisodeActionNumMap[epID][ActionNumber];

    return (m_Action.getActionFormat(m_ActionBaseOffset + 30*relOff));
}


int CPlayerBase::getSpriteIDFromAction(const int16_t ActionNumber)
{
    ActionFormatType action;

    const int epID = gBehaviorEngine.getEpisode()-4;
    const int relOff = mEpisodeActionNumMap[epID][ActionNumber];

    action.setActionFormat(m_ActionBaseOffset + 30*relOff);

    if( xDirection < 0 )
        return action.spriteLeft;
    else
        return action.spriteRight;
}



void CPlayerBase::setActionForce(const size_t ActionNumber)
{
    CGalaxySpriteObject::setActionForce(ActionNumber);

    const int epID = gBehaviorEngine.getEpisode()-4;
    mEndOfAction = false;
    m_ActionNumber = ActionNumber;
    m_Action.setActionFormat(m_ActionBaseOffset + 30*mEpisodeActionNumMap[epID][m_ActionNumber]);


    if(mActionMap.empty())
        return;

    if( mActionMap.find(ActionNumber) != mActionMap.end() )
        mp_processState = mActionMap[ActionNumber];
    else
    {
        mEndOfAction = false;
        m_ActionNumber = mActionMap.begin()->first;
        m_Action.setActionFormat(m_ActionBaseOffset + 30*mEpisodeActionNumMap[epID][m_ActionNumber]);
    }
}



};
