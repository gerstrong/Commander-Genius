/*
 * CPlayerLevel.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include "CPlayerLevel.h"
#include "platform/CPlatform.h"
#include "../CGalaxySpriteObject.h"
#include "CSecurityDoor.h"

#include <base/audio/Audio.h>
#include <graphics/CColorMerge.h>

#include <base/CInput.h>
#include <base/GsTimer.h>
#include <base/GsLogging.h>
#include <base/interface/property_tree/property_tree.h>
#include <engine/core/CBehaviorEngine.h>


#include "../dialog/CMessageBoxBitmapGalaxy.h"


namespace galaxy {


//const int MAX_JUMPHEIGHT = 10;
//const int MIN_JUMPHEIGHT = 5;

//const int MAX_POGOHEIGHT = 20;
//const int MIN_POGOHEIGHT = 5;
//const int POGO_SLOWDOWN = 4;

constexpr int POGO_START_INERTIA = -125; // 48 In K5 Disassemble
//const int POGO_START_INERTIA_VERT = -100;
//const int POGO_START_INERTIA_MAX_VERT = -168;
constexpr int POGO_START_INERTIA_IMPOSSIBLE_VERT = -180;
//const int POGO_INERTIA_HOR_MAX = 64;
//const int POGO_INERTIA_HOR_REACTION = 2;

const int FIRE_RECHARGE_TIME = 5;

const int MAX_POLE_GRAB_TIME = 10;

const int MAX_SCROLL_VIEW = (8<<CSF);

const int BELT_SPEED = 16;

const int STUN_TIME = 30;


CPlayerLevel::CPlayerLevel(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y,
                        std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtrs, direction_t facedir,
                        CInventory &l_Inventory,
                        const size_t offset, const int playerIdx, const int spriteTableIdx) :
CPlayerBase(pMap, foeID, x, y, facedir, l_Inventory, playerIdx, spriteTableIdx),
mObjectPtrs(ObjectPtrs)
{
    mActionMap[A_KEEN_STAND] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processStanding);
    mActionMap[A_KEEN_ON_PLAT] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processStanding);
    mActionMap[A_KEEN_QUESTION] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processStanding);
    mActionMap[A_KEEN_BORED] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processStanding);
    mActionMap[A_KEEN_BOOK_OPEN] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processReadingBook);
    mActionMap[A_KEEN_BOOK_READ] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processReadingBook);
    mActionMap[A_KEEN_BOOK_CLOSE] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processReadingBook);
    mActionMap[A_KEEN_MOON] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPants);
    mActionMap[A_KEEN_LOOKUP] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processLookingUp);
    mActionMap[A_KEEN_LOOKDOWN] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processLookingDown);
    mActionMap[A_KEEN_SHOOT] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processShootWhileStanding);
    mActionMap[A_KEEN_SHOOT_UP] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processShootWhileStanding);
    mActionMap[A_KEEN_SLIDE] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processSliding);
    mActionMap[A_KEEN_ENTER_DOOR] = static_cast<void (CPlayerBase::*)()>(&CPlayerLevel::processEnterDoor);
    mActionMap[A_KEEN_POLE] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPoleClimbingSit);
    mActionMap[A_KEEN_POLE_CLIMB] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPoleClimbingUp);
    mActionMap[A_KEEN_POLE_SLIDE] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPoleSlidingDown);
    mActionMap[A_KEEN_POLE_SHOOT] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPoleClimbingSit);
    mActionMap[A_KEEN_POLE_SHOOTUP] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPoleClimbingUp);
    mActionMap[A_KEEN_POLE_SHOOTDOWN] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPoleSlidingDown);
    mActionMap[A_KEEN_RUN] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processRunning);
    mActionMap[A_KEEN_POGO_START] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPogoBounce);
    mActionMap[A_KEEN_POGO_UP] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPogo);
    mActionMap[A_KEEN_POGO_HIGH] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processPogo);
    mActionMap[A_KEEN_JUMP] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processJumping);
    mActionMap[A_KEEN_JUMP_DOWN] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processJumping);
    mActionMap[A_KEEN_FALL] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processJumping);
    mActionMap[A_KEEN_JUMP_SHOOT] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processJumping);
    mActionMap[A_KEEN_JUMP_SHOOTUP] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processJumping);
    mActionMap[A_KEEN_JUMP_SHOOTDOWN] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processJumping);
    mActionMap[A_KEEN_HANG] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processCliffHanging);
    mActionMap[A_KEEN_CLIMB] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processCliffClimbingUp);
    mActionMap[A_KEEN_STUNNED] = static_cast<void (CPlayerBase::*)()> (&CPlayerLevel::processStunned);

    setupGalaxyObjectOnMap(offset, A_KEEN_STAND);

    performCollisions();
    if(!processActionRoutine())
    {
            exists = false;
    }
}


void
CPlayerLevel::
deserialize(GsKit::ptree &node)
{
  mIsClimbing = node.get<bool>("isClimbing", false);
  mActionState =  KeenState();
}

void
CPlayerLevel::
serialize(GsKit::ptree &node)
{
  node.put("isClimbing", mIsClimbing);
}



bool CPlayerLevel::verifyforPole()
{
    // Timeout before Keen can grab again after he jumped off.
    if ( mPoleGrabTime < MAX_POLE_GRAB_TIME )
    {
        return false;
    }

    mPoleGrabTime = 0;

    auto l_x_r = getXRightPos();
    auto l_x_l = getXLeftPos();
    const int l_y_up = ( getYUpPos() ) + (4<<STC);
    const int l_y_down = ( ( getYDownPos() >> CSF ) + 1 ) << CSF;

    const int yDir = (mPlaycontrol[PA_Y] < 0) ? -1 : 1;

    // Now check if Player has the chance to climb a pole or something similar
    // 1 = this tile is a pole (Property)

    if(yDir == 0) return false;

    const auto firstX = l_x_l-(1<<CSF);
    const auto lastX  = l_x_r+(1<<CSF);
    bool foundPole = false;
    auto xTarget = firstX;

    for(int x = firstX ; x<=lastX ; x+=(1<<CSF))
    {
        if( ( yDir < 0 && hitdetectWithTileProperty(1, x, l_y_up)  ) ||
            ( yDir > 0 && hitdetectWithTileProperty(1, x, l_y_down)  ) )
        {
            // first pole found
            if(!foundPole)
            {
                xTarget = x;
                foundPole = true;
            }
            else // Is there another pole closer to the player?
            {
                const auto xdiffTarget = std::abs(getXMidPos()-xTarget);
                const auto xdiffNewTarget = std::abs(getXMidPos()-x);

                if(xdiffNewTarget<xdiffTarget)
                {
                   xTarget = x;
                }
            }
        }
    }

    if(foundPole)
    {
        cancelAllMoveTasks();

        // Move to the proper X Coordinates, so Keen really grabs it!
        const auto polePosX = (xTarget>>CSF)<<CSF;
        const auto centeredX = polePosX-((1<<CSF)/2);
        moveTo(GsVec2D<int>(centeredX, getYPosition()));

        xinertia = 0;

        //next.x = 0;
        //next.y = 64*yDir;

        // Set Keen in climb mode
        setAction(A_KEEN_POLE);
        mIsClimbing = true;
        mClipped = false;
        solid = false;
        return true;
    }

    return false;
}


void CPlayerLevel::makeHimStand()
{
    const auto ep = gBehaviorEngine.getEpisode();
    if(pSupportedbyobject && ep == 5)
        setAction(A_KEEN_ON_PLAT);
    else
        setAction(A_KEEN_STAND);
}


int CPlayerLevel::evalVertPogoInertia()
{
    // If Super Pogo is enabled
    if( gInput.SuperPogo(mPlayerCtrlNum) )
    {
        return POGO_START_INERTIA_IMPOSSIBLE_VERT;
    }
    if( gInput.ImpossiblePogo(mPlayerCtrlNum) )
    {
        // Or if the player triggered the impossible pogo trick
        if(mActionState.jumpIsPressed && mActionState.pogoIsPressed)
        {
            return POGO_START_INERTIA_IMPOSSIBLE_VERT;
        }
    }

    return POGO_START_INERTIA;
}


void CPlayerLevel::processRunning()
{
    prepareToShoot();

    int px = mPlaycontrol[PA_X];
    int py = mPlaycontrol[PA_Y];

    // Most of the walking routine is done by the action script itself

    // Center the view after Keen looked up or down
    centerView();

    // He could stand again, if player doesn't move the dpad
    if( px == 0 )
    {
        makeHimStand();
        yDirection = 0;
        handleInputOnGround();
        return;
    }
    else if( !blockedl && !blockedr )
    {
        playWalkSound();
    }


    if ( py == -1)
    {
        if ( verifyforPole() ) return;
    }
    else if ( py == 1)
    {
        if ( verifyforPole() ) return;
    }


    xDirection = 0;
    if( px < 0 )
    {
        xDirection = -1;
    }
    else if( px > 0 )
    {
        xDirection = 1;
    }


    if (mActionState.jumpIsPressed && !mActionState.jumpWasPressed)
    {
        mActionState.jumpWasPressed = true;

        // If you pressed run, perform a long jump
        if(mPlaycontrol[PA_RUN])
        {
            xinertia = xDirection * 32;
        }
        else
        {
            xinertia = xDirection * 16;
        }

        yinertia = -90;
        /*nextX = */nextY = 0;
        mActionState.jumpTimer = 18;
        mMustHandleSlope = true;
        setAction(A_KEEN_JUMP);
        playSound( GameSound::KEEN_JUMP );
        return;
    }

    if(m_Inventory.Item.m_special.mCanPogo)
    {
        if (mActionState.pogoIsPressed && !mActionState.pogoWasPressed)
        {
            mActionState.pogoWasPressed = true;

            // If you pressed run, perform a long jump
            if(mPlaycontrol[PA_RUN])
            {
                xinertia = xDirection * 32;
            }
            else
            {
                xinertia = xDirection * 16;
            }

            yinertia = evalVertPogoInertia();
            //		nextX = 0;
            mActionState.jumpTimer = 24;
            mMustHandleSlope = true;
            playSound( GameSound::KEEN_POGO );
            setAction(A_KEEN_POGO_START);
            return;
        }
    }


    if (verifyForFalling())
    {
        xinertia = xDirection * 8;
        yinertia = 0;
        setAction(A_KEEN_FALL);

        playSound( GameSound::KEEN_FALL );

        mActionState.jumpTimer = 0;
        mMustHandleSlope = true;
    }
    else if ( (blockedl && xDirection == -1) || (blockedr && xDirection == 1))
    {
        makeHimStand();
        yDirection = 0;
    }

    // He could place a gem
    for( Uint32 i=7 ; i<=10 ; i++ )
    {
        const int l_x = getXMidPos();
        const int l_y = getYDownPos()-(3<<STC);

        // This will change the gemholder to a holder with the gem
        if( hitdetectWithTileProperty(i, l_x, l_y) )
        {
            stItemGalaxy &m_Item = m_Inventory.Item;

            if(i == 7 && m_Item.m_gem.red > 0)
            {
                m_Item.m_gem.red--;
                m_Inventory.addAchievementTask("Open Sesame!", 1);
            }
            else if(i == 8 && m_Item.m_gem.yellow > 0)
            {
                m_Item.m_gem.yellow--;
                m_Inventory.addAchievementTask("Open Sesame!", 1);
            }
            else if(i == 9 && m_Item.m_gem.blue > 0)
            {
                m_Item.m_gem.blue--;
                m_Inventory.addAchievementTask("Open Sesame!", 1);
            }
            else if(i == 10 && m_Item.m_gem.green > 0)
            {
                m_Item.m_gem.green--;
                m_Inventory.addAchievementTask("Open Sesame!", 1);
            }
            else
                break;

            moveToHorizontal((l_x>>CSF)<<CSF);
            mPlacingGem = true;
            setAction(A_KEEN_SLIDE);
        }
    }

}


void CPlayerLevel::prepareToShoot()
{
    // He could shoot
    if( mPlaycontrol[PA_FIRE] && !m_fire_recharge_time )
    {
        setAction(A_KEEN_SHOOT);
        mReleasedShot = false;
        mp_processState = (void (CPlayerBase::*)()) &CPlayerLevel::processShootWhileStanding;
        m_fire_recharge_time = FIRE_RECHARGE_TIME;
        return;
    }
}

void CPlayerLevel::handleInputOnGround()
{
    int px = mPlaycontrol[PA_X];
    int py = mPlaycontrol[PA_Y];


    if(px)
    {
        setAction(A_KEEN_RUN);
        processRunning();

        //nextX = (xDirection * m_Action.h_anim_move)/4;
        return;
    }

    if( mActionState.jumpIsPressed && !mActionState.jumpWasPressed)
    {
        mActionState.jumpWasPressed = true;
        xinertia = 0;
        yinertia = -90;
        nextY = 0;
        mActionState.jumpTimer = 18;
        mMustHandleSlope = true;
        setAction(A_KEEN_JUMP);
        playSound( GameSound::KEEN_JUMP );
        return;
    }

    if(m_Inventory.Item.m_special.mCanPogo)
    {
        if( mActionState.pogoIsPressed && !mActionState.pogoWasPressed)
        {
            mActionState.pogoWasPressed = true;
            xinertia = 0;
            yinertia = evalVertPogoInertia();
            setAction(A_KEEN_POGO_START);
            playSound( GameSound::KEEN_POGO );
            nextY = 0;
            mActionState.jumpTimer = 24;
            mMustHandleSlope = true;
            return;
        }
    }

    // He could duck or use the pole
    if( py > 0 )
    {
        if(verifyforPole()) return;

        setAction(A_KEEN_LOOKDOWN);
        return;
    }

    // He could press up and do further actions
    if( py < 0 )
    {
        processPressUp();
    }


}


bool CPlayerLevel::moonTiledetected()
{
    if( gBehaviorEngine.getEpisode() != 4 )
    return false;

    int lx = getXLeftPos()-(1<<STC);
    int ly = getYUpPos()-(1<<STC);
    int lw = getXRightPos()-getXLeftPos()+(2<<STC);
    int lh = getYDownPos()+getYUpPos()+(2<<STC);

    if( hitdetectWithTilePropertyRect(16, lx, ly, lw, lh, 1<<STC) )
    return true;

    return false;
}



/// Keen standing routine
void CPlayerLevel::processStanding()
{
    prepareToShoot();

    verifyFalling();

    int px = mPlaycontrol[PA_X];
    int py = mPlaycontrol[PA_Y];

    if( px || py || mActionState.jumpIsPressed || mActionState.pogoIsPressed )
    {
        user1 = user2 = 0;
        handleInputOnGround();

        return;
    }

    if( getActionStatus(A_KEEN_STAND) && !pSupportedbyobject )
    {
        user1++;

        if (user2 == 0 && user1 > 200)
        {
            user2++;
            setAction(A_KEEN_QUESTION);
            user1 = 0;
            return;
        }

        if (user2 == 1 && user1 > 300)
        {
            user2++;
            setAction(A_KEEN_BORED);
            user1 = 0;
            return;
        }

        if (user2 == 2 && user1 > 700)
        {
            user2++;

            if(moonTiledetected())
            {
                setAction(A_KEEN_MOON);
            }
            else
            {
                setAction(A_KEEN_BOOK_OPEN);
            }
            user1 = 0;
            return;
        }
    }

    // Center the view after Keen looked up or down
    centerView();
}



void CPlayerLevel::processReadingBook()
{
    if( mPlaycontrol[PA_X] )
    {
    user1 = user2 = 0;
    setAction(A_KEEN_BOOK_CLOSE);
    return;
    }

    if( getActionStatus(A_KEEN_STAND) )
    setAction(A_KEEN_STAND);
}


void CPlayerLevel::processPants()
{
    if( getActionStatus(A_KEEN_STAND) )
    setAction(A_KEEN_STAND);
}



void CPlayerLevel::processLookingDown()
{
    verifyFalling();

    //Try to jump down
    if (mActionState.jumpIsPressed && !mActionState.jumpWasPressed && (blockedd&7) == 1)
    {
        mActionState.jumpWasPressed = true;

        //If the tiles below the player are blocking on any side but the top, they cannot be jumped through
        /*int tile1 = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2, 1);
        int tile2 = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2+1, 1);

        if (TI_ForeLeft(tile1) || TI_ForeBottom(tile1) || TI_ForeRight(tile1))
            return;

        if (TI_ForeLeft(tile2) || TI_ForeBottom(tile2) || TI_ForeRight(tile2))
            return;

        #define max(a,b) ((a>b)?a:b)

        int deltay = max(gTimer.getTicksPerFrame(),4) << 4;

        //TODO: Moving platforms

        clipRects.unitY2 += deltay;
        posY += deltay;
        nextX = 0;
        nextY = 0;
        setAction(A_KEEN_FALL);
        xinertia = yinertia = 0;*/
        //playSound( GameSound::KEEN_FALL );
        return;
    }


    if ( mPlaycontrol[PA_Y] <= 0 || mPlaycontrol[PA_X] != 0
            || (mActionState.jumpIsPressed && !mActionState.jumpWasPressed)
            || (mActionState.pogoIsPressed && !mActionState.pogoWasPressed))
    {
        makeHimStand();
        yDirection = 0;
        return;
    }


    if( mPlaycontrol[PA_Y]>0 )
    {
        if ( mPlaycontrol[PA_JUMP] > 0 && !onslope  )
        {
            const bool jumpdowntile = canFallThroughTile();
            if( pSupportedbyobject || jumpdowntile )
            {
                m_jumpdown = jumpdowntile;
                blockedd = false;

                yinertia = 0;

                // This is for platform which might be going way down...
                if( (m_jumpdownfromobject = (pSupportedbyobject != nullptr)) == true )
                {
                    if(pSupportedbyobject->yDirection == DOWN )
                    {
                        yinertia = 60;
                    }
                }

                xinertia = 0;

                pSupportedbyobject = nullptr;
                setAction( A_KEEN_JUMP_DOWN );
                playSound( GameSound::KEEN_FALL );
            }
        }

        if( m_camera.mRelcam.y < MAX_SCROLL_VIEW )
            m_camera.mRelcam.y += (2<<STC);

        return;
    }

    makeHimStand();
}


// This special code is important, so platforms in all cases will catch Keen when he is falling on them
void CPlayerLevel::processMoveBitDown()
{
    auto obj = mObjectPtrs.begin();
    for(  ; obj != mObjectPtrs.end() ; obj++ )
    {
        if( CPlatform *platform = dynamic_cast<CPlatform*>(obj->get()) )
        {
            if(platform->hitdetect(*this))
            {
                platform->getTouchedBy(*this);
            }
        }
    }

    CSpriteObject::processMoveBitDown();
}



void CPlayerLevel::getTouchedBy(CSpriteObject &theObject)
{
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        if(player->getYDownPos() < getYUpPos()+(6<<STC) )
        {
            // The other keen might be able use this one to reach higher places
            const int myAction = getActionNumber();

            if( (myAction < A_KEEN_BOOK_OPEN ||  myAction > A_KEEN_BOOK_CLOSE) &&
                (myAction < A_KEEN_POLE ||  myAction > A_KEEN_POLE_SHOOTDOWN))

            {
               player->blockedd = true;
            }
        }
    }
}


void CPlayerLevel::processInput()
{
    CPlayerBase::processInput();

    if(mPlaycontrol[PA_Y] >= 0)
    {
        m_EnterDoorAttempt = false;
    }

    mActionState.jumpWasPressed = mActionState.jumpIsPressed;
    mActionState.pogoWasPressed = mActionState.pogoIsPressed;

    mActionState.jumpIsPressed = mPlaycontrol[PA_JUMP];
    mActionState.pogoIsPressed = mPlaycontrol[PA_POGO];

    if (!mActionState.jumpIsPressed) mActionState.jumpWasPressed = false;
    if (!mActionState.pogoIsPressed) mActionState.pogoWasPressed = false;
}


void CPlayerLevel::shootInAir()
{
    // process simple shooting
    if( mPlaycontrol[PA_Y] < 0 )
    {
        setActionForce(A_KEEN_JUMP_SHOOTUP);
        const GsVec2D<int> newVec(getXMidPos()-(3<<STC), getYUpPos()-(16<<STC));
        tryToShoot(newVec, 0, -1);
    }
    else if( mPlaycontrol[PA_Y] > 0 )
    {
        setActionForce(A_KEEN_JUMP_SHOOTDOWN);
        const GsVec2D<int> newVec(getXMidPos()-(3<<STC), getYDownPos());
        tryToShoot(newVec, 0, 1);
    }
    else
    {
        setActionForce(A_KEEN_JUMP_SHOOT);

        const GsVec2D<int> newVec(getXPosition() + ((xDirection < 0) ? -(16<<STC) : (16<<STC)),
                                    getYPosition()+(4<<STC));
        tryToShoot(newVec, xDirection, 0);
    }
    m_fire_recharge_time = FIRE_RECHARGE_TIME;
}





const int MAX_CLIFFHANG_TIME = 10;

bool CPlayerLevel::checkandtriggerforCliffHanging()
{
    if(PoleCollision())
        return false;

    if(mp_processState == (void (CPlayerBase::*)()) &CPlayerLevel::processPogo)
        return false;

    std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();

    const bool floorNearBy = TileProperty[mpMap->at((getXMidPos()>>CSF), (getYDownPos()>>CSF)+1)].bup;

    if(floorNearBy)
        return false;

    const int yUp = getYUpPos()>>CSF;

    if( mPlaycontrol[PA_X]<0 && blockedl )
    {
        const int xLeft = (getXLeftPos()>>CSF)-1;
        const int xMid = (getXMidPos()>>CSF)-1;

        bool check_block = false;
        for(int i=1 ; i<3 ; i++)
        {
            const auto left = mpMap->at(xLeft, yUp-i);
            check_block |= TileProperty[left].bup;
            check_block |= TileProperty[left].bright;

            const auto middle = mpMap->at(xMid, yUp-i);
            check_block |= TileProperty[middle].bup;
            check_block |= TileProperty[middle].bright;
        }

        const auto mapLowerOff = mpMap->at(xLeft, yUp);
        const bool check_block_lower = TileProperty[mapLowerOff].bright;

        if( !check_block && check_block_lower )
        {
            setAction(A_KEEN_HANG);
            setActionSprite();
            calcBoundingBoxes();

            Uint32 x = (xLeft+1)<<CSF;
            Uint32 y = yUp<<CSF;

            x -= m_BBox.x1;

            bool check_block_rhs = true;

            do
            {
                // Additional check so player does move too much into the tile
                const auto rightBlockIdx = mpMap->at(x>>CSF, y>>CSF);
                bool check_block_rhs = TileProperty[rightBlockIdx].bright;

                if(!check_block_rhs)
                    break;

                x += (1<<STC);
            } while(check_block_rhs);

            x -= (4<<STC);
            y -= m_BBox.y1;

            moveTo(x,y);
            solid = false;
            xinertia = 0;
            yinertia = 0;
            m_hangtime = MAX_CLIFFHANG_TIME;
            return true;
        }
    }
    else if( mPlaycontrol[PA_X]>0 && blockedr )
    {
        const int xRight = (getXRightPos()>>CSF)+1;

        bool check_block = false;
        for(int i=1 ; i<3 ; i++)
        {
            const auto mapUpperOff = mpMap->at(xRight, yUp-i);
            check_block |= TileProperty[mapUpperOff].bup;
            check_block |= TileProperty[mapUpperOff].bleft;
        }

        const auto mapLower1Off = mpMap->at(xRight, yUp);
        bool check_block_lower = TileProperty[mapLower1Off].bleft;

        if( !check_block && check_block_lower )
        {
            setAction(A_KEEN_HANG);
            setActionSprite();
            calcBoundingBoxes();

            Uint32 x = (xRight)<<CSF;
            Uint32 y = yUp<<CSF;

            x -= m_BBox.x2;
            y -= m_BBox.y1;


            moveTo(x,y);
            solid = false;
            xinertia = 0;
            yinertia = 0;
            m_hangtime = MAX_CLIFFHANG_TIME;
            return true;
        }
    }
    return false;
}


void CPlayerLevel::processCliffHanging()
{
    const int yUp = (getYUpPos()+(5<<STC))>>CSF;
    const int xLeft = getXLeftPos()>>CSF;
    const int xRight = getXRightPos()>>CSF;

    std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();

    if( xDirection == LEFT )
    {
        bool check_block = TileProperty[mpMap->at(xLeft-1, yUp)].bup;
        check_block |= TileProperty[mpMap->at(xLeft, yUp)].bup;

        if( !check_block )
        {
            setAction(A_KEEN_FALL);
            playSound( GameSound::KEEN_FALL );
            solid = true;
            return;
        }
    }
    else if( xDirection == RIGHT )
    {
        bool check_block = TileProperty[mpMap->at(xRight+1, yUp)].bup;
        check_block |= TileProperty[mpMap->at(xRight, yUp)].bup;

        if( !check_block )
        {
            setAction(A_KEEN_FALL);
            playSound( GameSound::KEEN_FALL );
            solid = true;
            return;
        }
    }


    // In case you released the direction
    if( mPlaycontrol[PA_Y] == 0 && mPlaycontrol[PA_X] == 0)
    {
        m_hangtime = 0;
    }

    if(m_hangtime > 0)
    {
        m_hangtime--;
        return;
    }

    // If you want to climb up
    if( ((xDirection == LEFT)  && (mPlaycontrol[PA_X] < 0)) ||
            ((xDirection == RIGHT) && (mPlaycontrol[PA_X] > 0))  )
    {
        // This will reset the target whenever the process object of climbing is launched
        mTarget.x = -1;
        mTarget.y = -1;
        setAction(A_KEEN_CLIMB);
    }

    // If you want to fall down.
    else if( mPlaycontrol[PA_Y] > 0 ||
        ((xDirection == LEFT) && (mPlaycontrol[PA_X] > 0)) ||
        ((xDirection == RIGHT) && (mPlaycontrol[PA_X] < 0))  )
    {
        setAction( A_KEEN_FALL );
        playSound( GameSound::KEEN_FALL );
        solid = true;
        const int dx = 8<<STC;
        moveXDir( (xDirection == LEFT) ? dx : -dx, true);
        setActionSprite();
        calcBoundingBoxes();
    }
}




const int PLAYER_CLIMB_SPEED_Y = 15;

void CPlayerLevel::processCliffClimbingUp()
{
    if(mTarget.x < 0 || mTarget.y < 0)
    {
        std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();
        const int spriteID = getSpriteIDFromAction(A_KEEN_STAND);
        GsSprite &standSpr = gGraphics.getSprite(mSprVar,spriteID);


        if( xDirection < 0 ) // left upper
        {
            const int height = (standSpr.getHeight() << (CSF-STC)) - (5<<STC);

            mClimbSpeedX = (5*PLAYER_CLIMB_SPEED_Y)/8;

            const int xMove = (((xDirection)<<CSF)*6)/6;
            int yPos = getYPosition();

            mTarget.x = getXMidPos() + xMove;
            mTarget.y = ((yPos>>CSF)<<CSF);

            int block = TileProperty[mpMap->at((mTarget.x>>CSF), (mTarget.y>>CSF))].bup;

            if(block)
                mTarget.y -= (1<<CSF);

            mTarget.y -= height;
        }
        else // right upper
        {
            const int height = (standSpr.getHeight() << (CSF-STC)) - (8<<STC);

            mClimbSpeedX = (3*PLAYER_CLIMB_SPEED_Y)/8;

            const int xMove = (((xDirection)<<CSF)*2)/6;
            const int yPos = getYPosition();

            mTarget.x = getXMidPos() + xMove;
            mTarget.y = ((yPos>>CSF)<<CSF);

            int block = TileProperty[mpMap->at((mTarget.x>>CSF), (mTarget.y>>CSF))].bup;

            if(block)
                mTarget.y -= (1<<CSF);

            mTarget.y -= height;
        }

        return;
    }

    // This is where Keen climbs the cliff up.
    guideToTarget( GsVec2D<int>(0, 4*PLAYER_CLIMB_SPEED_Y) );

    yDirection = 0;
    const int yDiff = mTarget.y - getYMidPos();

    const int abs_y = abs(yDiff);
    if( abs_y < 24 )
    {
        mp_processState = (void (CPlayerBase::*)()) &CPlayerLevel::processCliffClimbingOntoFloor;
    }
}


void CPlayerLevel::processCliffClimbingOntoFloor()
{
    // This is where Keen climbs the cliff towards the floor.
    guideToTarget( GsVec2D<int>(10*mClimbSpeedX, PLAYER_CLIMB_SPEED_Y) );

    if( getActionStatus(A_KEEN_STAND) || getActionStatus(A_KEEN_ON_PLAT) )
    {
        yDirection = 0;
        const int xDiff = mTarget.x - getXMidPos();

        const int abs_x = abs(xDiff);
        if(abs_x < mClimbSpeedX)
        {
            solid = true;
            setActionSprite();
            calcBoundingBoxes();
            makeHimStand();
            calcBoundingBoxes();
            processMove(0,-(1<<CSF));
            processMove(0,(1<<CSF));
        }
    }
}

bool CPlayerLevel::stun()
{
    bool rejectCriteria = false;

    rejectCriteria |= !blockedd;
    rejectCriteria |= getActionStatus(A_KEEN_SLIDE);

    if(rejectCriteria)
        return false;

    setAction(A_KEEN_STUNNED);
    playSound(GameSound::KEEN_STUNNED);

    mEndOfAction = false;
    m_Action.setActionFormat(gBehaviorEngine.isDemo() ? 0x1862 : 0x1868);

    mStunTimer = 0;
    return true;
}


void CPlayerLevel::processStunned()
{
    mStunTimer++;

    if(mStunTimer < STUN_TIME)
    return;

    mStunTimer = 0;

    setAction(A_KEEN_STAND);
}


void CPlayerLevel::processMovingHorizontal()
{
    // Verify facing directions
    if(  mPlaycontrol[PA_X]<0  ) // left
        xDirection = LEFT;
    else if( mPlaycontrol[PA_X]>0  ) // right
        xDirection = RIGHT;

    moveXDir(mPlaycontrol[PA_X]>>1);
}


void CPlayerLevel::processPogoCommon()
{
    if (blockedl && xDirection == -1)
        xinertia = 0;
    if (blockedr && xDirection == 1)
        xinertia = 0;


    if (blockedu)
    {
        //TODO: Something to do with poles (push keen into the centre)
        if (blockedu == 17)	//Pole
        {
            //obj->posY -= 32;
            //obj->clipRects.unitY1 -= 32;
            xinertia = 0;
            //obj->posX = (obj->clipRects.tileXmid << 8) - 32;
        }
        else
        {
            playSound( GameSound::KEEN_BUMPHEAD );
            if (blockedu > 1)
            {
                yinertia += 16;
                if (yinertia < 0)
                    yinertia = 0;
            }
            else
            {
                if( yinertia < 0 )
                {
                    yinertia = 0;
                }
            }
            mActionState.jumpTimer = 0;
        }
    }



    // Houston, we've landed!
    if(m_Inventory.Item.m_special.mCanPogo)
    {
        if(blockedd)
        {
            //yinertia = 0; // Not sure if that's correct
            if (mActionState.jumpTimer == 0)
            {
                yinertia = evalVertPogoInertia();
                playSound( GameSound::KEEN_POGO );
                mActionState.jumpTimer = 24;
                setAction(A_KEEN_POGO_UP);
            }
        }
    }

    // Let's see if Keen breaks the fuse
    if(mpMap->mFuseInLevel)
    {
        int x = getXMidPos();
        int y = getYDownPos();
        const int tileID = mpMap->getPlaneDataAt(1, x, y);

        if(tileID == 0x078A)
        {
            const int t1 = mpMap->getPlaneDataAt(1, 0, 0);
            const int t2 = mpMap->getPlaneDataAt(1, 0, (1<<CSF));

            x >>= CSF; y >>= CSF;

            mpMap->setTile(x, y, t1, true);
            mpMap->setTile(x, y+1, t2, true);
            mpMap->mNumFuses--;
        }
    }
}


void CPlayerLevel::processPogoBounce()
{
    processPogoCommon();

    yinertia = evalVertPogoInertia();
    //nextY = 6 * yinertia;
    mActionState.jumpTimer = 24;

    if( !getActionStatus(A_KEEN_POGO_START) )
        setAction(A_KEEN_POGO_UP);
}



// Here all the pogo code is processed
void CPlayerLevel::processPogo()
{
    processPogoCommon();

    if (!mActionState.jumpTimer)
    {
        if (gBehaviorEngine.mDifficulty == EASY)
        {
            performGravityMid();
        }
        else
        {
            performGravityHigh();
        }
    }
    else
    {
        if (mActionState.jumpIsPressed || mActionState.jumpTimer <= 9)
            performGravityLow();
        else
            performGravityHigh();

        if (mActionState.jumpTimer < 2)
            mActionState.jumpTimer = 0;
        else
            mActionState.jumpTimer--;

        if (!mActionState.jumpTimer && m_Action.Next_action )
        {
            setAction(A_KEEN_POGO_HIGH);
        }
    }


    if (mPlaycontrol[PA_X])
    {
        //if (!xinertia)
        {
            if(mPlaycontrol[PA_X]<0)
                xDirection = -1;
            else if(mPlaycontrol[PA_X]>0)
                xDirection = 1;
        }
        //performPhysAccelHor(xDirection, 48); // This value was in omnispeak. 56 might fit better. Please report in case of problems!
        performPhysAccelHor(xDirection, 56);
    }
    else
    {
        //obj->nextX += obj->velX * CK_GetTicksPerFrame();
        if (xinertia < 0) xDirection = -1;
        else if (xinertia > 0) xDirection = 1;
    }



    std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();

    const bool ceilNearBy = TileProperty[mpMap->at((getXMidPos()>>CSF), (getYUpPos()>>CSF)-1)].bup;


    if (mActionState.pogoIsPressed && !mActionState.pogoWasPressed)
    {
        mActionState.pogoWasPressed = true;
        setAction(A_KEEN_FALL);

        if(blockedu || ceilNearBy) // This might fix some collision issues
        {
            yinertia = 0;
            blockedu = true;
        }
    }

    moveXDir(xinertia);

    // process Shooting in air
    if( mPlaycontrol[PA_FIRE] && !m_fire_recharge_time )
    {
        xinertia = 0;
        shootInAir();
        if(blockedu || ceilNearBy) // This might fix some collision issues
        {
            yinertia = 0;
            //moveDown(1<<CSF);
            blockedu = true;
        }

        return;
    }
}





void CPlayerLevel::verifyJumpAndFall()
{
    if ( blockedl && xDirection == -1)
    {
        xinertia = 0;
    }
    if ( blockedr && xDirection == 1)
    {
        xinertia = 0;
    }

    // Did we hit our head on the ceiling?
    if (blockedu)
    {
        //TODO: Something to do with poles (push keen into the centre)
        if (blockedu == 17)	//Pole
        {
            /*obj->posY -= 32;
            obj->clipRects.unitY1 -= 32;*/
            xinertia = 0;
            //obj->posX = (obj->clipRects.tileXmid << 8) - 32;
        }
        else if(!gBehaviorEngine.mCheatmode.jump)
        {
            playSound( GameSound::KEEN_BUMPHEAD );

            if (blockedu > 1)
            {
                yinertia += 16;
                if (yinertia < 0)
                    yinertia = 0;
            }
            else
            {
                if( yinertia<0 )
                    yinertia = 0;
            }
            mActionState.jumpTimer = 0;
            mMustHandleSlope = false;
        }
    }


    // If keen is jumping down, not because he did from an object like a platform,
    // but a tile where Keen can fall through, process this part of code and
    // check if Keen is still jumpinto through any object
    if(!pSupportedbyobject && m_jumpdown)
    {
        if(!canFallThroughTile())
            m_jumpdown = false;
    }

    // Have we landed?
    if (blockedd)
    {
        //obj->deltaPosY = 0;
        //TODO: Check if deadly.

        //TODO: Check if fuse.

        mMustHandleSlope = false;

        if (mActionState.jumpTimer == 0) // Or standing on a platform.
        {
            user1 = user2 = 0;	// Being on the ground is boring.
            yDirection = 0;
            playSound( GameSound::KEEN_LAND );

            //TODO: Finish these
            if( mPlaycontrol[PA_X] != 0 )
            {
                setAction(A_KEEN_RUN);
            }
            else
            {
                makeHimStand();
            }

        }
    }
    /*else if (deltaPosY > 0)
    {
        int temp6 = obj->clipRects.unitY1 - obj->deltaPosY;
        int temp8 = ((obj->clipRects.unitY1 - 64) & 0xFF00) + 64;
        int temp10 = (temp8 >> 8) -1;

        if (temp6 < temp8 && obj->clipRects.unitY1 >= temp8)
        {
            printf("Preparing to hang!\n");
            if (ck_inputFrame.xDirection == -1)
            {
                int tileX = obj->clipRects.tileX1 - ((obj->rightTI)?1:0);
                int tileY = temp10;
                int upperTile = CA_TileAtPos(tileX, tileY, 1);
                int lowerTile = CA_TileAtPos(tileX, tileY+1, 1);
                printf("[%d,%d]: RightTI: %d, UpperTile = %d [%d], LowerTile = %d, [%d]\n",tileX, tileY, obj->rightTI, upperTile, TI_ForeRight(upperTile), lowerTile, TI_ForeRight(lowerTile));
                if ( (!TI_ForeLeft(upperTile) && !TI_ForeRight(upperTile) && !TI_ForeTop(upperTile) && !TI_ForeBottom(upperTile)) &&
                    TI_ForeRight(lowerTile) && TI_ForeTop(lowerTile))
                {
                    obj->xDirection = -1;
                    obj->clipped = false;
                    obj->posX = (obj->posX & 0xFF00) + 128;
                    obj->posY = (temp8 - 64);
                    obj->velY = obj->deltaPosY = 0;
                    CK_SetAction2(obj, &CK_ACT_keenHang1);
                    printf("Hung left!\n");
                } else printf("Couldn't hang left!\n");
            }
            else if (ck_inputFrame.xDirection == 1)
            {
                int tileX = obj->clipRects.tileX2 + ((obj->leftTI)?1:0);
                int tileY = temp10;
                int upperTile = CA_TileAtPos(tileX, tileY, 1);
                int lowerTile = CA_TileAtPos(tileX, tileY+1, 1);

                if (!TI_ForeLeft(upperTile) && !TI_ForeRight(upperTile) && !TI_ForeTop(upperTile) && !TI_ForeBottom(upperTile) &&
                    TI_ForeLeft(lowerTile) && TI_ForeTop(lowerTile))
                {
                    obj->xDirection = 1;
                    obj->clipped = false;
                    obj->posX = (obj->posX & 0xFF00) + 256;
                    obj->posY = (temp8 - 64);
                    obj->velY = obj->deltaPosY = 0;
                    CK_SetAction2(obj, &CK_ACT_keenHang1);
                }
            }
        }
    }*/

    //RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, 0, obj->zLayer);
}


// Processes the jumping of the player
void CPlayerLevel::processJumping()
{
    if(checkandtriggerforCliffHanging())
        return;

    verifyJumpAndFall();
    if (mActionState.jumpTimer)
    {
        if(mActionState.jumpTimer > 0 && !gBehaviorEngine.mCheatmode.jump)
            mActionState.jumpTimer--;

        // Stop moving up if we've let go of control.
        if (!mActionState.jumpIsPressed)
        {
            mActionState.jumpTimer = 0;
            mMustHandleSlope = false;
        }

        moveYDir(yinertia);

    }
    else
    {
        if(gBehaviorEngine.mDifficulty == EASY)
        {
            performGravityMid();
        }
        else	// Normal or Hard
        {
            performGravityHigh();
        }

        if( yinertia >0 && !getActionStatus(A_KEEN_FALL) && !getActionStatus(A_KEEN_FALL+1)  )
        {
            m_Action.setNextActionFormat();
        }
    }

    //Move horizontally
    if ( mPlaycontrol[PA_X] != 0 )
    {
        xDirection = (mPlaycontrol[PA_X] < 0) ? -1 : 1;

        // Jump further if run is pressed
        if(mPlaycontrol[PA_RUN])
        {
            performPhysAccelHor(xDirection*5, 75);
        }
        else
        {
            performPhysAccelHor(xDirection*4, 56);
        }
    }
    else performPhysDampHorz();

    moveXDir(xinertia);

    if(m_Inventory.Item.m_special.mCanPogo)
    {
        if (mActionState.pogoIsPressed && !mActionState.pogoWasPressed)
        {
            mActionState.pogoWasPressed = true;
            setAction(A_KEEN_POGO_UP);
            mActionState.jumpTimer = 0;
            mMustHandleSlope = false;
            return;
        }
    }

    if (mPlaycontrol[PA_Y] < 0)
    {
        verifyforPole();
    }


    if( gBehaviorEngine.mCheatmode.jump && (mActionState.jumpIsPressed && !mActionState.jumpWasPressed) )
    {
        mActionState.jumpWasPressed = true;
        yinertia = -90;
        mActionState.jumpTimer = 18;
        mMustHandleSlope = true;
        setAction(A_KEEN_JUMP);
        return;
    }

    // process Shooting in air
    if( mPlaycontrol[PA_FIRE] && !m_fire_recharge_time )
    {
        shootInAir();
    }
}






bool CPlayerLevel::canFallThroughTile()
{
    const int tile = mpMap->getPlaneDataAt(1, getXMidPos(), getYDownPos()+(5<<STC));
    const CTileProperties &TileProp = gBehaviorEngine.getTileProperties(1)[tile];
    return ( TileProp.bdown == 0 && TileProp.bup != 0 );
}






void CPlayerLevel::processLookingUp()
{
    // While looking up, Keen could shoot up!
    if( mPlaycontrol[PA_FIRE] && !m_fire_recharge_time )
    {
        setActionForce(A_KEEN_SHOOT_UP);
        mReleasedShot = false;
        m_fire_recharge_time = FIRE_RECHARGE_TIME;
        return;
    }

    if( m_camera.mRelcam.y > -MAX_SCROLL_VIEW )
        m_camera.mRelcam.y -= (2<<STC);

    if( mPlaycontrol[PA_Y]<0 )
        return;

    makeHimStand();
    yDirection = 0;
}






#define		MISCFLAG_SWITCHPLATON 5
#define 	MISCFLAG_SWITCHPLATOFF 6
#define		MISCFLAG_SWITCHBRIDGE 15
#define		MISCFLAG_DOOR		2
#define		MISCFLAG_KEYCARDDOOR		32

bool CPlayerLevel::verifySwitches()
{
    std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);
    const int x_left = getXLeftPos();
    const int x_right = getXRightPos();
    const int x_mid = (x_left+x_right)/2;
    const int up_y = getYUpPos()+(3<<STC);

    Uint32 tile_no = mpMap->getPlaneDataAt(1, x_mid, up_y);
    int flag = Tile[tile_no].behaviour;

    // pressing a switch
    if (flag == MISCFLAG_SWITCHPLATON ||
        flag == MISCFLAG_SWITCHPLATOFF ||
        flag == MISCFLAG_SWITCHBRIDGE)
    {
      playSound( GameSound::GUN_CLICK );

      auto newtile = tile_no-1;

      if(flag == MISCFLAG_SWITCHBRIDGE  || flag == 18 )
      {
            PressBridgeSwitch(x_mid, up_y);
      }
      else
      {
            PressPlatformSwitch(x_mid, up_y);
      }


      if(Tile[tile_no+1].behaviour == flag ||
         Tile[tile_no+1].behaviour == MISCFLAG_SWITCHPLATON ||
         Tile[tile_no+1].behaviour == MISCFLAG_SWITCHPLATOFF )
      {
            newtile = tile_no+1;
      }

      mpMap->setTile( x_mid>>CSF, up_y>>CSF, newtile, true, 1); // Wrong tiles, those are for the info plane


      setAction(A_KEEN_SLIDE);
      m_timer = 0;
      return true;
    }

    return false;
}

void CPlayerLevel::processPressUp()
{
    // If a switch is being pressed
    if(verifySwitches())
    {
        return;
    }

    std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);
    const int x_left = (getXLeftPos()>>CSF)<<CSF;
    const int x_right = (getXRightPos()>>CSF)<<CSF;
    const int x_mid = (x_left+x_right)/2;
    const int up_y = getYUpPos()+(3<<STC);

    int entranceFlag = 0;

    if ( !m_EnterDoorAttempt )
    {
        /// Test if keen may enter a door.
        /// Note: They are usually larger
        const size_t tile_from_left  = mpMap->getPlaneDataAt(1, x_left, up_y);
        const size_t tile_from_right = mpMap->getPlaneDataAt(1, x_right, up_y);

        int flag_left  = Tile[tile_from_left].behaviour;
        int flag_right = Tile[tile_from_right].behaviour;

        auto tile_no = tile_from_right;

        bool entranceDetected = false;
        auto x_pos_door = x_mid;

        if ( flag_left == MISCFLAG_DOOR || flag_left == MISCFLAG_KEYCARDDOOR )
        {
            x_pos_door = x_left;
            tile_no = tile_from_left;
            entranceFlag = flag_left;
            entranceDetected = true;
        }

        if ( flag_right == MISCFLAG_DOOR || flag_right == MISCFLAG_KEYCARDDOOR )
        {
            x_pos_door = x_right;
            tile_no = tile_from_right;
            entranceFlag = flag_right;
            entranceDetected = true;
        }

        const int info = mpMap->getPlaneDataAt(2, x_pos_door, up_y);


        if(entranceDetected)
        {
            int x_entr_left  = x_pos_door;
            int x_entr_right = x_pos_door+(1<<CSF);

            // Calculate entrace width so player will be centered appropiately
            for(int i=0 ; i<10 ; i++)
            {
                const size_t entr_tile  = mpMap->getPlaneDataAt(1, x_pos_door+(i<<CSF), up_y);
                int entr_flag  = Tile[entr_tile].behaviour;

                if(entr_flag == MISCFLAG_DOOR || entr_flag == MISCFLAG_KEYCARDDOOR )
                    x_entr_right = x_pos_door+((i+1)<<CSF);
                else
                    break;
            }
            for(int i=0 ; i<10 ; i++)
            {
                const size_t entr_tile  = mpMap->getPlaneDataAt(1, x_pos_door-(i<<CSF), up_y);
                int entr_flag  = Tile[entr_tile].behaviour;

                if(entr_flag == MISCFLAG_DOOR || entr_flag == MISCFLAG_KEYCARDDOOR )
                    x_entr_left = x_pos_door-(i<<CSF);
                else
                    break;
            }

            x_pos_door = (x_entr_right+x_entr_left) / 2;

            if (entranceFlag == MISCFLAG_KEYCARDDOOR)
            {
                if (m_Inventory.Item.m_keycards)
                {
                    m_Inventory.Item.m_keycards = 0;
                    playSound(GameSound::OPEN_EXIT_DOOR);
                    //GetNewObj(0);
                    //new_object->xpos = o->boxTXmid-2;
                    //new_object->ypos = o->boxTY2-4;
                    //new_object->active = 2;
                    //new_object->clipping = 0;
                    //new_object->type = 1;
                    //new_object->action = ACTION_SECURITYDOOROPEN;
                    //check_ground(new_object, ACTION_SECURITYDOOROPEN);
                    //o->action = ACTION_KEENENTERDOOR0;
                    //o->int16 = 0;
                    spawnObj( new CSecurityDoor(getMapPtr(), 0, x_pos_door-(1<<CSF), up_y-(3<<CSF) ) );

                    mTarget = getPosition();
                    mTarget.y -= (1<<CSF);

                    setAction(A_KEEN_ENTER_DOOR);

                    setActionSprite();
                    GsSprite &rSprite = gGraphics.getSprite(mSprVar, mSpriteIdx);
                    const auto xPosCenteredKeen = x_pos_door - ((rSprite.getWidth()<<STC)/2);

                    //moveToHorizontal(xPosCenteredKeen);
                    mTarget.x = xPosCenteredKeen;

                    mExitDoorTimer = 110;

                    m_EnterDoorAttempt = true;
                    return;
                }
                else
                {
                    playSound(GameSound::CANT_DO);
                    //SD_PlaySound(GameSound::NOOPENSECURITYDOOR);
                    setAction(A_KEEN_STAND);
                    m_EnterDoorAttempt = true;
                    return;
                }
            }
            else
            {
                mTarget = getPosition();

                // Illusion for going into the background does not apply on teleporters
                if(info || tile_no != 0x401)
                {
                    mTarget.y -= (1<<CSF);
                }

                solid = false;

                setAction(A_KEEN_ENTER_DOOR);

                setActionSprite();
                GsSprite &rSprite = gGraphics.getSprite(mSprVar, mSpriteIdx);
                const auto sprHalfWidth = ((rSprite.getWidth()<<STC)/2);
                const auto xPosCenteredKeen = x_pos_door - sprHalfWidth;

                //moveToHorizontal(xPosCenteredKeen);
                mTarget.x = xPosCenteredKeen;

                m_EnterDoorAttempt = true;

                return;
            }
        }
    }

    if(verifyforPole())
    {
       return;
    }

    // If the above did not happen, then just look up
    setAction(A_KEEN_LOOKUP);

    return;
}













void CPlayerLevel::processSliding()
{
    if(mPlacingGem)
    {
        if(m_timer == 0 || m_timer == 5 || m_timer == 8)
        {
            openDoorsTile();
        }
    }

    if(m_timer < 10)
    {
        m_timer++;
        return;
    }


    m_timer = 0;

    if(mPlacingGem)
    {
        const auto lx1 = getXLeftPos();
        const auto lx2 = getXRightPos();
        const auto ly1 = getYUpPos();
        const auto ly2 = getYDownPos();

        std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);

        bool foundHolder = false;

        for(auto x = lx1 ; x<=lx2 ; x += (1<<CSF) )
        {
            for(auto y = ly1 ; y<=ly2 ; y += (1<<CSF) )
            {
                Uint32 tile_no = mpMap->getPlaneDataAt(1, x, y);
                int flag = Tile[tile_no].behaviour;

                if(flag >= 7 && flag <= 10)
                {
                    mpMap->setTile(x>>CSF, y>>CSF, tile_no+18, true, 1);
                    mPlacingGem = false;
                    playSound( GameSound::DOOR_OPEN );

                    foundHolder = true;
                    break;
                }

            }

            if(foundHolder) break;
        }
    }

    makeHimStand();
}

void CPlayerLevel::processEnterDoor()
{
    playWalkSound();

    if( getActionStatus(A_KEEN_STAND) )
        dontdraw = true;

    int xdiff = m_Pos.x-mTarget.x;

    const int enterDoorSpeedX = 64;
    const int enterDoorSpeedY = 16;

    if( xdiff > enterDoorSpeedX )
    {
        moveLeft(enterDoorSpeedX);
        return;
    }
    if( xdiff < -enterDoorSpeedX )
    {
        moveRight(enterDoorSpeedX);
        return;
    }

    // This happens in Keen 5 when Players enters the exit door and it still has to open.
    if(mExitDoorTimer > 0)
    {
        mExitDoorTimer--;
        return;
    }

    moveUp(enterDoorSpeedY);

    if( m_Pos.y-mTarget.y > enterDoorSpeedY )
        return;

    moveToForce(mTarget);

    yDirection = 0;
    setAction(A_KEEN_STAND);
    solid = true;

    dontdraw = false;

    int xmid = getXMidPos();
    int y1 = getYDownPos();

    Uint32 t = mpMap->getPlaneDataAt(2, xmid, y1);

    for(int x = -(2<<CSF) ; x<=(2<<CSF) ; x+=(1<<CSF))
    {
        if(t==0)
            t = mpMap->getPlaneDataAt(2, xmid+x, y1-(1<<CSF));

        if(t==0)
            t = mpMap->getPlaneDataAt(2, xmid+x, y1-(2<<CSF));

        if(t==0)
            t = mpMap->getPlaneDataAt(2, xmid+x, y1-(3<<CSF));
    }

    if (t == 0)
    {
        bool mustTeleportOnMap = false;

        auto &frontTileProperties = gBehaviorEngine.getTileProperties(1);

        // Check if there is a teleporter. In Keen 5 there might be one!
        if(gBehaviorEngine.getEpisode() == 5)
        {
            Uint32 teletile = mpMap->getPlaneDataAt(1, xmid, y1);

            if(teletile==0)
                teletile = mpMap->getPlaneDataAt(1, xmid, y1-(1<<CSF));

            if(teletile==0)
                teletile = mpMap->getPlaneDataAt(1, xmid, y1-(2<<CSF));

            if(teletile==0)
                teletile = mpMap->getPlaneDataAt(1, xmid, y1-(3<<CSF));

            // Code for the teleport tile
            //if(teletile == 0x0401)
            const auto &tile = frontTileProperties[teletile];

            // Teleport to the secret level
            if(tile.behaviour == 0x02)
            {
                // There is one!
                mustTeleportOnMap = true;
            }

        }

        //level_state = 13;
        //o->action = ACTION_KEENENTEREDDOOR;
        // TODO: Figure out what this does
        gMusicPlayer.stop();

        gEffectController.setupEffect(new CDimDark(8));

        auto evExit = new EventExitLevel(mpMap->getLevel(), true,
                                         mustTeleportOnMap, mPlayerIdx);
        evExit->playSound = true;
        gEventManager.add( evExit );

        dontdraw = true;
        m_Inventory.Item.m_gem.clear();
        return;
    }

    if (t == 0xB1B1)
    {
        //level_state = 2;
        //o->action = ACTION_KEENENTEREDDOOR;
        gEffectController.setupEffect(new CDimDark(8));
        gMusicPlayer.stop();

        auto evExit = new EventExitLevel(mpMap->getLevel(), true, false, mPlayerIdx);
        evExit->playSound = true;
        gEventManager.add( evExit );


        dontdraw = true;
        m_Inventory.Item.m_gem.clear();
        return;
    }

    const int ypos = (t &  0xFF)<<CSF;
    const int xpos = (t >> 0x08)<<CSF;

    GsVec2D<int> new_pos(xpos, ypos);
    moveToForce(new_pos);
    moveOtherPlayersTo(new_pos);

    new_pos.x += ((m_BBox.x2-m_BBox.x1)/2);
    new_pos.y += ((m_BBox.y2/*-m_BBox.y1*/)/2);

    mpMap->mGamePlayPos = new_pos;
    m_camera.setPosition(new_pos);

    //o->ypos = TILE2MU(*t%256 - 1) + 15;
    //o->xpos = (*t >> 8 << 8);
    //o->int16 = 1;
    //o->clipping = 0;
    //set_sprite_action(o, (o->action)->next);
    //o->clipping = 1;
    //sub_183F1(o);
    return;
}

void CPlayerLevel::toggleBridge(const Uint32 newX, const Uint32 newY)
{
    const int start_tile = mpMap->getPlaneDataAt(1, newX<<CSF, newY<<CSF)-1;

    // This is usual for Keen 4. The last tile comes three tiles later
    int end_tile = start_tile+3;

    const int ep = gBehaviorEngine.getEpisode();
    std::vector<CTileProperties> &tileProp = gBehaviorEngine.getTileProperties(1);

    int x = newX;

    if(ep > 4) // In keen 5 and 6 if there is no bridge property found anymore we found end_tile
    {
        for(int t = start_tile+1 ;  ; x++ )
        {
            t = mpMap->getPlaneDataAt(1, x<<CSF, newY<<CSF);

            if(tileProp[t].behaviour != 18)
            {
                end_tile = mpMap->getPlaneDataAt(1, (x-1)<<CSF, newY<<CSF);;
                break;
            }
        }
    }


    if(mLua)
    {
        int value = 0;
        mLua.runFunctionRetOneInt("defineEndBridgeTile", value);
        end_tile = start_tile+value;
    }

    x = newX-1;

    for(int t = start_tile ; mpMap->m_width  ; x++ )
    {
        // Now decide whether the tile is a piece or borders of the bridge
        t = mpMap->getPlaneDataAt(1, x<<CSF, newY<<CSF);

        if(t == 0)
            break;

        if(t == end_tile)
            break;

        // We have two rows
        for(int y = newY ; y<int(newY+2) ; y++)
        {
            t = mpMap->getPlaneDataAt(1, x<<CSF, y<<CSF);
            const auto NewTile = t+tileProp[t].nextTile;

            mpMap->setTile(x, y, NewTile, true, 1);
        }

    }

    // Last column
    for(int y = newY ; y<int(newY+2) ; y++)
    {
        int t = mpMap->getPlaneDataAt(1, x<<CSF, y<<CSF);
        const auto NewTile = t+tileProp[t].nextTile;

        mpMap->setTile(x, y, NewTile, true, 1);
    }

}




/**
 * This function checks whether a bridge must be opened or closed and does this kind of work
 * I'm not really happy with that part of the code and I know that it works for Keen 4. Not sure about the
 * other episodes, but it's well though and should...
 */

void CPlayerLevel::PressBridgeSwitch(const Uint32 lx, const Uint32 ly)
{
    Uint32 targetXY = mpMap->getPlaneDataAt(2, lx, ly);

    Uint32 startX = targetXY >> 8;
    Uint32 startY = targetXY & 0xFF;

    toggleBridge(startX, startY);
}




/**
 * This function will put/release the blockers of some platforms used in Keen Galaxy
 */
void CPlayerLevel::PressPlatformSwitch(const Uint32 lx, const Uint32 ly)
{
  Uint32 targetXY = mpMap->getPlaneDataAt(2, lx, ly);

  Uint32 newX = targetXY >> 8;
  Uint32 newY = targetXY & 0xFF;

  const int oldInfoTile = mpMap->getPlaneDataAt(2, newX<<CSF, newY<<CSF);
  int newTile = 0x0;

  if(oldInfoTile == 31) // 31 stands for the plat blocker
  {
    newTile = 0;
  }
  else if(oldInfoTile >= 0x5B && oldInfoTile <= 0x62 )
  {
    switch( oldInfoTile )
    {
      case 0x5B:
    newTile = 0x5D; break;
      case 0x5C:
    newTile = 0x5E; break;
      case 0x5D:
    newTile = 0x5B; break;
      case 0x5E:
    newTile = 0x5C; break;
      case 0x5F:
    newTile = 0x61; break;
      case 0x60:
    newTile = 0x62; break;
      case 0x61:
    newTile = 0x5F; break;
      case 0x62:
    newTile = 0x60; break;
      default:
    break;
    }
  }
  else if(oldInfoTile == 0)
  {
    newTile = 31;
  }

  mpMap->setInfoTile({newX, newY}, newTile);

  return;
}



void CPlayerLevel::exchangeZapper(const int mapx, const int mapy, const int offset, const int stopZapperTile)
{
    // Exchange the tiles until stopTile makes the operation cancelling
    int nextTile = 0;
    int x = mapx;
    int y = mapy;
    while( nextTile != stopZapperTile )
    {
      const int curTile = mpMap->at(x,y);
      nextTile = curTile + offset;
      mpMap->setTile(x, y, nextTile, true);
      y++;
    }
}



void CPlayerLevel::disableZapper(const Uint32 lx, const Uint32 ly)
{
    // Find the inactive zapper tile, if you don't find it, cancel the operation!
    int startZapTile = mpMap->at(lx, ly);
    int iZapperTile = startZapTile;
    for( ; iZapperTile < startZapTile+4 ; iZapperTile++ )
    {
      if(iZapperTile == 0xA6B) // The inactive Zapper
    break;
    }

    if(iZapperTile >= startZapTile+4) // Did not find that zapper. I have to quit sorry!
      return;

    // Get the tile where exchanging stops. It's just 3 rows down the tilemap. The width of the tilemap is 18!
    const int stopZapperTile = iZapperTile+2*18;

    // Disable all the zapping vertically until the stopping zap tile is exchanged being the last one
    const int offset = iZapperTile-startZapTile;
    exchangeZapper(lx, ly, offset, stopZapperTile);
}


void CPlayerLevel::enableZapper(const Uint32 lx, const Uint32 ly)
{
    // Find the active zapper tile, if you don't find it, cancel the operation!
    int startZapTile = mpMap->at(lx, ly);
    int iZapperTile = startZapTile;
    for( ; iZapperTile > startZapTile-4 ; iZapperTile-- )
    {
      if(iZapperTile == 0xA68) // The inactive Zapper
    break;
    }

    if(iZapperTile <= startZapTile-4) // Did not find that zapper. I have to quit sorry!
      return;

    // Get the tile where exchanging stops. It's just 3 rows down the tilemap. The width of the tilemap is 18!
    const int stopZapperTile = iZapperTile+2*18;

    // Disable all the zapping vertically until the stopping zap tile is exchanged being the last one
    const int offset = iZapperTile-startZapTile;
    exchangeZapper(lx, ly, offset, stopZapperTile);
}


void CPlayerLevel::openDoorsTile()
{
    int lx = getXMidPos();
    int ly = getYDownPos()-(3<<STC);
    Uint32 targetXY = mpMap->getPlaneDataAt(2, lx, ly);

    Uint32 newX = targetXY >> 8;
    Uint32 newY = targetXY & 0xFF;
    Uint32 tileno, next_tileno;

    while(1)
    {
        tileno = mpMap->getPlaneDataAt(1, newX<<CSF, newY<<CSF);
        mpMap->setTile(newX, newY, tileno+1, true, 1);
        newY++;
        next_tileno = mpMap->getPlaneDataAt(1, newX<<CSF, newY<<CSF);

        //const int freeTileno = mpMap->getPlaneDataAt(1, (newX+1)<<CSF, newY<<CSF);
        //const bool isBlock = tilePropVec[freeTileno].bup;

        /*if( isBlock ) // Freeshot for some doors, which might have strange
            continue;   // tiles setup. v.g. Keen 7 Krodacia        */ // Disabled, because not sure how valid this is.
        // It would break some doors in Keen 6.

        if( next_tileno != tileno && next_tileno != tileno+18 && next_tileno != tileno+2*18)
            break;
    }
}




/*

void CK_KeenHangThink(CK_object *obj)
{
    printf("Just hangin'...\n");
    if (ck_inputFrame.yDirection == -1 || ck_inputFrame.xDirection == obj->xDirection)
    {
        printf("Goin' up!\n");
        obj->currentAction = &CK_ACT_keenPull1;

        obj->clipped = false;

        if(obj->xDirection == 1)
        {
            obj->nextY = -256;
        }
        else
        {
            obj->nextY = -128;
        }
        //TODO: Set keen->zlayer 3

        //if (obj->xDirection == 1)
        //{


    }
    else if (ck_inputFrame.yDirection == 1 || (ck_inputFrame.xDirection && ck_inputFrame.xDirection != obj->xDirection))
    {
        printf("Goin' Down!\n");
        // Drop down.
        obj->currentAction = &CK_ACT_keenFall1;
        obj->clipped = true;
    }
}

void CK_KeenPullThink1(CK_object *obj)
{
    if (obj->xDirection == 1)
        obj->nextX = 128;
    else
        obj->nextY = -128;
}

void CK_KeenPullThink2(CK_object *obj)
{
    printf("PullThink2\n");
    obj->nextX = obj->xDirection * 128;
    obj->nextY = -128;
}

void CK_KeenPullThink3(CK_object *obj)
{
    printf("PullThink3\n");
    obj->nextY = -128;
}

void CK_KeenPullThink4(CK_object *obj)
{
    printf("PullThink4\n");
    obj->nextY = 128;
    obj->clipped = true;
    obj->zLayer = 1;
}

 */


void CPlayerLevel::performPoleHandleInput()
{
    const int px = mPlaycontrol[PA_X];
    const int py = mPlaycontrol[PA_Y];

    if ( px )
    {
        xDirection = (px>0) ? 1 : -1;
    }

    // Shooting things. *ZAP!*
    if( py < 0 )
    {
        // First check player pressed shoot button
        if( mPlaycontrol[PA_FIRE] && !m_fire_recharge_time )
        {
            m_fire_recharge_time = FIRE_RECHARGE_TIME;
            setActionForce(A_KEEN_POLE_SHOOTUP);
            const GsVec2D<int> newVec(getXMidPos()-(3<<STC), getYUpPos()-(16<<STC));
            tryToShoot(newVec, 0, -1);
            return;
        }

    }
    else if( py > 0 )
    {
        // First check player pressed shoot button
        if( mPlaycontrol[PA_FIRE] && !m_fire_recharge_time )
        {
            m_fire_recharge_time = FIRE_RECHARGE_TIME;
            setActionForce(A_KEEN_POLE_SHOOTDOWN);
            const GsVec2D<int> newVec(getXMidPos()-(3<<STC), getYDownPos());
            tryToShoot(newVec, 0, 1);
            return;
        }
    }
    else if(!m_fire_recharge_time)
    {
        // First check player pressed shoot button
        if( mPlaycontrol[PA_FIRE] )
        {
            m_fire_recharge_time = FIRE_RECHARGE_TIME;
            setActionForce(A_KEEN_POLE_SHOOT);
            const GsVec2D<int> newVec(getXPosition(),
                                        getYPosition()+(4<<STC));
            tryToShoot(newVec, xDirection, yDirection);
            return;
        }

        setAction(A_KEEN_POLE);
        yDirection = 0;
    }


    if (mActionState.jumpIsPressed && !mActionState.jumpWasPressed)
    {
        mActionState.jumpWasPressed = false;

        if ( px )
            xinertia = 8*xDirection;
        else
            xinertia = 0;

        yinertia = -80;
        mActionState.jumpTimer = 10;
        mMustHandleSlope = true;
        setAction(A_KEEN_JUMP);
        playSound( GameSound::KEEN_JUMP );
        yDirection = 1;
        mIsClimbing = false;
        m_jumped = true;
        mPoleGrabTime = 0;
        solid = true;
        mActionState.poleGrabTime = 0;
    }

    return;
}


void CPlayerLevel::processPoleClimbingSit()
{
    const int px = mPlaycontrol[PA_X];
    const int py = mPlaycontrol[PA_Y];

    Uint32 l_x_l = getXLeftPos();
    Uint32 l_x = getXMidPos();
    Uint32 l_x_r = getXRightPos();
    Uint32 l_y_up = getYUpPos()-(2<<STC);
    Uint32 l_y_down = getYDownPos();


    if ( py > 0 ) // pressed down
    {
        setAction(A_KEEN_POLE_SLIDE);
        yDirection = 1;
        processPoleSlidingDown();
        return;
    }
    else if ( py < 0 ) // pressed up
    {

        // Check for the upper side and don't let him move if the pole ends
        if( hitdetectWithTileProperty(1, l_x_l, l_y_up) ||
            hitdetectWithTileProperty(1, l_x, l_y_up) ||
            hitdetectWithTileProperty(1, l_x_r, l_y_up) )
        {
            setAction(A_KEEN_POLE_CLIMB);
            yDirection = UP;
        }
        else
        {
            setAction(A_KEEN_POLE);
            yDirection = 0;
        }

        return;
    }


    if ( px )
    {
        // This will check three points and avoid that keen falls on sloped tiles
        const int fall1 = mpMap->getPlaneDataAt(1, l_x, l_y_down+(1<<CSF));
        const CTileProperties &TileProp1 = gBehaviorEngine.getTileProperties(1)[fall1];
        const bool leavePole = (TileProp1.bup != 0);

        if ( leavePole )
        {
            mActionState.jumpWasPressed = false;
            mActionState.jumpIsPressed = true;

            int dir = 0;
            if(px < 0)
                dir = -1;
            else if(px > 0)
                dir = 1;

            xinertia = 8*dir;
            yinertia = -20;
            mActionState.jumpTimer = 10;
            mMustHandleSlope = false;
            solid = true;
            setAction(A_KEEN_JUMP);
            yDirection = 1;
            mIsClimbing = false;
            m_jumped = true;
            mPoleGrabTime = 0;
            mActionState.poleGrabTime = 0;

            return;
        }

    }

    performPoleHandleInput();
}



void CPlayerLevel::processPoleClimbingUp()
{
    Uint32 l_x_l = getXLeftPos();
    Uint32 l_x = getXMidPos();
    Uint32 l_x_r = getXRightPos();
    Uint32 l_y_up = getYUpPos()-(2<<STC);

    // Check for the upper side and don't let him move if the pole ends
    if( hitdetectWithTileProperty(1, l_x_l, l_y_up) ||
        hitdetectWithTileProperty(1, l_x, l_y_up) ||
        hitdetectWithTileProperty(1, l_x_r, l_y_up) )
    {
        setAction(A_KEEN_POLE_CLIMB);
        yDirection = UP;
    }
    else
    {
        setAction(A_KEEN_POLE);
        yDirection = 0;
    }

    performPoleHandleInput();
}



void CPlayerLevel::processPoleSlidingDown()
{
    auto l_x_l = getXLeftPos();
    auto l_x = getXMidPos();
    auto l_x_r = getXRightPos();
    auto l_y_up = getYUpPos()+(16<<STC);
    auto l_y_down = getYDownPos()+(16<<STC);

    // Check for the upper and lower side collision with the pole element.

    bool lower = false;

    lower |= hitdetectWithTileProperty(1, l_x_l, l_y_down);
    lower |= hitdetectWithTileProperty(1, l_x, l_y_down);
    lower |= hitdetectWithTileProperty(1, l_x_r, l_y_down);

    if(!lower)
    {
        solid = true;
    }
    else
    {
        solid = false;
    }

    // upper must be considered because the hand can touch the edge in that case
    bool upper = false;
    upper |= hitdetectWithTileProperty(1, l_x_l, l_y_up);
    upper |= hitdetectWithTileProperty(1, l_x, l_y_up);
    upper |= hitdetectWithTileProperty(1, l_x_r, l_y_up);
    if( upper && !blockedd )
    {
        // Slide down if there is more of the pole
        setAction(A_KEEN_POLE_SLIDE);
        yDirection = DOWN;
    }
    else
    {
        // Fall down if there isn't any pole to slide down
        mIsClimbing = false;
        yDirection = 0;
        yinertia = 0;
        solid = true;

        bool down = false;
        down |= mpMap->at(l_x_l>>CSF, l_y_down>>CSF);
        down |= mpMap->at(l_x>>CSF, l_y_down>>CSF);
        down |= mpMap->at(l_x_r>>CSF, l_y_down>>CSF);


        // Check if keen is trying to climb through the floor.
        // It's quite a strange clipping error if he succeeds.
        if( !blockedd && !down )
        {
            mActionState.jumpTimer = 0;
            mMustHandleSlope = false;
            xinertia = 8*xDirection;
            yinertia = 0;

            setAction(A_KEEN_FALL);
            playSound( GameSound::KEEN_FALL );
        }
        else
        {
            blockedd = true;
            moveUp(1<<CSF);
            moveDown(1<<CSF);

            makeHimStand();
            yDirection = 0;

            /*int yReset = -(obj->clipRects.unitY2 & 255) - 1;
            obj->posY += yReset;
            obj->clipRects.unitY2 += yReset;
            obj->clipRects.tileY2 += -1;
            obj->clipped = true;*/
            setAction(A_KEEN_LOOKDOWN);
            //CK_SetAction2(obj, &CK_ACT_keenLookDown1);

        }

        return;
    }

    if (mPlaycontrol[PA_Y] == 0)
    {
        setAction(A_KEEN_POLE);
        yDirection = 0;
    }
    else if (mPlaycontrol[PA_Y] < 0)
    {
        setAction(A_KEEN_POLE_CLIMB);
        yDirection = -1;
    }

    performPoleHandleInput();

}



void CPlayerLevel::processShootWhileStanding()
{
    if(!mReleasedShot)
    {
        if( getActionNumber(A_KEEN_SHOOT) && !getActionStatus(A_KEEN_SHOOT) )
        {
            int newx = getXPosition();

            if(xDirection > 0)
                newx = getXRightPos();
            else
                newx = getXLeftPos()-(1<<CSF);

            const int newy = getYPosition()+(6<<STC);
            const GsVec2D<int> newVec(newx, newy);
            tryToShoot(newVec, xDirection, yDirection);
        }

        if( getActionNumber(A_KEEN_SHOOT_UP) && !getActionStatus(A_KEEN_SHOOT_UP) )
        {
            const GsVec2D<int> newVec(getXMidPos()-(3<<STC), getYUpPos()-(16<<STC));
            tryToShoot(newVec, 0, -1);
        }
    }
    else
    {
        // wait until player releases the button and get back to stand status
        if( !mPlaycontrol[PA_FIRE] || gInput.AutoGun(mPlayerIdx) )
        {
            yDirection = 0;
            if(getActionNumber(A_KEEN_SHOOT_UP))
            {
                setAction(A_KEEN_LOOKUP);
            }
            else
            {
                makeHimStand();
            }
        }
    }
}

void CPlayerLevel::verifyFalling()
{
    if (CSpriteObject::verifyForFalling())
    {
        xinertia = xDirection * 16;
        yinertia = 0;
        setAction(A_KEEN_FALL);
        playSound( GameSound::KEEN_FALL );
        mActionState.jumpTimer = 0;
        mMustHandleSlope = false;
    }
}




// Falling code
void CPlayerLevel::processFalling()
{
    // Check Keen could hang on a cliff and do so if possible
    if(checkandtriggerforCliffHanging())
        return;


    // If keen is jumping down, not because he did from an object like a platform,
    // but a tile where Keen can fall through, process this part of code and
    // check if Keen is still jumpinto through any object
    if(!pSupportedbyobject && m_jumpdown)
    {
        if(!canFallThroughTile())
            m_jumpdown = false;
    }

    if(blockedd)
    {
        playSound( GameSound::KEEN_LAND );
        makeHimStand();
        yDirection = 0;
    }

    processMovingHorizontal();

    // If Jump mode is enabled he can jump again
    // This will cancel the pole process and make Keen jump
    if( gBehaviorEngine.mCheatmode.jump && mPlaycontrol[PA_JUMP] > 0 )
    {
        setAction(A_KEEN_JUMP);
        mIsClimbing = false;
        m_jumped = true;
        yinertia = 0;
        yDirection = 0;
        return;
    }

    if(m_Inventory.Item.m_special.mCanPogo)
    {

        /// While falling Keen could switch to pogo again anytime
        // but first the player must release the pogo button
        if( !mPlaycontrol[PA_POGO] )
            m_pogotoggle = false;

        // Now we can check if player wants to use it again
        if( !m_pogotoggle && mPlaycontrol[PA_POGO] )
        {
            m_jumpheight = 0;
            yinertia = 0;
            setAction(A_KEEN_POGO_START);
            m_pogotoggle = true;
        }
    }

    // Check if keen should stick to the pole
    if( mPlaycontrol[PA_Y] < 0 )
    {
        verifyforPole();
    }

    if( mPlaycontrol[PA_FIRE] && !m_fire_recharge_time )
        shootInAir();
}



void CPlayerLevel::centerView()
{
    // If keen looked up or down, this will return the camera to initial position
    if( m_camera.mRelcam.y < 0 )
    {
        m_camera.mRelcam.y += (4<<STC);
        if( m_camera.mRelcam.y > 0 )
            m_camera.mRelcam.y = 0;
    }
    else if( m_camera.mRelcam.y > 0 )
    {
        m_camera.mRelcam.y -= (4<<STC);
        if( m_camera.mRelcam.y < 0 )
            m_camera.mRelcam.y = 0;
    }
}

void CPlayerLevel::push(CGalaxySpriteObject& theObject)
{
  if(dying ||
     m_EnterDoorAttempt ||
     getActionStatus(A_KEEN_SLIDE) ||
     getActionStatus(A_KEEN_ENTER_DOOR) )
    return;

  CPlayerBase::push(theObject);

  if(onPole())
  {
    mIsClimbing = false;
    yDirection = 0;
    yinertia = 0;
    solid = true;
    setAction(A_KEEN_FALL);
  }
}


int CPlayerLevel::checkConveyorBelt()
{
    auto l_x_l = getXLeftPos();
    auto l_x = getXMidPos();
    auto l_x_r = getXRightPos();
    auto l_y_down = getYDownPos()+(1<<STC);

    int tileID1 = mpMap->getPlaneDataAt(1, l_x_l, l_y_down);
    int tileID2 = mpMap->getPlaneDataAt(1, l_x, l_y_down);
    int tileID3 = mpMap->getPlaneDataAt(1, l_x_r, l_y_down);

    std::vector<CTileProperties> &tileProp = gBehaviorEngine.getTileProperties(1);

    const CTileProperties &TileProp1 = tileProp[tileID1];
    const CTileProperties &TileProp2 = tileProp[tileID2];
    const CTileProperties &TileProp3 = tileProp[tileID3];


    if( TileProp1.bup == 0x29 || TileProp2.bup == 0x29 || TileProp3.bup == 0x29)
    {
        return RIGHT;
    }
    else if( TileProp1.bup == 0x31 || TileProp2.bup == 0x31 || TileProp3.bup == 0x31)
    {
        return LEFT;
    }

    return 0;
}



void CPlayerLevel::process()
{
    if(mDying)
    {
        setActionSprite();
    }

    if(mIsDead) // If this player is dead, the camera still runs,
    {       // in case a second player inherits the camera lead
        processCamera();
        return;
    }

    if(mExitTouched)
    {
        return;
    }

    if(!mDying)
    {
        processInput();

        // If no clipping was triggered change solid state of keen
        noclipping = gBehaviorEngine.mCheatmode.noclipping;

        if(pSupportedbyobject)
        {
            blockedd = true;
        }


        if ( mPoleGrabTime < MAX_POLE_GRAB_TIME )
        {
            mPoleGrabTime++;
        }

        (this->*mp_processState)();
    }
    else // Otherwise keen is dying
    {
        // Might happen if dopefish eats keen
        if(mp_processState == &CPlayerBase::processGetEaten)
        {
            processGetEaten();
        }
        else
        {
            processDying();
        }
    }


    if(mpMap->mFuseInLevel && mpMap->mNumFuses == 0)
    {
        // TODO: Need to spawn other messages here!
        gMusicPlayer.stop();

        m_Inventory.Item.m_gem.clear();
        m_Inventory.Item.fuse_levels_completed++;
        mpMap->mFuseInLevel = false;

        std::vector<CMessageBoxGalaxy*> msgs;

        bool specialLevel = false;

        const auto level = mpMap->getLevel();

        if(level == 13)
        {
            specialLevel = true;
            m_Inventory.Item.fuse_level_secret_completed = true;
        }

        const std::string fuse_msg = gBehaviorEngine.getString( (specialLevel) ? "FUSE_WONDER" : "FUSE_CASUAL");

        playSound( GameSound::FUSE_BREAK, SoundPlayMode::PLAY_PAUSEALL );

        gEffectController.setupEffect(new CDimDark(8));

        auto evExit = new EventExitLevel(level, true, false, mPlayerIdx);
        evExit->playSound = true;


        msgs.push_back( new CMessageBoxBitmapGalaxy( mSprVar,
                            fuse_msg,
                            *gGraphics.getBitmapFromStr(mSprVar, "KEENTHUMBSUP"),
                            LEFT,
                            false,
                            evExit) );

        showMsgVec( msgs );

        return;
    }


    // Make the fire recharge time decreased if player
    // is not pressing firing button
    // or AutoGun feature is enabled
    if(m_fire_recharge_time)
    {
        const auto autofire = gInput.AutoGun(mPlayerIdx);
        if(!mPlaycontrol[PA_FIRE] || autofire)
        {
            m_fire_recharge_time--;
        }
    }

    processLevelMiscFlagsCheck();

    if(mDying)
    {
        return;
    }

    if(!mExitDoorTimer)
    {
        // Run very fast
        if(mPlaycontrol[PA_RUN])
        {
            if(xDirection == LEFT )
                moveLeft( m_Action.h_anim_move<<1 );
            else if(xDirection == RIGHT )
                moveRight( m_Action.h_anim_move<<1 );
        }


        if(!processActionRoutine())
        {
            exists = false;
        }
    }


    if(!mDying && !mExitTouched)
    {
        processExiting();

        processCamera();

        performCollisions();

        void (CPlayerBase::*PogoProcess)() = static_cast<void (CPlayerBase::*)()>(&CPlayerLevel::processPogo);
        void (CPlayerBase::*FallProcess)() = static_cast<void (CPlayerBase::*)()>(&CPlayerLevel::processFalling);

        // It's not always desired to push out
        if( (mp_processState != PogoProcess) &&
            (mp_processState != FallProcess) )
        {
            processPushOutCollision();
        }
    }


    // Conveyor Belt in Keen 6 (but can work in 4 and 5)
    const auto conveyorBeltDir = checkConveyorBelt();
    if(conveyorBeltDir)
    {
        moveXDir(conveyorBeltDir*BELT_SPEED);
    }
}

bool CPlayerLevel::verifyAndToggleZapper(const int lx, const int ly)
{
    Uint32 targetXY = mpMap->getPlaneDataAt(2, lx, ly);

    Uint32 newX = targetXY >> 8;
    Uint32 newY = targetXY & 0xFF;

    const int zapperTile = mpMap->getPlaneDataAt(1, newX<<CSF, newY<<CSF);

    if(zapperTile >= 0xA68 && zapperTile <= 0xA6A )
    {
    disableZapper(newX, newY);
    }
    else if(zapperTile == 0xA6B ) // On Keen 6 this seems to be the zapper
    {
    enableZapper(newX, newY);
    }
    else
    {
      return false;
    }

    return true;
}

bool CPlayerLevel::verifyAndToggleBridge(const int lx, const int ly)
{
    Uint32 targetXY = mpMap->getPlaneDataAt(2, lx, ly);

    Uint32 newX = targetXY >> 8;
    Uint32 newY = targetXY & 0xFF;

    auto &Tile = gBehaviorEngine.getTileProperties(1);
    const int zapperTile = mpMap->getPlaneDataAt(1, newX<<CSF, newY<<CSF);
    const int flag = Tile[zapperTile].behaviour;

    if(flag == 18 )
    {
      toggleBridge(newX, newY);
    }
    else
    {
      return false;
    }

    return true;
}



void CPlayerLevel::TurnGiantSwitchOff(const int x, const int y)
{
    const int tile = mpMap->at(x, y);

    if(tile != 0x43C || yinertia > 0)
    return;

    for(int i=-1 ; i<=0 ; i++)
    {
    for(int j=-1 ; j<=1 ; j++)
    {
        const int tile = mpMap->at(x+i, y+j);
        mpMap->setTile( x+i, y+j, tile-3, true );
    }
    }

    std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);
    const int x_csf = x<<CSF;
    const int y_csf = y<<CSF;

    Uint32 tile_no = mpMap->getPlaneDataAt(1, x_csf, y_csf);
    int flag = Tile[tile_no].behaviour;

    // pressing a switch
    playSound( GameSound::GUN_CLICK );

    if(flag == 18)
    {
        PressBridgeSwitch(x_csf, y_csf);
    }
    else
    {
        if( verifyAndToggleZapper(x_csf, y_csf) )
        {
            return;
        }

        if( verifyAndToggleBridge(x_csf, y_csf) )
        {
            return;
        }

        PressPlatformSwitch(x_csf, y_csf);
    }
}

void CPlayerLevel::TurnGiantSwitchOn(const int x, const int y)
{
    const int tile = mpMap->at(x, y);

    if(tile != 0x439 || yinertia < 0)
    return;

    playSound( GameSound::GUN_CLICK );

    for(int i=-1 ; i<=0 ; i++)
    {
    for(int j=-1 ; j<=1 ; j++)
    {
        const int tile = mpMap->at(x+i, y+j);
        mpMap->setTile( x+i, y+j, tile+3, true );
    }
    }

    std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);
    const int x_csf = x<<CSF;
    const int y_csf = y<<CSF;

    Uint32 tile_no = mpMap->getPlaneDataAt(1, x_csf, y_csf);
    int flag = Tile[tile_no].behaviour;

    // pressing a switch
    playSound( GameSound::GUN_CLICK );

    if(flag == 18 )
    {
    PressBridgeSwitch(x_csf, y_csf);
    }
    else
    {
      if( verifyAndToggleZapper(x_csf, y_csf) )
    return;

      if( verifyAndToggleBridge(x_csf, y_csf) )
      return;

      PressPlatformSwitch(x_csf, y_csf);
    }
}



int CPlayerLevel::checkSolidU(int x1, int x2, int y1, const bool push_mode )
{
    if(mDying)  return 0;

    if(hitdetectWithTilePropertyHor(1, x1, x2, y1-COLLISION_RES, 1<<CSF))
        return 0;

    std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();

    y1 -= COLLISION_RES;

    // Check for sloped tiles here. They must be handled differently
    if(solid && !noclipping)
    {
        int8_t blocked;

        if(mIsClimbing)
        {
            x1 += 4*COLLISION_RES;
            x2 -= 4*COLLISION_RES;
        }

        for(int c=x1 ; c<=x2 ; c += COLLISION_RES)
        {
            blocked = TileProperty[mpMap->at(c>>CSF, y1>>CSF)].bdown;

            if(blocked == 33)
            {
                TurnGiantSwitchOff(c>>CSF, y1>>CSF);

                if(!gBehaviorEngine.mCheatmode.jump)
                {
                    playSound( GameSound::KEEN_BUMPHEAD );

                    if (blockedu > 1)
                    {
                        yinertia += 16;
                        if (yinertia < 0)
                            yinertia = 0;
                    }
                    else
                    {
                        if( yinertia<0 )
                            yinertia = 0;
                    }
                    mActionState.jumpTimer = 0;
                    mMustHandleSlope = false;
                }

                return 1;
            }

            if(blocked == 17 && mIsClimbing)
                return 0;

            if( blocked >= 2 && blocked <= 7 && checkslopedU(c, y1, blocked))
                return blocked;
        }

        blocked = TileProperty[mpMap->at(x2>>CSF, y1>>CSF)].bdown;

        if( blocked >= 2 && blocked <= 7 && checkslopedU(x2, y1, blocked ))
            return 1;

        if(blocked == 17 && mIsClimbing)
            return 0;
    }

    return CSpriteObject::checkSolidU(x1, x2, y1+COLLISION_RES, push_mode);
}


int CPlayerLevel::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
    std::vector<CTileProperties> &TileProperty = gBehaviorEngine.getTileProperties();

    // Negativ values not allowed
    if(y2 < 0)
        return 0;


    if(mDying)  return 0;

    y2 += COLLISION_RES;

    // Check for sloped tiles here. They must be handled differently
    if(solid && !noclipping)
    {
        int8_t blockedu;

        if(mIsClimbing)
        {
            x1 += 4*COLLISION_RES;
            x2 -= 4*COLLISION_RES;
        }

        for(int c=x1 ; c<=x2 ; c += COLLISION_RES)
        {
            blockedu = TileProperty[mpMap->at(c>>CSF, y2>>CSF)].bup;

            if(blockedu == 33)
            {
                TurnGiantSwitchOn(c>>CSF, y2>>CSF);

                const int action = getActionNumber();

                if ( action >= A_KEEN_POGO_START && action <= A_KEEN_POGO_HIGH && mActionState.jumpTimer == 0)
                {
                    yinertia = evalVertPogoInertia();
                    playSound( GameSound::KEEN_POGO );
                    mActionState.jumpTimer = 24;
                    mMustHandleSlope = true;
                    setAction(A_KEEN_POGO_UP);
                    return 1;
                }
            }

            if( blockedu == 17 && mIsClimbing)
                return 0;

            if( blockedu >= 2 && blockedu <= 7 && checkslopedD(c, y2, blockedu) )
                return blockedu;
        }

        blockedu = TileProperty[mpMap->at(x2>>CSF, y2>>CSF)].bup;

        if(blockedu == 17 && mIsClimbing)
            return 0;

        if( blockedu >= 2 && blockedu <= 7 && checkslopedD(x2, y2, blockedu)  )
            return blockedu;
    }


    if( ( (y2>>STC) != ((y2>>CSF)<<TILE_S) ) && !push_mode )
        return 0;


    // Check for down from the object
    if(solid && !noclipping)
    {
        int8_t blocked;
        for(int c=x1 ; c<=x2 ; c += COLLISION_RES)
        {
            blocked = TileProperty[mpMap->at(c>>CSF, y2>>CSF)].bup;

            if(blocked)
            {
                if( blocked < 2 || blocked > 7 )
                {
                    int8_t blockedd = TileProperty[mpMap->at(c>>CSF, y2>>CSF)].bdown;

                    if(blockedd == 0 && m_jumpdown)
                        return 0;

                    if(blockedd == 33) // keen 6 Giant switch
                        return 0;

                    return blocked;
                }
            }
        }

        blocked = TileProperty[mpMap->at((x2-(1<<STC))>>CSF, y2>>CSF)].bup;
        if(blocked)
        {
            if( blocked < 2 || blocked > 7 )
                return blocked;
        }
    }


    if( (Uint32)y2 > ((mpMap->m_height)<<CSF) )
    {
        kill(true, true);
    }

    return 0;
}



}
