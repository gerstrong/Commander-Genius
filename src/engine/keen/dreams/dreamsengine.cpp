#include "dreamsengine.h"

#include "../../refkeen/be_cross.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/keen/KeenEngine.h"
#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <fileio/CExeFile.h>
#include <fileio/KeenFiles.h>
#include <fileio/CPatcher.h>

// TODO: Ugly wrapper for the refkeen variables used. It serves as interface to C. Might be inmproved in future.
extern "C"
{

void kdreams_exe_main(void);

char *dreamsengine_datapath = nullptr;

extern void RefKeen_Patch_id_ca(void);
extern void RefKeen_Patch_id_us(void);
extern void RefKeen_Patch_id_rf(void);
extern void RefKeen_Patch_kd_play(void);

typedef struct {
    const char *filename;
    int filesize;
    uint32_t crc32;
} BE_GameFileDetails_T;

// Describes a file originally embedded somewhere (in an EXE file)
typedef struct {
    BE_GameFileDetails_T fileDetails;
    int offset; // Location of file/chunk in the EXE
} BE_EmbeddedGameFileDetails_T;


typedef enum {
    BE_EXECOMPRESSION_NONE, BE_EXECOMPRESSION_LZEXE9X
} BE_ExeCompression_T;


/* This is separate from be_cross.h since the compiled code
 * depends on version-related macros
 */

typedef enum {
    BE_GAMEVER_KDREAMSE113,
    BE_GAMEVER_KDREAMSE193,
    BE_GAMEVER_KDREAMSE120,
    BE_GAMEVER_LAST
} BE_GameVer_T;


typedef struct {
    const BE_GameFileDetails_T *reqFiles;
    const BE_EmbeddedGameFileDetails_T *embeddedFiles;
    const char *writableFilesDir;
    const char *exeName;
    int decompExeSize;
    BE_ExeCompression_T compressionType;
    BE_GameVer_T verId;
} BE_GameVerDetails_T;

static const BE_GameFileDetails_T g_be_reqgameverfiles_kdreamse113[] = {
    {"KDREAMS.AUD", 3498, 0x80ac85e5},
    {"KDREAMS.CMP", 14189, 0x97628ca0},
    {"KDREAMS.EGA", 213045, 0x2dc94687},
    {"KDREAMS.EXE", 81619, 0x9dce0a39},
    {"KDREAMS.MAP", 65673, 0x8dce09af},
    {"LAST.SHL", 1634, 0xc0a3560f},
    {0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_kdreamse113[] = {
    {"AUDIODCT.KDR", 1024, 0x8b6116d7, 0x2bc42},
    {"AUDIOHHD.KDR", 340, 0x499e0cbf, 0x24480},
    {"CONTEXT.KDR", 1283, 0x5a33439d, 0x245e0},
    {"EGADICT.KDR", 1024, 0xa69af202, 0x2b446},
    {"EGAHEAD.KDR", 12068, 0xb9d789ee, 0x1e720},
    {"GAMETEXT.KDR", 413, 0xb0df2792, 0x24af0},
    {"MAPDICT.KDR", 1020, 0x9faa7213, 0x2b846},
    {"MAPHEAD.KDR", 11824, 0xb2f36c60, 0x21650},
    {"STORY.KDR", 2526, 0xcafc1d15, 0x24c90},
    {0}
};

static const BE_GameVerDetails_T g_be_gamever_kdreamse113 = {
    g_be_reqgameverfiles_kdreamse113,
    g_be_embeddedgameverfiles_kdreamse113,
    "kdreamse_113",
    "KDREAMS.EXE",
    213536,
    BE_EXECOMPRESSION_LZEXE9X,
    BE_GAMEVER_KDREAMSE113
};

extern void BEL_Cross_ConditionallyAddGameInstallation(const BE_GameVerDetails_T *details, const char *searchdir, const char *descStr);



// (REFKEEN) Used for patching version-specific stuff
uint16_t refkeen_compat_kd_play_objoffset;

extern BE_GameVer_T refkeen_current_gamever;

extern	uint8_t	*EGAhead;
extern	uint8_t	*EGAdict;
extern	uint8_t	*maphead;
extern	uint8_t	*mapdict;
extern	uint8_t	*audiohead;
extern	uint8_t	*audiodict;


}



void setupObjOffset()
{
    switch (refkeen_current_gamever)
    {
    /*case BE_GAMEVER_KDREAMSC105:
        refkeen_compat_kd_play_objoffset = 0x7470;
        break;*/
    case BE_GAMEVER_KDREAMSE113:
        refkeen_compat_kd_play_objoffset = 0x712A;
        break;
    case BE_GAMEVER_KDREAMSE193:
        refkeen_compat_kd_play_objoffset = 0x707A;
        break;
    case BE_GAMEVER_KDREAMSE120:
        refkeen_compat_kd_play_objoffset = 0x734C;
        break;
    }
}


namespace dreams
{


bool extractEmbeddedFilesIntoMemory(const BE_GameVerDetails_T &gameVerDetails)
{
    // TODO: Extract the stuff from the Exe to the memory

    // User these pointers:
    /*
extern	uint8_t	*EGAhead;
extern	uint8_t	*EGAdict;
extern	uint8_t	*maphead;
extern	uint8_t	*mapdict;
extern	uint8_t	*audiohead;
extern	uint8_t	*audiodict;
     */

    return true;
}


///
// This is used for loading all the resources of the game the use has chosen.
// It loads graphics, sound and text into the memory
///
bool DreamsEngine::loadResources()
{
    gLogging.ftextOut("Loading Dreams Engine...<br>");

    gTimer.setLPS(60.0f);

    mEngineLoader.setStyle(PROGRESS_STYLE_BAR);
    const std::string threadname = "Loading Keen Dreams";

    struct DreamsDataLoad : public Action
    {
        CResourceLoaderBackground &mLoader;

        DreamsDataLoad(CResourceLoaderBackground &loader) :
            mLoader(loader) {}

        int handle()
        {
            CExeFile &ExeFile = gKeenFiles.exeFile;
            int version = ExeFile.getEXEVersion();
            unsigned char *p_exedata = ExeFile.getRawData();
            const int Episode = ExeFile.getEpisode();

            mLoader.setPermilage(10);

            // Patch the EXE-File-Data directly in the memory.
            CPatcher Patcher(ExeFile, g_pBehaviorEngine->mPatchFname);
            Patcher.process();

            mLoader.setPermilage(50);

            extractEmbeddedFilesIntoMemory(g_be_gamever_kdreamse113);

            /*if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (Only EGAGRAPH.CK?)
                CEGAGraphicsDreams graphics(ExeFile);
                if( !graphics.loadData() )
                {
                    return 0;
                }

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
                setupAudio();

                mLoader.setPermilage(900);
                gLogging.ftextOut("Done loading audio.<br>");
            }

            gLogging.ftextOut("Loading game constants...<br>");

            g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(Episode, p_exedata);

            gLogging.ftextOut("Looking for patches...<br>");

            // If there are patches left that must be applied later, do it here!
            Patcher.postProcess();

            gLogging.ftextOut("Done loading the resources...<br>");

            mLoader.setPermilage(1000);*/

            gEventManager.add(new FinishedLoadingResources());

            return 1;
        }
    };

    mEngineLoader.RunLoadActionBackground(new DreamsDataLoad(mEngineLoader));
    mEngineLoader.start();

    return true;
}





void DreamsEngine::start()
{
    dreamsengine_datapath = const_cast<char*>(mDataPath.c_str());

    // This function extracts the embedded files. TODO: We should integrate that to our existing system
    // Load the Resources
    loadResources();
    //BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_kdreamse113, dreamsengine_datapath, "Keen Dreams EGA v1.13 (Local)");

    RefKeen_Patch_id_ca();
    RefKeen_Patch_id_us();
    RefKeen_Patch_id_rf();
    setupObjOffset();

    // TODO: This seems to be the exe with main cycle. We need to break it into draw and logic routines.
    kdreams_exe_main();
}


void DreamsEngine::ponder(const float deltaT)
{

}

void DreamsEngine::render()
{

}

}
