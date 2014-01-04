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

    g_pResourceLoader->setStyle(PROGRESS_STYLE_BAR);
    const std::string threadname = "Loading Keen " + itoa(mEp);

    struct GalaxyDataLoad : public Action
    {
        const Uint8 mFlags;

        GalaxyDataLoad(const Uint8 flags) :
            mFlags(flags) {}

        int handle()
        {
            CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
            int version = ExeFile.getEXEVersion();
            unsigned char *p_exedata = ExeFile.getRawData();
            const int Episode = ExeFile.getEpisode();

            g_pResourceLoader->setPermilage(10);

            // Patch the EXE-File-Data directly in the memory.
            CPatcher Patcher(ExeFile, g_pBehaviorEngine->m_is_a_mod);
            Patcher.process();

            g_pResourceLoader->setPermilage(50);

            if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (Only EGAGRAPH.CK?)
                CEGAGraphicsGalaxy graphics(ExeFile);
                /*mpEGAGraphics.reset(new CEGAGraphicsGalaxy(ExeFile)); // Path is relative to the data directory
                if(!mpEGAGraphics)
                    return 0;*/

                //mpEGAGraphics->loadData();
                if( !graphics.loadData() )
                    return 0;

                g_pResourceLoader->setPermilage(400);
            }

            if( (mFlags & LOADSTR) == LOADSTR )
            {
                // load the strings.
                CMessages Messages(p_exedata, Episode, version);
                Messages.extractGlobalStrings();
                g_pResourceLoader->setPermilage(450);
            }


            if( (mFlags & LOADSND) == LOADSND )
            {
                gLogging.ftextOut("Loading audio... <br>");
                // Load the sound data
                g_pSound->loadSoundData();
                g_pResourceLoader->setPermilage(900);
                gLogging.ftextOut("Done loading audio.<br>");
            }

            gLogging.ftextOut("Loading game constants...<br>");

            g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(Episode, p_exedata);

            gLogging.ftextOut("Looking for patches...<br>");

            // If there are patches left that must be applied later, do it here!
            Patcher.postProcess();

            gLogging.ftextOut("Done loading the resources...<br>");

            g_pResourceLoader->setPermilage(1000);

            return 1;
        }
    };

    if(g_pResourceLoader->RunLoadAction(new GalaxyDataLoad(flags), threadname) == 0)
    {
        gEventManager.add( new GMQuit() );
        return false;
    }

    return true;
}

}
