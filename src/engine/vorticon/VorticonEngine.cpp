#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <base/GsApp.h>
#include <base/utils/StringUtils.h>

#include "CResourceLoader.h"
#include "common/CBehaviorEngine.h"
#include "fileio/CPatcher.h"
#include "engine/CMessages.h"
#include "sdl/sound/CSound.h"
#include "VorticonEngine.h"


namespace vorticon
{

///
// This is used for loading all the resources of the game the use has chosen.
// It loads graphics, sound and text into the memory
///
bool VorticonEngine::loadResources( const Uint8 flags )
{
    gLogging.ftextOut("Loading Vorticon Engine...<br>");

    CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
    int version = ExeFile.getEXEVersion();
    unsigned char *p_exedata = ExeFile.getRawData();

    if( mEp == 1 && version == 134)
    {
        gLogging.ftextOut("This version of the game is not supported!<br>");
        return false;
    }

    // Patch the EXE-File-Data directly in the memory.
    CPatcher Patcher(ExeFile, g_pBehaviorEngine->m_is_a_mod);
    Patcher.process();

    gTimer.setLPS(DEFAULT_LPS_VORTICON);

    g_pBehaviorEngine->readTeleporterTable(p_exedata);

    if( (flags & LOADGFX) == LOADGFX )
    {
        // Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
        // This will also read the Tile-Properties
        CEGAGraphicsVort graphics(mEp, mDataPath);
        /*mpEGAGraphics.reset( new vorticon::CEGAGraphicsVort(Episode, DataDirectory) );
        if(!mp_EGAGraphics.get())
            return false;*/

        //mpEGAGraphics->loadData( version, p_exedata );
        graphics.loadData(version, p_exedata);
    }

    if( (flags & LOADSTR) == LOADSTR )
    {
        // load the strings.
        CMessages Messages(p_exedata, mEp, version);
        Messages.extractGlobalStrings();
    }

    if( (flags & LOADSND) == LOADSND )
    {
        // Load the sound data
        g_pSound->loadSoundData();
    }

    g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(mEp, p_exedata);

    // If there are patches left that must be apllied later, do it here!
    Patcher.postProcess();

    return true;

}

}
