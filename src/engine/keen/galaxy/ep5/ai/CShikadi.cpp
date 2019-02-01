/*
 * CShikadi.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShikadi.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CEnemyShot.h"
#include <base/utils/misc.h>


/*
$2D10W #Shikadi stand
$2D2EW #Shikadi stand
$2D4CW #Shikadi stand
$2D6AW #Shikadi stand
$2D88W #Shikadi walk 4
$2DA6W #Shikadi walk
$2DC4W #Shikadi walk
$2DE2W #Shikadi walk
$2E00W #Shikadi polezaps 8
$2E1EW #Shikadi polezaps
$2E3CW #Shikadi stunned 10
$2E5AW #Shikadi polezap 11
$2E78W #Shikadi polezap
 */


namespace galaxy {  
  
enum SHIKADIACTIONS
{
A_SHIKADI_STAND = 0,
A_SHIKADI_WALK = 4,
A_SHIKADI_POLE_ZAPS = 8,
A_SHIKADI_STUNNED = 10
};

constexpr int WALK_SPEED = 25;
constexpr int TIME_UNTIL_STAND = 150;
//constexpr int TIME_TO_ZAP = 30;
  
CShikadi::CShikadi(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mHealth(4),
mTimer(0)
{
    mActionMap[A_SHIKADI_STAND]     = static_cast<GASOFctr>(&CShikadi::processStanding);
    mActionMap[A_SHIKADI_WALK]      = static_cast<GASOFctr>(&CShikadi::processWalking);
    mActionMap[A_SHIKADI_POLE_ZAPS] = static_cast<GASOFctr>(&CShikadi::processPoleZaps);
    mActionMap[A_SHIKADI_STUNNED]   = static_cast<GASOFctr>(&CStunnable::processGettingStunned);
  
    // Adapt this AI
    setupGalaxyObjectOnMap(0x2D10, A_SHIKADI_STAND);
    
    xDirection = LEFT;

    loadPythonScripts("shikadi");
}


bool CShikadi::loadPythonScripts(const std::string &scriptBaseName)
{
#if USE_PYTHON3

    mModule.load( scriptBaseName, JoinPaths(gKeenFiles.gameDir ,"ai") );

    if(!mModule)
        return false;

    mp_processState = static_cast<GASOFctr>(&CShikadi::processPython);
    mProcessFunc.load(mModule, "process");
    mUpdatePlayerCoord.load(mModule, "updatePlayerCoord");

#endif
    return true;
}


void CShikadi::processPython()
{
    #if USE_PYTHON3
    // TODO: Needs to be put into members of class. This part is yet too CPU heavy
    // NOTE: My tests for using Pyton in CG. These will be forwared to a more common function
    // The following will atempt to get a walk instruction from Python process.    
    if (mProcessFunc)
    {
        // create arguments for the python function
        auto args = PyTuple_New(2);
        auto pValueX = PyLong_FromLong(getPosition().x);
        auto pValueY = PyLong_FromLong(getPosition().y);
        PyTuple_SetItem(args, 0, pValueX);
        PyTuple_SetItem(args, 1, pValueY);
        PyObject *pDict = mProcessFunc.call(args);

        // Process return a dictionary of tasks to perform with some values
        if (pDict != nullptr)
        {
            // TODO: The commented still crashes. I think it is related to some DECREF
            PyObject *key, *value;
            Py_ssize_t pos = 0;

            while (PyDict_Next(pDict, &pos, &key, &value))
            {
                PyObject* repr = PyObject_Repr(key);
                PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
                const std::string keyAsStr = PyBytes_AS_STRING(str);

                const long iValue = PyLong_AsLong(value);

                if(keyAsStr == "'walk'")
                {
                    moveXDir(iValue);
                }
                if(keyAsStr == "'dir'")
                {
                    xDirection = iValue;
                }

                Py_XDECREF(repr);
                Py_XDECREF(str);
            }

            Py_XDECREF(pDict);
            Py_XDECREF(args);
        }
    }
    #endif
}


void CShikadi::processStanding()
{
    if(getActionStatus(A_SHIKADI_WALK))
    {
        setAction(A_SHIKADI_WALK);
    }
}

void CShikadi::processWalking()
{    
    // Move normally in the direction
    if( xDirection == RIGHT )
    {
        moveRight( WALK_SPEED );
    }
    else
    {
        moveLeft( WALK_SPEED );
    }

    if( blockedl )
    {
        xDirection = RIGHT;
    }
    else if(blockedr)
    {
        xDirection = LEFT;
    }

    mTimer++;
    if( mTimer < TIME_UNTIL_STAND )
        return;

    mTimer = 0;

    int newX = (xDirection == LEFT) ? getXLeftPos()+(4<<STC) : getXRightPos()-(4<<STC);

    if(hitdetectWithTilePropertyVert(1, newX, getYUpPos(), getYDownPos(), (1<<STC) ))
    {
        if(getProbability(500))
        {
            newX = (newX>>CSF)<<CSF;
            // Spawn a Enemyshot in form electrostatic  charge which goes upwards
            spawnObj( new CEnemyShot(mpMap, 0,
                                     newX, getYUpPos(),
                                     0x2E5A, 0, UP,
                                     150, mSprVar, true) );
            setAction(A_SHIKADI_POLE_ZAPS);
            playSound(SOUND_POLEZAP);
            return;
        }
    }

    if(getProbability(300))
    {
        setAction(A_SHIKADI_STAND);
    }

}

void CShikadi::processPoleZaps()
{    
    if(getActionStatus(A_SHIKADI_WALK))
    {
        setAction(A_SHIKADI_WALK);
    }
}


bool CShikadi::isNearby(CSpriteObject &theObject)
{

#if USE_PYTHON3

    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        const int x = player->getPosition().x;
        const int y = player->getPosition().y;

        auto args = PyTuple_New(2);
        auto pValueX = PyLong_FromLong(x);
        auto pValueY = PyLong_FromLong(y);
        PyTuple_SetItem(args, 0, pValueX);
        PyTuple_SetItem(args, 1, pValueY);

        mUpdatePlayerCoord.call(args);

        Py_XDECREF(args);
    }


#endif

    if( !getProbability(10) )
        return false;

    return true;
}

void CShikadi::getTouchedBy(CSpriteObject &theObject)
{
    if(mIsDead || theObject.mIsDead)
        return;

    CStunnable::getTouchedBy(theObject);

    // Was it a bullet? Than make loose health.
    if( dynamic_cast<CBullet*>(&theObject) )
    {
        mHealth--;
        theObject.mIsDead = true;

        if(mHealth == 0)
        {
            setAction(A_SHIKADI_STUNNED);
            mIsDead = true;
        }
        else
        {
            blink(10);
        }
    }

    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        player->kill(false);
    }
}


int CShikadi::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
    turnAroundOnCliff( x1, x2, y2 );

    return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CShikadi::process()
{
    performCollisions();

    performGravityMid();

    if(!processActionRoutine())
        exists = false;

    (this->*mp_processState)();
}

}
