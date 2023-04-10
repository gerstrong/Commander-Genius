/*
 * CSpriteObject.cpp
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 */

#include "engine/core/spritedefines.h"
#include "CSpriteObject.h"

#include <base/GsLogging.h>
#include <base/GsEventContainer.h>
#include <base/video/CVideoDriver.h>

#include <base/interface/FindFile.h>
#include <graphics/GsSprite.h>
#include <graphics/GsGraphics.h>

#include "fileio/KeenFiles.h"

#include <base/audio/Audio.h>
#include "CBehaviorEngine.h"


int CSpriteObject::m_number_of_objects = 0; // The current number of total objects we have within the game!

void CSpriteObject::resetMainVars()
{
    exists = true;
    solid = true;
    inhibitfall = false;
    canbezapped = false;
    onscreen = false;
    yinertia = 0;
    xinertia = 0;
    onslope = false;
    scrx = scry = 0;
    mIsDead = false;
    hasbeenonscreen = false;
    honorPriority = true;
    dontdraw = false;
    cansupportplayer = false;
    dying = false;
    yDirection = xDirection = 0;
    pSupportedbyobject = nullptr;

    blockedd = false;
    blockedu = false;
    blockedl = false;
    blockedr = false;
}

///
// Initialization Routine
///
CSpriteObject::CSpriteObject(CMap *pmap,
                             const int x, const int y,
                             const int spriteVar) :
m_index(m_number_of_objects),
mpMap(pmap),
m_Pos(x,y),
mSprVar(spriteVar)
{
	m_number_of_objects++;

    resetMainVars();

    setupCollisionModel();
}


bool CSpriteObject::loadLuaScript(const std::string &scriptBaseName)
{
    bool ok = true;

    auto fullFName = JoinPaths(gKeenFiles.gameDir ,"ai");
    fullFName = JoinPaths(fullFName, scriptBaseName + ".lua");

    ok &= mLua.loadFile( fullFName );

    if(!ok)
        return false;

    mLua.runFunctionRetOneBool("canRecoverFromStun", mRecoverFromStun);
    mLua.runFunctionRetOneBool("turnAroundOnCliff", mTurnAroundOnCliff);
    mLua.runFunctionRetOneBool("endGameOnDefeat", mEndGameOnDefeat);
    mLua.runFunctionRetOneBool("isInvincible", mInvincible);
    mLua.runFunctionRetOneBool("willNeverStop", mNeverStop);
    mLua.runFunctionRetOneBool("isStunnableWithPogo", mPogoStunnable);
    mLua.runFunctionRetOneBool("isStunnableWithJump", mJumpStunnable);
    mLua.runFunctionRetOneBool("mayShoot", mMayShoot);
    mLua.runFunctionRetOneBool("moreAgressive", mMoreAgressive);

    int health = int(mHealthPoints);
    mLua.runFunctionRetOneInt("healthPoints", health);
    mHealthPoints = static_cast<unsigned int>(health);

    int walksound = int(mWalkSound);
    mLua.runFunctionRetOneInt("walkSound", walksound);
    mWalkSound = static_cast<GameSound>(walksound);

    return true;
}


void CSpriteObject::setScrPos( int px, int py )
{
	scrx = px;
	scry = py;
}


bool CSpriteObject::PoleCollision()
{
    int l_x = getXLeftPos();
    int l_w = getXRightPos() - getXLeftPos();
    int l_h = getYDownPos() - getYUpPos();
    int l_y = getYMidPos();
    
    return hitdetectWithTilePropertyRectRO(1, l_x, l_y, l_w, l_h, 1<<CSF);
}

// This functions checks, if the enemy is near to the player. In case, that it is
// it will return true. Other case it will return false.
// This used for objects that only can trigger, when it's really worth to do so.
bool CSpriteObject::calcVisibility()
{
	int visibility = gBehaviorEngine.getPhysicsSettings().misc.visibility;

	SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();

    const auto scroll = mpMap->getMainScrollCoords();

    const int left = (((scroll.x<<STC)-(visibility<<CSF))<0) ? 0 :
                       (scroll.x<<STC)-(visibility<<CSF);
    const int right = ((scroll.x+gameres.w)<<STC)+(visibility<<CSF);
    const int up = (((scroll.y<<STC)-(visibility<<CSF))<0) ? 0 :
                            (scroll.y<<STC)-(visibility<<CSF);
    const int down = ((scroll.y+gameres.h)<<STC)+(visibility<<CSF);

	return ( right > m_Pos.x && left < m_Pos.x && down > m_Pos.y && up < m_Pos.y );
}




bool CSpriteObject::verifyForFalling()
{
	if( !blockedd )
	{
        // This will check three points (below feet) and avoid that foes fall on sloped tiles
        auto &relTileProp = gBehaviorEngine.getTileProperties(1);

        bool can_fall = true;

        for(int i=0 ; i<3 ; i++)
        {
            const auto fallTileL = mpMap->getPlaneDataAt(1, getXLeftPos(), getYDownPos()+(i<<(CSF)));
            const auto fallTileM = mpMap->getPlaneDataAt(1, getXMidPos(), getYDownPos()+(i<<(CSF)));
            const auto fallTileR = mpMap->getPlaneDataAt(1, getXRightPos(), getYDownPos()+(i<<(CSF)));
            const CTileProperties &TilePropL = relTileProp[fallTileL];
            const CTileProperties &TilePropM = relTileProp[fallTileM];
            const CTileProperties &TilePropR = relTileProp[fallTileR];
            can_fall &= (TilePropL.bup == 0);
            can_fall &= (TilePropM.bup == 0);
            can_fall &= (TilePropR.bup == 0);
        }

		if(can_fall)
		{
			return true;
		}
		else
		{
			// Force the player a bit down, so he will never fall from sloped tiles
			moveDown(100);
		}
	}

	return false;
}



void CSpriteObject::performCliffStop(const int &speed)
{
	if(verifyForFalling())
	{
		if( xDirection == RIGHT )
		{
			xDirection = LEFT;
			moveLeft( speed );
		}
		else
		{
			xDirection = RIGHT;
			moveRight( speed );
		}
	}
}






// Used in some setup mode, like putting the player to
// the current map position
void CSpriteObject::moveToForce(const GsVec2D<int> &dir)
{
	m_Pos = dir;
}

void CSpriteObject::moveToForce(const int new_x, const int new_y)
{
    moveToForce(GsVec2D<int>(new_x, new_y));
}

// For the vector functions
void CSpriteObject::moveDir(const GsVec2D<int> &dir)
{
	moveXDir(dir.x);
	moveYDir(dir.y);
}

void CSpriteObject::moveToHorizontal(const int& new_x)
{
	const int pos_x = new_x - m_Pos.x;
	moveXDir(pos_x);
}

void CSpriteObject::moveToVertical(const int& new_y)
{
	const int pos_y = new_y - m_Pos.y;
	moveYDir(pos_y);
}

void CSpriteObject::moveTo(const GsVec2D<Uint32> &new_loc)
{
    GsVec2D<int> amount = new_loc - m_Pos;

	moveXDir(amount.x);
	moveYDir(amount.y);
}

void CSpriteObject::moveTo(const int new_x, const int new_y)
{
    moveTo(GsVec2D<Uint32>(new_x, new_y));
}

void CSpriteObject::moveXDir(const int amount, const bool force)
{
	if(amount<0)
		moveLeft(-amount, force);
	else if(amount>0)
		moveRight(amount, force);
}
void CSpriteObject::moveYDir(const int amount)
{
	if(amount<0)
		moveUp(-amount);
	else if(amount>0)
		moveDown(amount);
}

void CSpriteObject::moveLeft(const int amnt, const bool)
{
	if(amnt <= 0)
		return;

    mMoveTasks.push_back(new ObjMove(-amnt, 0));
}

void CSpriteObject::moveRight(const int amnt, const bool)
{
	if(amnt <= 0)
		return;

    mMoveTasks.push_back(new ObjMove(amnt, 0));
}

void CSpriteObject::moveUp(const int amnt)
{
	if(amnt <= 0)
		return;

    mMoveTasks.push_back(new ObjMove(0, -amnt));
}

void CSpriteObject::moveDown(const int amnt)
{
	// If zero was given as argument return.
	if(amnt <= 0)
		return;

    mMoveTasks.push_back(new ObjMove(0, amnt));
}

// This decreases the inertia we have of the object in X-direction.
// It should be used for objects, where it must be assured, that the inertia can get
// zero and not pass that limit
void CSpriteObject::decreaseXInertia(const int value)
{
	if(xinertia < 0)
	{
		if(xinertia+value > 0) xinertia = 0;
		else xinertia += value;
	}
	else if(xinertia > 0)
	{
		if(xinertia-value < 0) xinertia = 0;
		else xinertia -= value;
	}
}

// handles inertia and friction for the X direction
void CSpriteObject::InertiaAndFriction_X(const int friction_rate)
{
	int dx=xinertia;
	// check first if the player is not blocked
	if( (!blockedr and dx>0) or (!blockedl and dx<0) )
		moveXDir(dx);
	else
		xinertia = 0;

	// and apply friction to xinertia
	// when pogoing apply friction till we get down to PFASTINCMAXSPEED
	// then stop the friction
	decreaseXInertia(friction_rate);
}

void CSpriteObject::processFallPhysics(const int boost)
{
	CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();

	// In this case foe is jumping?
	// Not sure here. We should use another variable...
	if(yinertia<0 && !blockedu)
	{
		moveUp(-yinertia);
		yinertia += boost;
	}
	else if( yinertia>=0 && !blockedd )
	{
		moveDown(yinertia);

		// gradually increase the fall speed up to maximum rate
		if (yinertia>Physics.max_fallspeed)
			yinertia = Physics.max_fallspeed;
		else if (yinertia<Physics.max_fallspeed)
			yinertia += boost;
	}

	// hit floor or ceiling? set inertia to zero
	if( (blockedd && yinertia>0) || (blockedu && yinertia<0) )
		yinertia = 0;
}

bool CSpriteObject::revive()
{
    mIsDead = false;

    return true;
}

void CSpriteObject::processFallPhysics()
{
	CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();
	processFallPhysics(Physics.fallspeed_increase);
}

/**
 * processes falling of an object. Can be player or any other foe
 */
void CSpriteObject::processFalling()
{
	// CAUTION: There is a difference between falling and going down with the gravity...

	// So it reaches the maximum of fallspeed
	if(!inhibitfall)
	{
		processFallPhysics();
	}
	else
	{
		moveYDir(yinertia);
	}

	// sometimes, due to mistakes on the map, some foe are embedded into blocks!
	// In order to avoid, that they can't get out, pull them out of there!
}

void CSpriteObject::getShotByRay(object_t &)
{
    if( !mInvincible && mHealthPoints>0)
	{
        if(mHealthPoints > 1
           &&
           gBehaviorEngine.mOptions[GameOption::SPECIALFX].value)
        {
			blink(10);
        }

		mHealthPoints--;
	}
}

// anything (players/enemies) occupying the map tile at [mpx,mpy] is killed
void CSpriteObject::kill_intersecting_tile(int mpx, int mpy, CSpriteObject &theObject)
{
	 unsigned int xpix,ypix;
	 unsigned int x, y;
     xpix = static_cast<unsigned int>(mpx << CSF);
     ypix = static_cast<unsigned int>(mpy << CSF);

     x = static_cast<unsigned int>(theObject.getXMidPos());
     y = static_cast<unsigned int>(theObject.getYUpPos());
	 if (theObject.exists)
	 {
		 if (xpix-(1<<CSF) <= x && xpix+(1<<CSF) >= x)
		 {
			 if (ypix <= y && ypix+(1<<CSF) >= y)
			 {
				 theObject.kill();
				 theObject.dontdraw = true;
                 theObject.exists = false;
			 }
		 }
	 }
}



// Just kills the object
void CSpriteObject::kill(const bool, const bool)
{
	if ( exists && canbezapped )
	{
		mHealthPoints = 0;
		mIsDead = true;
	}
}

/**
 * \brief This function triggers the blinking behavior of an object (The object flashes).
 * 		  Normally this happens, when an enemy gets shot and can get
 * 		  multiple hits.
 * \param frametime	amount of drawn blinking frames during the blitting.
 * 					Every draw cycle performs one
 */
void CSpriteObject::blink(Uint16 frametime)
{
    m_blinktime = frametime;
}

void CSpriteObject::playSound(const GameSound snd,
                              const SoundPlayMode mode )
{
    gAudio.playStereofromCoord(static_cast<const int>(snd), mode, scrx);
}


////
// For drawing
////

// Functions finally draws the object also considering that there could be a masked
// or priority tile!
void CSpriteObject::draw()
{
    if( mSpriteIdx == BLANKSPRITE || dontdraw )
		return;

    GsSprite &Sprite = gGraphics.getSprite(mSprVar, mSpriteIdx);

    if(Sprite.empty())
    {
        return;
    }

    const auto scroll = mpMap->getMainScrollCoords();

    scrx = (m_Pos.x>>STC)-scroll.x;
    scry = (m_Pos.y>>STC)-scroll.y;

	SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();    

    if( scrx < gameres.w && scry < gameres.h && exists )
	{
        int showX = scrx+Sprite.getXOffset();
        int showY = scry+Sprite.getYOffset();

        int w = Sprite.getWidth();
        int h = Sprite.getHeight();

        const auto visGA = gVideoDriver.mpVideoEngine->mRelativeVisGameArea;

        if( showX+w < visGA.pos.x || showX > visGA.pos.x+visGA.dim.x+16 )
            return;

        if( showY+h < visGA.pos.y || showY > visGA.pos.y+visGA.dim.y+16 )
            return;

        if( showX+Sprite.getWidth() > visGA.pos.x+visGA.dim.x+16 )
            w = w - ((showX+w) - (visGA.pos.x+visGA.dim.x+16));
        if( showY+Sprite.getHeight() > visGA.pos.y+visGA.dim.y+16 )
            h = h - ((showY+h) - (visGA.pos.y+visGA.dim.y+16));

		if(m_blinktime > 0)
		{            
            Sprite.drawBlinkingSprite( showX, showY );
			m_blinktime--;
		}
		else
		{
            if (w && h)
            {
                // Only draw textures, when some translucency applied.
                if(useScrollTexels)
                {
                    if( (255-transluceny) == 255 )
                        Sprite.renderTexture( showX, showY, w, h );
                    else
                        Sprite.drawSprite( showX, showY, w, h, (255-transluceny) );
                }
                else
                {
                    if( (255-transluceny) == 255 )
                        Sprite.drawSprite( showX, showY, w, h );
                    else
                        Sprite.drawSprite( showX, showY, w, h, (255-transluceny) );
                }
            }
            else
            {
                gLogging.ftextOut("drawSprite problem w=%d h=%d mSprVar=%d mSpriteIdx=%d", w, h, mSprVar, mSpriteIdx);
            }
        }
		hasbeenonscreen = true;
	}
}

///
// Cleanup Routine
///
CSpriteObject::~CSpriteObject()
{
	if(m_number_of_objects > 0)
		m_number_of_objects--;
}

void spawnObj(const CSpriteObject *obj)
{
    gEventManager.add(new EventSpawnObject( obj ));
}


ObjMove::~ObjMove()
{}
