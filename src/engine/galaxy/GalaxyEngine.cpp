#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <base/GsApp.h>
#include <base/utils/StringUtils.h>

#include "CResourceLoader.h"
#include "GalaxyEngine.h"
#include "common/CBehaviorEngine.h"
#include "fileio/CPatcher.h"
#include "engine/CMessages.h"
#include "sdl/sound/CSound.h"


namespace galaxy
{


///
// This is used for loading all the resources of the game the use has chosen.
// It loads graphics, sound and text into the memory
///
bool GalaxyEngine::loadResources( const Uint8 flags )
{
    gLogging.ftextOut("Loading Galaxy Engine...<br>");

    gTimer.setLPS(DEFAULT_LPS_GALAXY);

    mEngineLoader.setStyle(PROGRESS_STYLE_BAR);
    const std::string threadname = "Loading Keen " + itoa(mEp);

    struct GalaxyDataLoad : public Action
    {
        const Uint8 mFlags;
        CResourceLoaderBackground &mLoader;

        GalaxyDataLoad(const Uint8 flags, CResourceLoaderBackground &loader) :
            mFlags(flags),
            mLoader(loader) {}

        int handle()
        {
            CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
            int version = ExeFile.getEXEVersion();
            unsigned char *p_exedata = ExeFile.getRawData();
            const int Episode = ExeFile.getEpisode();

            mLoader.setPermilage(10);

            // Patch the EXE-File-Data directly in the memory.
            CPatcher Patcher(ExeFile, g_pBehaviorEngine->m_is_a_mod);
            Patcher.process();

            mLoader.setPermilage(50);

            if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (Only EGAGRAPH.CK?)
                CEGAGraphicsGalaxy graphics(ExeFile);
                if( !graphics.loadData() )
                    return 0;

                mLoader.setPermilage(400);
            }

            if( (mFlags & LOADSTR) == LOADSTR )
            {
                // load the strings.
                CMessages Messages(p_exedata, Episode, version);
                Messages.extractGlobalStrings();
                mLoader.setPermilage(450);
            }


            if( (mFlags & LOADSND) == LOADSND )
            {
                gLogging.ftextOut("Loading audio... <br>");
                // Load the sound data
                g_pSound->loadSoundData();
                mLoader.setPermilage(900);
                gLogging.ftextOut("Done loading audio.<br>");
            }

            gLogging.ftextOut("Loading game constants...<br>");

            g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(Episode, p_exedata);

            gLogging.ftextOut("Looking for patches...<br>");

            // If there are patches left that must be applied later, do it here!
            Patcher.postProcess();

            gLogging.ftextOut("Done loading the resources...<br>");

            mLoader.setPermilage(1000);

            gEventManager.add(new FinishedLoadingResources());

            return 1;
        }
    };

    mEngineLoader.RunLoadActionBackground(new GalaxyDataLoad(flags, mEngineLoader));
    mEngineLoader.start();

    return true;
}

}
