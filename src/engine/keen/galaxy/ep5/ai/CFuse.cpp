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
    // Extra Python script for this AI defined?
    std::string aiscript = JoinPaths(gKeenFiles.gameDir ,"ai");
    aiscript = JoinPaths(aiscript,scriptBaseName);
    aiscript += ".py";
    aiscript = GetFullFileName(aiscript);

    std::string aidir = ExtractDirectory(aiscript);

    Py_Initialize();

    PyObject* programName = PyUnicode_FromString(scriptBaseName.c_str());

    PyRun_SimpleString("import sys");

    const std::string sysPathCommand = "sys.path.append(\"" + aidir + "\")";

    PyRun_SimpleString(sysPathCommand.c_str());

    auto pModule = PyImport_Import(programName);
    Py_DECREF(programName);



    if (pModule != nullptr)
    {
        /*{
            //gpBehaviorEngine->setMessage("LEVEL_TEXT5", "Lindsay Stuff");

            // pFunc is a new reference
            PyObject *pFunc = PyObject_GetAttrString(pModule, "getLevelText");

            if (pFunc && PyCallable_Check(pFunc))
            {
                PyObject *pValue = PyObject_CallObject(pFunc, nullptr);

                if (pValue != nullptr)
                {
                    if( PyObject_IsTrue(pValue) )
                    {
                        value = true;
                    }
                    else
                    {
                        value = false;
                    }
                    Py_DECREF(pValue);
                }
                else
                {
                    Py_DECREF(pFunc);
                    PyErr_Print();
                    gLogging.ftextOut("Call failed\n");
                    return false;
                }
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


            return true;

        }*/

        Py_DECREF(pModule);
    }
    else
    {
#if PYTHON_VERBOSE
        PyErr_Print();
        gLogging.ftextOut("Failed to load \"%s\"\n", aiscript.c_str());
#endif

        return false;
    }

    Py_Finalize();

    return true;
}




void CFuse::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

    /*
    if( auto *thePlayer = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        g_pMusicPlayer->stop();

        thePlayer->m_Inventory.Item.m_gem.clear();
        thePlayer->m_Inventory.Item.fuse_levels_completed++;
        mp_Map->mFuseInLevel = false;

        std::vector<CMessageBoxGalaxy*> msgs;

        bool specialLevel = false;

        const std::string fuse_msg = gpBehaviorEngine->getString( (specialLevel) ? "FUSE_WONDER" : "FUSE_CASUAL");

        g_pSound->playSound( SOUND_FUSE_BREAK, PLAY_PAUSEALL );

        //gEffectController.setupEffect(new CDimDark(8));

        auto evExit = new EventExitLevel(mp_Map->getLevel(), true, false, mSprVar);
        evExit->playSound = true;

        msgs.push_back( new CMessageBoxBitmapGalaxy(
                            fuse_msg,
                            *gGraphics.getBitmapFromStr("KEENTHUMBSUP"),
                            RIGHT,
                            evExit) );

        showMsgVec( msgs );
        dead = false;
    }
*/
}


void CFuse::process()
{
    // TODO: We might need a scatter effect here
	//if(!processActionRoutine())
	  //  exists = false;
}

}
