#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <base/GsApp.h>
#include <base/utils/StringUtils.h>

#include "CResourceLoader.h"
#include "common/CBehaviorEngine.h"
#include "fileio/CPatcher.h"
#include "fileio/CSaveGameController.h"
#include "engine/CMessages.h"
#include "sdl/sound/CSound.h"
#include "VorticonEngine.h"

#include "CPassiveVort.h"

namespace vorticon
{

///
// This is used for loading all the resources of the game the use has chosen.
// It loads graphics, sound and text into the memory
///
bool VorticonEngine::loadResources( const Uint8 flags )
{
    gLogging.ftextOut("Loading Vorticon Engine...<br>");

    mEngineLoader.setStyle(PROGRESS_STYLE_BAR);


    struct VorticonDataLoad : public Action
    {
        const int mEp;
        const Uint8 mFlags;
        const std::string &mDataPath;

        VorticonDataLoad(const int ep,
                         const Uint8 flags,
                         const std::string &datapath) :
            mEp(ep),
            mFlags(flags),
            mDataPath(datapath) {}

        int handle()
        {
            CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
            int version = ExeFile.getEXEVersion();
            unsigned char *p_exedata = ExeFile.getRawData();

            if( mEp == 1 && version == 134)
            {
                gLogging.ftextOut("This version of the game is not supported!<br>");
                return 0;
            }

            // Patch the EXE-File-Data directly in the memory.
            CPatcher Patcher(ExeFile, g_pBehaviorEngine->m_is_a_mod);
            Patcher.process();

            gTimer.setLPS(DEFAULT_LPS_VORTICON);

            g_pBehaviorEngine->readTeleporterTable(p_exedata);

            if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
                // This will also read the Tile-Properties
                CEGAGraphicsVort graphics(mEp, mDataPath);
                graphics.loadData(version, p_exedata);
            }

            if( (mFlags & LOADSTR) == LOADSTR )
            {
                // load the strings.
                CMessages Messages(p_exedata, mEp, version);
                Messages.extractGlobalStrings();
            }

            if( (mFlags & LOADSND) == LOADSND )
            {
                // Load the sound data
                g_pSound->loadSoundData();
            }

            g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(mEp, p_exedata);

            // If there are patches left that must be apllied later, do it here!
            Patcher.postProcess();

            gEventManager.add(new FinishedLoadingResources());
            return 1;
        }
    };

    mEngineLoader.RunLoadActionBackground(new VorticonDataLoad(mEp, flags, mDataPath));
    mEngineLoader.start();

    return true;

}

void VorticonEngine::pumpEvent(const CEvent *evPtr)
{
    if( dynamic_cast<const FinishedLoadingResources*>(evPtr) )
    {
        // Now look if there are any old savegames that need to be converted
        CSaveGameController &savedgames = *gpSaveGameController;
        savedgames.setGameDirectory(mDataPath);
        savedgames.setEpisode(mEp);
        savedgames.convertAllOldFormats();

        mpGameMode.reset( new vorticon::CPassiveVort() );
        mpGameMode->init();

        /*if(  )
        {
            gEventManager.add( new GMSwitchToGameLauncher(-1, -1) );
        }
        else*/
        {
            mOpenedGamePlay = false;
        }

    }
}

}
