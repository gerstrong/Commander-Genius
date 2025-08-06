/*
 * CPlayerWM.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 *
 *  The code for the player when he is shown on the map...
 */


#include "../../ep6/ai/CRope.h"

#include "CPlayerWM.h"
#include "CFlag.h"

#include "../dialog/CMessageBoxBitmapGalaxy.h"

#include "fileio/KeenFiles.h"
#include "engine/core/VGamepads/vgamepadsimple.h"

#include "engine/core/CBehaviorEngine.h"

#include <graphics/CDimDark.h>
#include <base/video/CVideoDriver.h>
#include <base/interface/property_tree/property_tree.h>
#include <base/audio/Audio.h>
#include <base/CInput.h>
#include <array>

const int TIME_TO_WAVE = 400;

namespace galaxy {

CPlayerWM::CPlayerWM(std::shared_ptr<CMap> pMap,
        const Uint16 foeID,
        Uint32 x,
        Uint32 y,
        CInventory &l_Inventory,
        const unsigned int actionoffset,
        const int playerIdx,
        const int spriteTableIdx):
CPlayerBase(pMap, foeID, x, y,
            LEFT,
            l_Inventory,
            playerIdx,
            spriteTableIdx)
{
    m_ActionBaseOffset = actionoffset;

    mEndOfAction = false;
    m_ActionNumber = 0;
    m_Action.setActionFormat(m_ActionBaseOffset);
    setActionSprite();

    walkBaseFrame = Uint16(mSpriteIdx);
    wavingBaseFrame = walkBaseFrame + 22;
    swimBaseFrame = walkBaseFrame + 24;
    climbBaseFrame = walkBaseFrame + (gBehaviorEngine.isDemo() ? 26 : 37);
    m_basesprite = walkBaseFrame;

    performCollisions();
    mProcessPtr = &CPlayerWM::processMoving;
}


void
CPlayerWM::
deserialize(CSaveGameController &savedGame)
{
  savedGame.decodeData(mUsedGrapplingHook);
}

void
CPlayerWM::
serialize(CSaveGameController &savedGame)
{
  savedGame.encodeData(mUsedGrapplingHook);
}

void
CPlayerWM::
serialize(GsKit::ptree &node)
{
    node.put("usedGrapplingHook", mUsedGrapplingHook);

    const bool swimming = isSwimming();
    node.put("isSwimming", swimming);
}
void
CPlayerWM::
deserialize(GsKit::ptree &node)
{
    mUsedGrapplingHook = node.get<bool>("usedGrapplingHook", false);

    bool swimming;
    swimming = node.get<bool>("isSwimming",false);
    makeHimSwim(swimming);
}


/**
 * Before Keen rides on the foot we get the location where to ride
 */
GsVec2D<int> CPlayerWM::fetchFootDestCoord()
{
    GsVec2D<int> location1;
    GsVec2D<int> location2;
    int coordData;

    const gs_byte *dataPtr = gKeenFiles.exeFile.getRawData();

    memcpy(&coordData, dataPtr+0xDE43, sizeof(int));
    location1.x = coordData & 0xFF;
    memcpy(&coordData, dataPtr+0xDE58, sizeof(int));
    location1.y = coordData & 0xFF;

    memcpy(&coordData, dataPtr+0xDE78, sizeof(int));
    location2.x = coordData & 0xFF;
    memcpy(&coordData, dataPtr+0xDE8C, sizeof(int));
    location2.y = coordData & 0xFF;


    // Check for the first location
    GsVec2D<int> vec1;
    GsVec2D<int> vec2;

    GsVec2D<int> levelCoord;

    levelCoord = getPosition();

    levelCoord.x >>= CSF;
    levelCoord.y >>= CSF;

    vec1.x = levelCoord.x - location1.x;
    vec1.y = levelCoord.y - location1.y;
    vec2.x = levelCoord.x - location2.x;
    vec2.y = levelCoord.y - location2.y;

    const int dist1 = vec1.GetLength2();
    const int dist2 = vec2.GetLength2();

    GsVec2D<int> newCoord;

    if(dist2 > dist1)
        newCoord = location2;
    else
        newCoord = location1;

    newCoord.x <<= CSF;
    newCoord.y <<= CSF;
    return newCoord;
}



void CPlayerWM::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    CSpriteObject::pumpEvent(evPtr);

    // Events for the Player are processed here.
    if( const auto ev = std::dynamic_pointer_cast<const EventPlayerEndLevel>(evPtr) )
    {
        if(ev->who == mPlayerIdx)
        {
            gEventManager.flush();
            if(ev->sucess)
            {
                finishLevel(ev->levelObject);

                if(gBehaviorEngine.getEpisode() == 5)
                {
                    // enough fuses broken, which are all the fuse levels, except the secret one.
                    // In this case open the elevator door for the last level!
                    // NOTE: I'm not sure, if there is a better way to do it.
                    // if you know one, go ahead and improve this!
                    if(m_Inventory.Item.fuse_levels_completed >= 4)
                    {
                        // Must happen only once!
                        m_Inventory.Item.fuse_levels_completed = 0;

                        int x = (0x1A)<<CSF; // upper left corner of that elevator
                        int y = (0x37)<<CSF; // on the map

                        // I only will change the closed elevator tiles!
                        const int tileID = mpMap->getPlaneDataAt(1, x, y);

                        // I only will change the closed elevator tiles!
                        if(tileID == 0x072C)
                        {
                            const int t_ul = tileID+10;
                            const int t_ur = mpMap->getPlaneDataAt(1, x+(1<<CSF), y) + 10;
                            const int t_ll = mpMap->getPlaneDataAt(1, x,          y+(1<<CSF)) + 10;
                            const int t_lr = mpMap->getPlaneDataAt(1, x+(1<<CSF), y+(1<<CSF)) + 10;

                            x >>= CSF; y >>= CSF;

                            mpMap->setTile(x,   y, t_ul, true);
                            mpMap->setTile(x+1, y, t_ur, true);
                            mpMap->setTile(x,   y+1, t_ll, true);
                            mpMap->setTile(x+1, y+1, t_lr, true);
                        }
                    }
                }
            }
        }

        if(ev->teleport)
        {
            // Find the spot of the teleportation destination
            // TODO: This part is only meant for Episode 5. We should catch exception
            // Whenever another episode tries to trigger this call.
            int x,y;
            mpMap->findTile( 0x1A, x, y, 2);

            const int newX = x<<CSF;
            const int newY = y<<CSF;

            m_Pos.x = newX;
            m_Pos.y = newY;

            m_camera.setPosition(m_Pos);
        }
    }
    else if( const auto ev = std::dynamic_pointer_cast<const EventPlayerRideFoot>(evPtr) )
    {
        if(ev->who == mPlayerIdx)
        {
            gEventManager.flush();
            finishLevel(ev->levelObject);
            solid = false;
        }

        // Here we need to set the coordinates calculated to where Keen has to go.
        target = fetchFootDestCoord();

        // Make Keen ride on the foot
        m_Action.setActionFormat(0x1492);
        setActionSprite();
        mProcessPtr = &CPlayerWM::processRiding;
    }
}



/**
 * The main process cycle for the player itself only on the map
 */
void CPlayerWM::process()
{
    if(mpMap->locked())
        return;

    processInput();

    // Perform animation cycle
    if(m_animation_ticker >= m_animation_time)
    {
        m_animation++;
        m_animation_ticker = 0;
    }
    else m_animation_ticker++;

    (this->*mProcessPtr)();


    processCamera();
}


/*
 * Processes the waving of the player on map here
 */
void CPlayerWM::processWaving()
{
    if(mounted)
    {
        mProcessPtr = &CPlayerWM::processMoving;
        m_basesprite = walkBaseFrame;
        waveTimer = 0;
        return;
    }

    waveTimer++;


    if( mPlaycontrol[PA_Y] != 0 || mPlaycontrol[PA_X] != 0 ||
        mPlaycontrol[PA_JUMP] || waveTimer >= (TIME_TO_WAVE/4) )
    {
        mProcessPtr = &CPlayerWM::processMoving;
        m_basesprite = walkBaseFrame;
        waveTimer = 0;
        return;
    }

    m_animation_time = 10;
    mSpriteIdx = m_basesprite;
    mSpriteIdx +=  m_animation%2;
}




/*
 * Processes the walking of the player on map here
 */
void CPlayerWM::processMoving()
{
    // Only happens in Keen6 when keening is hanging on the satelite
    if(mounted)
    {
        mSpriteIdx = 181;
        return;
    }


    // Check if the player is swimming or walking and setup the proper speed
    int movespeed;
    if(m_basesprite == swimBaseFrame)
        movespeed = 25;
    else if(m_basesprite == walkBaseFrame)
        movespeed = 50;
    else
        movespeed = 0;

    // Running a bit faster on the world map
    if(mPlaycontrol[PA_RUN])
    {
        movespeed = (movespeed*6)/5;
    }


    bool moving = false;

    bool bleft, bright, bup, bdown;

    // This will trigger between swim and walkmode
    checkforSwimming(bleft, bright, bup, bdown);

    // This will make Keen climb in
    direction_t climbDir;
    if( checkforClimbing(climbDir) )
    {
        // Check if Keen has a hook, but
        if(!mUsedGrapplingHook)
        {
            if(m_Inventory.Item.m_special.hook > 0)
            {
                m_Inventory.Item.m_special.hook--;
                mUsedGrapplingHook = true;

                int x = getXMidPos();
                int y = getYMidPos();

                x = x>>CSF;
                y = y>>CSF;

                x = x<<CSF;
                if(climbDir == UP)
                    y = (y+2*climbDir)<<CSF;
                else
                    y = (y+climbDir)<<CSF;

                spawnObj(new CRope(mpMap, x, y));
                playSound(GameSound::ROPE_THROW);
            }
            else
            {
                // Tell the player he cannot climb yet
                showModalMsgWithBmp(mSprVar, gBehaviorEngine.getString("KEEN_ROPE_REQUIRED"),
                               "KEENTALKING", RIGHT, false, nullptr);
                moveYDir(-(climbDir<<CSF)/2);
            }
        }

        if(mUsedGrapplingHook)
        {
            xDirection = CENTER;
            yDirection = climbDir;
            solid = false;
            mProcessPtr = &CPlayerWM::processClimbing;
            m_basesprite = climbBaseFrame;
            waveTimer = 0;
            return;
        }
    }

    // In Episode 5 and 6 there are teleporters. Verify those teleporters and elevators
    if(gBehaviorEngine.getEpisode() >= 5)
        verifyTeleportation();

    // Normal walking

    if( mPlaycontrol[PA_X]<0 && !bleft)
    {
        if( mPlaycontrol[PA_Y] == 0 )
            yDirection = 0;

        auto movespeedX = movespeed;
        movespeedX *= mPlaycontrol[PA_X];
        movespeedX /= (-100);

        moveLeft(movespeedX);
        moving = true;
        xDirection = LEFT;
        waveTimer = 0;
    }
    else if( mPlaycontrol[PA_X]>0 && !bright)
    {
        if(mPlaycontrol[PA_Y]==0)
            yDirection = 0;

        auto movespeedX = movespeed;
        movespeedX *= mPlaycontrol[PA_X];
        movespeedX /= (100);

        moveRight(movespeedX);
        moving = true;
        xDirection = RIGHT;
        waveTimer = 0;
    }

    if(mPlaycontrol[PA_Y]<0 && !bup)
    {
        if(mPlaycontrol[PA_X]==0)
            xDirection = 0;

        auto movespeedY = movespeed;
        movespeedY *= mPlaycontrol[PA_Y];
        movespeedY /= (-100);

        moveUp(movespeedY);
        moving = true;
        yDirection = UP;
        waveTimer = 0;
    }
    else if(mPlaycontrol[PA_Y]>0 && !bdown)
    {
        if(mPlaycontrol[PA_X]==0)
            xDirection = 0;

        auto movespeedY = movespeed;
        movespeedY *= mPlaycontrol[PA_Y];
        movespeedY /= (100);

        moveDown(movespeedY);
        moving = true;
        yDirection = DOWN;
        waveTimer = 0;
    }

    // No clipping makes player go through walls etc.
    noclipping = gBehaviorEngine.mCheatmode.noclipping;

    /// Check if Keen is able to access a level
    // Get the object
    int x = getXMidPos();
    int y = getYMidPos();
    const Uint16 object = mpMap->getInfoData(GsVec2D<Uint32>(x>>CSF,y>>CSF));
    if(object) // if we found an object
    {

#ifdef USE_VIRTUALPAD
        if( gVideoDriver.VGamePadEnabled() )
        {
            VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());

            int level = object - 0xC000;
            const Uint16 flag_dest = level + 0xF000;

            const int ep = gBehaviorEngine.getEpisode();
            const int shipLevel = (ep < 6) ? 18 : 17;

#if SDL_VERSION_ATLEAST(2, 0, 0)
            if(mpMap->findTile(flag_dest, x, y, 2) ||
                    gBehaviorEngine.mOptions[GameOption::LVLREPLAYABILITY].value ||
                    level >= shipLevel)
            {
                vkc->mStartButton.invisible = false;
            }
#endif
        }
#endif

        // Try to start a level
        if( mPlaycontrol[PA_JUMP] )
        {
            // start the level
            startLevel(object);
            gInput.flushCommands();
        }
    }
    else
    {
#ifdef USE_VIRTUALPAD
#if SDL_VERSION_ATLEAST(2, 0, 0)
        if( gVideoDriver.VGamePadEnabled() )
        {
            VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
            vkc->mDPad.invisible = false;
            vkc->mStatusButton.invisible = false;
            vkc->mStartButton.invisible = true;
            vkc->mMenuButton.invisible = true;
        }
#endif
#endif
    }


    // If keen is just walking on the map or swimming in the sea. Do the proper animation for it.
    if(m_basesprite == walkBaseFrame)
    {
        performWalkingAnimation(moving);
        m_cantswim = false;

        waveTimer++;
        if( waveTimer >= TIME_TO_WAVE)
        {
            mProcessPtr = &CPlayerWM::processWaving;
            m_basesprite = wavingBaseFrame;
            waveTimer = 0;
            return;
        }
    }
    else if( isSwimming() )
    {
        if(m_Inventory.Item.m_special.swimsuit)
        {
            performSwimmingAnimation(moving);
        }
        else
        {
            if( !m_cantswim )
            {
                playSound( GameSound::CANT_DO, SoundPlayMode::PLAY_PAUSEALL );
                showModalMsgWithBmp(mSprVar,
                               gBehaviorEngine.getString("CANT_SWIM_TEXT"),
                               105, LEFT, false, nullptr);

                m_cantswim = true;
            }

        }
    }
}



void CPlayerWM::processClimbing()
{
    moveYDir(yDirection*30);

    mSpriteIdx = m_basesprite + m_animation%2;

    direction_t climbDir;

    if( checkforClimbing(climbDir) )
    {
    if(yDirection != climbDir)
    {
        mProcessPtr = &CPlayerWM::processMoving;
        m_basesprite = walkBaseFrame;
        waveTimer = 0;
        solid = true;
        moveYDir(-climbDir<<CSF);
        return;
    }
    }

}



const int RIDE_SPEED = 32;

void CPlayerWM::processRiding()
{
    // Ride while trying to reach the destination coords
    // Move the player to the target
    GsVec2D<int> pos(getXPosition(), getYPosition());
    GsVec2D<int> vec = target-pos;

    GsVec2D<int> vec_norm = vec;

    const int dist_x = abs(vec.x);
    const int dist_y = abs(vec.y);

    if(dist_x != 0)
        vec_norm.x = vec.x/dist_x;
    if(dist_y != 0)
        vec_norm.y = vec.y/dist_y;

    if( dist_x < RIDE_SPEED &&	dist_y < RIDE_SPEED)
    {
        // When he reaches the target. make him visible and start opening the elevator
        moveDir(vec);
        setAction(0);
        mProcessPtr = &CPlayerWM::processMoving;
        solid = true;
    }
    else
    {
        moveDir(vec_norm*RIDE_SPEED);
        processActionRoutine();
    }
}


void CPlayerWM::verifyTeleportation()
{
    //target -> change it when the touched tile is known
    int x = getXMidPos();
    int y = getYUpPos();

    // Check if Keen touches a teleporter or elevator
    const Uint16 object = mpMap->getPlaneDataAt( 2, x, y );
    if(object < 0xC000 && object > 0x100)
    {
        x = (x >> CSF);
        y = (y >> CSF);

        bool isElevator = false;

        std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);
        Uint16 behav = Tile[mpMap->at( x, y, 1)].behaviour;

        // Elevator are double the size. Check that! Else it must be an teleporter
        if( behav==33 || behav==34 )
        {
          if(object == mpMap->getPlaneDataAt( 2, (x-1) << CSF, y << CSF ))
          {
            isElevator |= true;
          }
          if(object == mpMap->getPlaneDataAt( 2, (x+1) << CSF, y << CSF ))
          {
            x = x + 1;
            isElevator |= true;
          }
        }

        x = (x << CSF);
        y = (y << CSF);

        target.x = x;
        target.y = y;

        // In that case get the tile where to go and make him move or ...
        if(isElevator)
        {
            mProcessPtr = &CPlayerWM::processEnteringElevator;
        }
        else // ... make him move until teleporter hides him.
        {
            mProcessPtr = &CPlayerWM::processEnteringTeleporter;

            setupTeleportAnimation(false, target);

            playSound(GameSound::TELEPORT);

            solid = false;
        }
    }

}


// Elevators
const int SLOW_TELEPORT_WALK_SPEED = 8;
const int ELEVATOR_SPEED = 128;
const int ELEVATOR_CLOSE_TIME = 5;

void CPlayerWM::processEnteringElevator()
{
    // Move him to the target
    GsVec2D<int> pos(getXPosition(), getYPosition());
    GsVec2D<int> vec = target-pos;


    GsVec2D<int> vec_norm = vec;

    const int dist_x = abs(vec.x);
    const int dist_y = abs(vec.y);

    if(dist_x != 0)
        vec_norm.x = vec.x/dist_x;
    if(dist_y != 0)
        vec_norm.y = vec.y/dist_y;

    yDirection = vec_norm.y;


    if( dist_x < SLOW_TELEPORT_WALK_SPEED &&
        dist_y < SLOW_TELEPORT_WALK_SPEED)
    {
        moveDir(vec);
        mProcessPtr = &CPlayerWM::processClosingElevator;
        performWalkingAnimation(false);
        elevator_frames = 5;
        elevator_close_timer = 0;
    }
    else
    {
        moveDir(vec_norm*SLOW_TELEPORT_WALK_SPEED);
    }

    performWalkingAnimation(true);
}

void CPlayerWM::processClosingElevator()
{
    const int x = getXMidPos() >> CSF;
    const int y = getYMidPos() >> CSF;
    const Uint16 tile1 = mpMap->getPlaneDataAt( 1, x<<CSF, y<<CSF );
    const Uint16 tile2 = mpMap->getPlaneDataAt( 1, (x-1)<<CSF, y<<CSF );
    const Uint16 tile3 = mpMap->getPlaneDataAt( 1, (x-1)<<CSF, (y-1)<<CSF );
    const Uint16 tile4 = mpMap->getPlaneDataAt( 1, x<<CSF, (y-1)<<CSF );

    elevator_close_timer++;
    if(elevator_close_timer >= ELEVATOR_CLOSE_TIME)
    {
        elevator_close_timer = 0;

        // Make the player close the elevator
        mpMap->setTile(x, y, tile1-2, true);
        mpMap->setTile(x-1, y, tile2-2, true);
        mpMap->setTile(x-1, y-1, tile3-2, true);
        mpMap->setTile(x, y-1, tile4-2, true);

        playSound(GameSound::ELEVATOR_OPEN);

        elevator_frames--;

        if(elevator_frames == 0)
        {
            // If done make him invisible and transport him through the level. !solid

            //target -> change it when the touched tile is known
            const int x = getXMidPos();
            const int y = getYMidPos();

            // Check if Keen touches a teleporter or elevator
            const Uint16 object = mpMap->getPlaneDataAt( 2, x, y );
            const Uint32 filter = object & 0xFFFF;
            const Uint32 newPosX = (filter & 0xFF00) >> 8;
            const Uint32 newPosY = (filter & 0x00FF);

            // Set new target
            target.x = ((newPosX+1)<<CSF);
            target.y = (newPosY<<CSF);

            // make him invisible
            solid = false;
            dontdraw = true;

            // change process
            mProcessPtr = &CPlayerWM::processElevating;
        }
    }
}

void CPlayerWM::processElevating()
{
    // Move the player to the target
    GsVec2D<int> pos(getXPosition(), getYPosition());
    GsVec2D<int> vec = target-pos;


    GsVec2D<int> vec_norm = vec;

    const int dist_x = abs(vec.x);
    const int dist_y = abs(vec.y);

    if(dist_x != 0)
        vec_norm.x = vec.x/dist_x;
    if(dist_y != 0)
        vec_norm.y = vec.y/dist_y;

    if( dist_x < SLOW_TELEPORT_WALK_SPEED &&
        dist_y < SLOW_TELEPORT_WALK_SPEED)
    {
        moveDir(vec);
        mProcessPtr = &CPlayerWM::processOpeningElevator;
        performWalkingAnimation(false);
        elevator_frames = 5;
        elevator_close_timer = 0;
    }
    else
    {
        moveDir(vec_norm*ELEVATOR_SPEED);
        this->playSound(GameSound::ELEVATING);
    }
}

void CPlayerWM::processOpeningElevator()
{
    // Open until it's wide open
    const int x = getXMidPos() >> CSF;
    const int y = getYMidPos() >> CSF;

    std::array<Uint16,4> curTile;

    curTile[0] = mpMap->getPlaneDataAt( 1, x<<CSF,     y<<CSF );
    curTile[1] = mpMap->getPlaneDataAt( 1, (x-1)<<CSF, y<<CSF );
    curTile[2] = mpMap->getPlaneDataAt( 1, (x-1)<<CSF, (y-1)<<CSF );
    curTile[3] = mpMap->getPlaneDataAt( 1, x<<CSF,     (y-1)<<CSF );

    std::vector<CTileProperties> &tileProp = gBehaviorEngine.getTileProperties(1);

    const auto &prop0 = tileProp[ curTile[0] ];
    const auto &prop1 = tileProp[ curTile[1] ];
    const auto &prop2 = tileProp[ curTile[2] ];
    const auto &prop3 = tileProp[ curTile[3] ];

    elevator_close_timer++;
    if(elevator_close_timer >= ELEVATOR_CLOSE_TIME)
    {
        elevator_close_timer = 0;

        // Make the player close the elevator
        mpMap->setTile(x,   y,   curTile[0] + prop0.nextTile, true);
        mpMap->setTile(x-1, y,   curTile[1] + prop1.nextTile, true);
        mpMap->setTile(x-1, y-1, curTile[2] + prop2.nextTile, true);
        mpMap->setTile(x,   y-1, curTile[3] + prop3.nextTile, true);

        playSound(GameSound::ELEVATOR_OPEN);

        elevator_frames--;

        if(elevator_frames == 0)
        {
            // make him visible
            dontdraw = false;

            target.y += (1<<CSF);

            // and walk out
            mProcessPtr = &CPlayerWM::processLeavingElevator;
        }
    }
}

void CPlayerWM::processLeavingElevator()
{
    // Move him to the target
    GsVec2D<int> pos(getXPosition(), getYPosition());
    GsVec2D<int> vec = target-pos;

    GsVec2D<int> vec_norm = vec;

    const int dist_x = abs(vec.x);
    const int dist_y = abs(vec.y);

    if(dist_x != 0)
    {
        vec_norm.x = vec.x/dist_x;
    }
    if(dist_y != 0)
    {
        vec_norm.y = vec.y/dist_y;
    }

    yDirection = vec_norm.y;

    if( dist_x < SLOW_TELEPORT_WALK_SPEED &&
        dist_y < SLOW_TELEPORT_WALK_SPEED)
    {
        // When done set player to solid state
        solid = true;
        moveDir(vec);
        mProcessPtr = &CPlayerWM::processMoving;
    }
    else
    {
        moveDir(vec_norm*SLOW_TELEPORT_WALK_SPEED);
    }

    performWalkingAnimation(true);
}


// Teleporter

void CPlayerWM::setupTeleportAnimation(const bool unset, const GsVec2D<int> &pos)
{
    const int x = pos.x;
    const int y = pos.y;
    const int ep = gBehaviorEngine.getEpisode();

    if(!unset)
        m_teleportoldtile = mpMap->getPlaneDataAt( 1, x, y );

    // Depending on having Keen 6 or 5 the animation tiles are a bit different
    if( ep==5 )
    {
        m_teleportanibasetile = unset ? m_teleportoldtile : 0xA7F;
    }
    if( ep==6 )
    {
        m_teleportanibasetile = unset ? m_teleportoldtile : 0xA35;
    }

    mpMap->setTile(x>>CSF, y>>CSF, m_teleportanibasetile, true);
}


void CPlayerWM::processEnteringTeleporter()
{
    // Move him to the target
    GsVec2D<int> pos(getXPosition(), getYPosition());
    GsVec2D<int> vec = target-pos;


    GsVec2D<int> vec_norm = vec;

    const int dist_x = abs(vec.x);
    const int dist_y = abs(vec.y);

    if(dist_x != 0)
        vec_norm.x = vec.x/dist_x;
    if(dist_y != 0)
        vec_norm.y = vec.y/dist_y;

    yDirection = vec_norm.y;

    // if Keen reached target
    if( dist_x < SLOW_TELEPORT_WALK_SPEED &&
        dist_y < SLOW_TELEPORT_WALK_SPEED)
    {
        moveDir(vec);

        // If done make him invisible and transport him through the map. !solid

        //target -> change it when the touched tile is known
        const int x = getXMidPos();
        const int y = getYMidPos();

        setupTeleportAnimation(true, target);

        // Get the destination
        const Uint16 object = mpMap->getPlaneDataAt( 2, x, y );
        const Uint32 filter = object & 0xFFFF;
        const Uint32 newPosX = (filter & 0xFF00) >> 8;
        const Uint32 newPosY = (filter & 0x00FF);

        // Set new target
        target.x = (newPosX<<CSF);
        target.y = (newPosY<<CSF);

        // make him invisible
        solid = false;
        dontdraw = true;
        mProcessPtr = &CPlayerWM::processWarpInTeleporter;
        performWalkingAnimation(false);
    }
    else
    {
      const int x = target.x;
      const int y = target.y;

      const int ep = gBehaviorEngine.getEpisode();

      // Amount of animation tiles.
      const int teleportAnimTiles = (ep==5) ? 1 : 3;

      Uint16 aniTile = mpMap->getPlaneDataAt( 1, x, y ) + 1;

      if(m_teleportanibasetile + teleportAnimTiles < aniTile)
      {
        aniTile = m_teleportanibasetile;
      }

      mpMap->setTile(x>>CSF, y>>CSF, aniTile, true);

      moveDir(vec_norm*SLOW_TELEPORT_WALK_SPEED);
    }

    performWalkingAnimation(true);

}


void CPlayerWM::processWarpInTeleporter()
{
    // Move the player to the target directly
    GsVec2D<int> new_pos(target);
    moveToForce(target);
    new_pos.x += ((m_BBox.x2-m_BBox.x1)/2);
    new_pos.y += ((m_BBox.y2-m_BBox.y1)/2);
    m_camera.setPosition(new_pos);

    mpMap->mGamePlayPos = new_pos;
    mpMap->calcVisibleArea();
    mpMap->refreshVisibleArea();

    mProcessPtr = &CPlayerWM::processLeavingTeleporter;
    playSound(GameSound::TELEPORT);

    setupTeleportAnimation(false, new_pos);

    target.y += (1<<CSF);
    dontdraw = false;
}

void CPlayerWM::processLeavingTeleporter()
{
    // Move him to the target
    GsVec2D<int> pos(getXPosition(), getYPosition());
    GsVec2D<int> vec = target-pos;

    GsVec2D<int> vec_norm = vec;

    const int dist_x = abs(vec.x);
    const int dist_y = abs(vec.y);

    if(dist_x != 0)
        vec_norm.x = vec.x/dist_x;
    if(dist_y != 0)
        vec_norm.y = vec.y/dist_y;

    yDirection = vec_norm.y;

    if( dist_x < SLOW_TELEPORT_WALK_SPEED &&
        dist_y < SLOW_TELEPORT_WALK_SPEED)
    {
        // When done set him solid
        solid = true;
        moveDir(vec);

        GsVec2D<int> animTilePos = target;
        animTilePos.y -= (1<<CSF);
        setupTeleportAnimation(true, animTilePos);
        mProcessPtr = &CPlayerWM::processMoving;
    }
    else
    {
        const int x = pos.x;
        const int y = pos.y;

        const int ep = gBehaviorEngine.getEpisode();

        // Amount of animation tiles.
        const int teleportAnimTiles = (ep==5) ? 1 : 3;

        Uint16 aniTile = mpMap->getPlaneDataAt( 1, x, y ) + 1;

        if(m_teleportanibasetile + teleportAnimTiles < aniTile)
        {
          aniTile = m_teleportanibasetile;
        }

        mpMap->setTile(x>>CSF, y>>CSF, aniTile, true);

        moveDir(vec_norm*SLOW_TELEPORT_WALK_SPEED);
    }

    performWalkingAnimation(true);
}




/**
 * This function will help starting the level for Commander Keen
 */
void CPlayerWM::startLevel(Uint16 object)
{
    int x, y;
    int level = object - 0xC000;

    const auto ep = gBehaviorEngine.getEpisode();
    int shipLevel;

    switch(ep)
    {
        case 4 : shipLevel = 18; break;
        case 5 : shipLevel = 15; break;
        case 6 : shipLevel = 17; break;
        default : shipLevel = 0;
    };

    if(ep == 4)
    {
        shipLevel = 18;

        if(!gKeenFiles.exeFile.isLuaScript())
        {
            shipLevel = gKeenFiles.exeFile.getRawData()[0x60FD];
        }
    }

    const Uint16 flag_dest = level + 0xF000;

    // Check if there already exists a flag. If that's not the case enter the level
    if( mpMap->findTile(flag_dest, x, y, 2) ||
        gBehaviorEngine.mOptions[GameOption::LVLREPLAYABILITY].value ||
        level >= shipLevel)
    {
        gEventManager.add(new EventEnterLevel(mSprVar, object));

        if(level > 0 && level < 50)
        {
            gEffectController.setupEffect(new CDimDark(8));
        }
    }
}

/*
 *	makes the player finish the level
 */
bool CPlayerWM::finishLevel(const int object)
{
    mpMap->refreshVisibleArea();

    // if a door or other blocker was found remove it
    int x, y;
    Uint16 door = object + 0xD000;
    while( mpMap->findTile(door, x, y, 2) )
    {
        // Remove blocks in case there are
        mpMap->setTile( x, y, 0, true, 1);
        mpMap->setInfoTile( {x, y}, 0 );
        mpMap->redrawAt( x, y);
    }

    Uint16 flag_dest = object + 0xF000;
    if( mpMap->findTile(flag_dest, x, y, 2) )
    {
        // spawn the flag
        const auto episode = gBehaviorEngine.getEpisode();
        GsVec2D<Uint32> src(getXPosition(), getYPosition());

        // Here we move the coordinates for the correction position of the done flag/sign
        GsSprite &FlagSprite = gGraphics.getSprite(mSprVar, gBehaviorEngine.isDemo() ? WAVING_BASEFRAME_DEMO : WAVING_BASEFRAME);

        unsigned int csfX = (x<<CSF);
        unsigned int csfY = (y<<CSF);

        csfX += (7<<STC);
        csfY += (2<<STC);

        // Different placement spots
        if(episode == 4)
        {
            csfY -= FlagSprite.m_bboxY2;
        }
        else if(episode == 5)
        {
            csfX -= (15<<STC);
            csfY -= (2<<CSF);
        }
        else if(episode == 6)
        {
            csfY -= (FlagSprite.m_bboxY2-FlagSprite.m_bboxY1+32);
            csfY += (2<<STC);
        }

        GsVec2D<Uint32> dst(csfX, csfY);

        CFlag *pFlag = new CFlag(mpMap, src, dst, mSprVar, true, true);
        spawnObj(pFlag);


        // Mark the tileinfo on the map as level finished,
        // so player cannot just re-enter. Exception: If option "replayability" is enabled,
        // or any special level like the bwb rocket, which can be accessed at any time
        mpMap->setInfoTile( {x, y}, 0 );

        return true;
    }

    return false;
}

/**
 * This is the function will switch between swim and walk mode
 * Those are the tileproperties to check for
 * 11      Enter water from top	Keen 4
 * 12      Enter water from right Keen 4
 * 13      Enter water from bottom Keen 4
 * 14      Enter water from left Keen 4
 */
void CPlayerWM::checkforSwimming(bool &bleft, bool &bright, bool &bup, bool &bdown)
{
    Uint16 left, right, up, down;
    std::vector<CTileProperties> &Tile = gBehaviorEngine.getTileProperties(1);

    bleft = bright = bup = bdown = false;

    left = Tile[mpMap->at( getXLeftPos()>>CSF, getYMidPos()>>CSF, 1)].behaviour;
    right = Tile[mpMap->at( getXRightPos()>>CSF, getYMidPos()>>CSF, 1)].behaviour;
    up = Tile[mpMap->at( getXMidPos()>>CSF, getYUpPos()>>CSF, 1)].behaviour;
    down = Tile[mpMap->at( getXMidPos()>>CSF, getYDownPos()>>CSF, 1)].behaviour;

    // from top
    if(up == 11)
    {
        bdown = true;
        makeHimSwim(true);
    }
    else if(down == 11)
    {
        makeHimSwim(false);
    }

    // from right
    if(right == 12)
    {
        bleft = true;
        makeHimSwim(true);
    }
    else if(left == 12)
    {
        makeHimSwim(false);
    }

    // from bottom
    if(down == 13)
    {
        bup = true;
        makeHimSwim(true);
    }
    else if(up == 13)
    {
        makeHimSwim(false);
    }

    // from left
    if(left == 14)
    {
        bright = true;
        makeHimSwim(true);
    }
    else if(right == 14)
    {
        makeHimSwim(false);
    }

    if(m_Inventory.Item.m_special.swimsuit)
    {
        bleft = bright = bup = bdown = false;
    }
}

bool CPlayerWM::checkforClimbing(direction_t &climbDir)
{
    const int y = getYMidPos();
    const int x = getXMidPos();

    const auto info = mpMap->getInfoData(GsVec2D<Uint32>(x>>CSF,y>>CSF));

    // from top
    if(info == 0x0F)
    {
        climbDir = UP;
        return true;
    }
    else if(info == 0x10)
    {
        climbDir = DOWN;
        return true;
    }

    return false;
}


/**
 * This performs the animation when player is walking on the map
 */
void CPlayerWM::performWalkingAnimation(bool walking)
{
    if(xDirection == RIGHT && yDirection == 0)
        mSpriteIdx = m_basesprite + 1;
    else if(xDirection == 0 && yDirection == UP)
        mSpriteIdx = m_basesprite + 4;
    else if(xDirection == 0 && yDirection == DOWN)
        mSpriteIdx = m_basesprite + 7;
    else if(xDirection == RIGHT && yDirection == DOWN)
        mSpriteIdx = m_basesprite + 10;
    else if(xDirection == LEFT && yDirection == DOWN)
        mSpriteIdx = m_basesprite + 13;
    else if(xDirection == LEFT && yDirection == UP)
        mSpriteIdx = m_basesprite + 16;
    else if(xDirection == RIGHT && yDirection == UP)
        mSpriteIdx = m_basesprite + 19;
    else
        mSpriteIdx = m_basesprite - 2;

    if(walking)
    {
        m_animation_time = 5;
        mSpriteIdx +=  m_animation%3;
        playWalkSound();
    }
    else
        mSpriteIdx +=  2;

    if(swimming)
    {
        playSound(GameSound::KEEN_SWIM_TO_LAND);
        swimming = false;
    }

}

/**
 * This performs the animation when player is swimming in water on the map
 */
void CPlayerWM::performSwimmingAnimation(const bool moving)
{
    if(xDirection == RIGHT && yDirection == 0)
        mSpriteIdx = m_basesprite + 2;
    else if(xDirection == 0 && yDirection == DOWN)
        mSpriteIdx = m_basesprite + 4;
    else if(xDirection == LEFT && yDirection == 0)
        mSpriteIdx = m_basesprite + 6;
    else if(xDirection == RIGHT && yDirection == UP)
        mSpriteIdx = m_basesprite + 8;
    else if(xDirection == RIGHT && yDirection == DOWN)
        mSpriteIdx = m_basesprite + 10;
    else if(xDirection == LEFT && yDirection == DOWN)
        mSpriteIdx = m_basesprite + 12;
    else if(xDirection == LEFT && yDirection == UP)
        mSpriteIdx = m_basesprite + 14;
    else
        mSpriteIdx = m_basesprite;

    if(!swimming)
    {
        playSound(GameSound::KEEN_SWIM_TO_LAND);
        swimming = true;
    }

    m_animation_time = 5;
    mSpriteIdx +=  m_animation%2;

    playSwimSound(moving);
}

void CPlayerWM::setMounted(const bool value)
{
    mounted = value;
}

void CPlayerWM::processMoveBitDown()
{
    /// Now check the neighboring tile to the down
    const unsigned int x1 = getXPosition()+m_BBox.x1;
    const unsigned int x2 = getXPosition()+m_BBox.x2;
    const unsigned int y2 = getYPosition()+m_BBox.y2;

    blockedd = checkSolidD(x1, x2, y2);

    if( blockedd )
    {
        // additionally if there is a narrow space and the object might fit in, try to move it into that space
        const int xMid = (x1+x2)/2;
        if(checkSolidD(x1, xMid, y2) == false)
        {
            processMoveBitLeft();
        }
        else if(checkSolidD(xMid, x2, y2) == false)
        {
            processMoveBitRight();
        }

        return;
    }

    // if we are here, the tiles aren't blocking us.
    m_Pos.y++;
}


bool CPlayerWM::isSwimming()
{
    return (m_basesprite == swimBaseFrame);
}

void CPlayerWM::makeHimSwim(const bool value)
{
    if(value)
        m_basesprite = swimBaseFrame;
    else
        m_basesprite = walkBaseFrame;
}




}
