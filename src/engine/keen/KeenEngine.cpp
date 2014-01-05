#include <base/GsEvent.h>
#include <base/GsLogging.h>
#include <widgets/CMenuController.h>

#include "common/CGameLauncher.h"
#include "core/mode/CGamePassiveMode.h"
#include "KeenEngine.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/CSaveGameController.h"


void KeenEngine::start()
{
    int version;

    CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;

    version = ExeFile.getEXEVersion();

    gLogging.ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", mEp, version/100, version%100);

    if( ExeFile.getHeaderData() == NULL)
    {
        gLogging.textOut(RED, "KeenEngine::loadResources: Could not load data from the EXE File<br>");
        gEventManager.add(new GMSwitchToGameLauncher() );
        return;
    }

    gpResource->setupFilenames(mEp);

    g_pBehaviorEngine->setEpisode(mEp);

    // Load the Resources
    loadResources( LOADALL );
}

void KeenEngine::pumpEvent(const CEvent *evPtr)
{
    GameEngine::pumpEvent(evPtr);

    if( dynamic_cast<const FinishedLoadingResources*>(evPtr) )
    {
        // Now look if there are any old savegames that need to be converted
        CSaveGameController &savedgames = *gpSaveGameController;
        savedgames.setGameDirectory(mDataPath);
        savedgames.setEpisode(mEp);
        savedgames.convertAllOldFormats();

        std::unique_ptr<CGamePassiveMode> passive(new CGamePassiveMode());
        mpGameMode = move(passive);
        mpGameMode->init();
        mOpenedGamePlay = false;
        gMenuController.emptyMenuStack();

        //if(m_start_level == -1) // Starts normally
            //gEventManager.add( new GMSwitchToPassiveMode(mDataPath, mEp) );
        /*else // This happens, when a level was passed as argument when launching CG
            EventContainer.add( new GMSwitchToPlayGameMode(episode, 1,
                                    DataDirectory,
                                    m_start_level) );*/
    }
    /*else
    {
        pumpChild(evPtr);
    }*/

}



