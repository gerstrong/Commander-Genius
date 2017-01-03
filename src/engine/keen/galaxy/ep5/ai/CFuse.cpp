/*
 * CAmpton.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CFuse.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"
#include <base/utils/misc.h>
#include <sdl/audio/music/CMusic.h>
#include "../../common/ai/CEnemyShot.h"

/*
$3186W #QED?
 */


namespace galaxy {  
  
  
CFuse::CFuse(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
mTimer(0)
{  
	// Adapt this AI
	m_ActionBaseOffset = 0x3186;
	setActionForce(0);
	//setupGalaxyObjectOnMap(0x3186, 0);
	
	xDirection = LEFT;
	
	mp_Map->mNumFuses++;
	mp_Map->mFuseInLevel = true;

    loadPythonScripts("qed");
}



bool CFuse::loadPythonScripts(const std::string &scriptBaseName)
{
    #if USE_PYTHON3
    auto pModule = gPython.loadModule( scriptBaseName, JoinPaths(gKeenFiles.gameDir ,"ai") );

    const int level = mp_Map->getLevel();

    if (pModule != nullptr)
    {

        // Change the bounding box, so object are easier to touch
        m_BBox.x1 = -(1<<CSF);
        m_BBox.y1 = -(1<<CSF);
        m_BBox.x2 = (2<<CSF);
        m_BBox.y2 = (2<<CSF);

        // Level Text
        {            
            // pFunc is a new reference
            PyObject *pFunc = PyObject_GetAttrString(pModule, "getLevelText");

            if (pFunc && PyCallable_Check(pFunc))
            {
                PyObject *arglist = Py_BuildValue("(i)", level);


                PyObject *pValue = PyObject_CallObject(pFunc, arglist);


                if (pValue != nullptr)
                {                    
                    PyObject *objectsRepresentation = PyObject_Repr(pValue);

                    char *str = PyUnicode_AsUTF8(objectsRepresentation) ;

                    if(str)
                    {
                        std::string message = str;

                        // Because line breaks are not formatted correctly
                        fixNewLine(message);

                        if(!message.empty())
                        {
                            std::string levelText = "LEVEL_TEXT";
                            levelText += itoa(level);
                            gpBehaviorEngine->setMessage(levelText, message);
                        }
                    }

                    Py_DECREF(pValue);
                    Py_DECREF(objectsRepresentation);
                }
                else
                {

                    PyErr_Print();
                    gLogging.ftextOut("Call failed\n");
                    return false;
                }

                Py_DECREF(pFunc);
                Py_DECREF(arglist);
            }
            else
            {
                if (PyErr_Occurred())
                {
                    PyErr_Print();
                }

                gLogging.ftextOut("Cannot find function \"pyMethodStr\"\n");
                return false;
            }

            Py_XDECREF(pFunc);


        }

        // The bitmap of the hint message
        {
            // pFunc is a new reference
            PyObject *pFunc = PyObject_GetAttrString(pModule, "getLevelTextBmp");

            if (pFunc && PyCallable_Check(pFunc))
            {
                PyObject *arglist = Py_BuildValue("(i)", level);


                PyObject *pValue = PyObject_CallObject(pFunc, arglist);


                if (pValue != nullptr)
                {
                    mLevelTestBmp = PyLong_AsLong(pValue);
                    Py_DECREF(pValue);
                }
                else
                {
                    PyErr_Print();
                    gLogging.ftextOut("Call failed\n");
                    return false;
                }

                Py_DECREF(pFunc);
                Py_DECREF(arglist);
            }
            else
            {
                if (PyErr_Occurred())
                {
                    PyErr_Print();
                }

                gLogging.ftextOut("Cannot find function \"pyMethodStr\"\n");
                return false;
            }

            Py_XDECREF(pFunc);


        }

        Py_DECREF(pModule);
    }
    else
    {
        return false;
    }

    Py_Finalize();
#endif

    return true;
}




void CFuse::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;


    if( auto *thePlayer = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        const auto level = mp_Map->getLevel();
        std::string levelText = "LEVEL_TEXT";
        levelText += itoa(level);

        const auto msg = gpBehaviorEngine->getString(levelText);

        if(!msg.empty())
        {
            thePlayer->m_Inventory.Item.m_gem.clear();
            thePlayer->m_Inventory.Item.fuse_levels_completed++;
            mp_Map->mFuseInLevel = false;

            std::vector<CMessageBoxGalaxy*> msgs;

            msgs.push_back( new CMessageBoxBitmapGalaxy(
                                msg,
                                gGraphics.getBitmapFromId(mLevelTestBmp),
                                RIGHT) );

            showMsgVec( msgs );
            dead = true;
        }
    }
    else if( auto *theEnemyShot = dynamic_cast<CEnemyShot*>(&theObject) ) // happens when Keen 9 - Fight against Mortimer
    {
        // Now replace those tiles
        for(int x=m_BBox.x1 ; x<m_BBox.x2 ; x+=(1<<CSF))
        {
            for(int y=m_BBox.y1 ; y<m_BBox.y2 ; y+=(1<<CSF))
            {
                const Uint16 where_x = (getXPosition()+x)>>CSF;
                const Uint16 where_y = (getYPosition()+y)>>CSF;

                mp_Map->setTile(where_x, where_y, 0, true, 1);
            }
        }

        dead = true;
        theEnemyShot->dead = true;
    }

}


void CFuse::process()
{
    // TODO: We might need a scattermLevelTestBmp effect here
	//if(!processActionRoutine())
	  //  exists = false;
}

}
