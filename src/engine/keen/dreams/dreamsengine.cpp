#include "dreamsengine.h"

#include "../../refkeen/be_cross.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/keen/KeenEngine.h"
#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <fileio/CExeFile.h>
#include <fileio/KeenFiles.h>
#include <fileio/CPatcher.h>
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <SDL.h>

#include "../galaxy/res/CAudioGalaxy.h"

#define REFKEEN_VER_KDREAMS_ANYEGA_ALL


dreams::DreamsEngine *gDreamsEngine;


// TODO: Ugly wrapper for the refkeen variables used. It serves as interface to C. Might be inmproved in future.
extern "C"
{


#include "../../refkeen/kdreams/kd_def.h"


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

/*typedef enum {
    BE_GAMEVER_KDREAMSE113,
    BE_GAMEVER_KDREAMSE193,
    BE_GAMEVER_KDREAMSE120,
    BE_GAMEVER_LAST
} BE_GameVer_T;*/


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



// (REFKEEN) Used for patching version-specific stuff
uint16_t refkeen_compat_kd_play_objoffset;

extern	uint8_t	*EGAhead;
extern	uint8_t	*EGAdict;
extern	uint8_t	*maphead;
extern	uint8_t	*mapdict;
extern	uint8_t	*audiohead;
extern	uint8_t	*audiodict;

// (REFKEEN) Used for patching version-specific stuff
extern char *gametext, *context, *story;


extern SDL_Surface *gpBlitSfc;

// if an external event like closing the window is send, this variable
// will force refkeen to close everything.
int gDreamsForceClose;


void BEL_ST_UpdateHostDisplay(SDL_Surface *sfc);

void BE_ST_PollEvents(SDL_Event event);

int gRenderToken = 0; // 0 means only main thread, 1 refkeen prepares data for rendering

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



bool setupAudio()
{
    //const CExeFile &ExeFile = gKeenFiles.exeFile;
    //const unsigned int ep = ExeFile.getEpisode();

    CAudioGalaxy *audio = new CAudioGalaxy();
    //AudioDreams *audio = new AudioDreams();

    if(audio->loadSoundData())
    {
        g_pSound->setupSoundData(audio->sndSlotMapGalaxy[7], audio);
        return true;
    }

    return false;
}

bool extractEmbeddedFilesIntoMemory(const BE_GameVerDetails_T &gameVerDetails)
{
    // Mapping the strings of the filenames to the pointers where we store the embedded data
    std::map< std::string, uint8_t **> dataMap;

    // CA
    dataMap.insert ( std::pair<std::string, uint8_t **>("EGAHEAD.KDR", &EGAhead) );
    dataMap.insert ( std::pair<std::string, uint8_t **>("EGADICT.KDR", &EGAdict) );
    dataMap.insert ( std::pair<std::string, uint8_t **>("MAPHEAD.KDR", &maphead) );
    dataMap.insert ( std::pair<std::string, uint8_t **>("MAPDICT.KDR", &mapdict) );
    dataMap.insert ( std::pair<std::string, uint8_t **>("AUDIOHHD.KDR", &audiohead) );
    dataMap.insert ( std::pair<std::string, uint8_t **>("AUDIODCT.KDR", &audiodict) );

    // US
    dataMap.insert ( std::pair<std::string, uint8_t **>("GAMETEXT.KDR", (uint8_t **) &gametext) );
    dataMap.insert ( std::pair<std::string, uint8_t **>("CONTEXT.KDR", (uint8_t **) &context) );
    dataMap.insert ( std::pair<std::string, uint8_t **>("STORY.KDR", (uint8_t **) &story) );


    CExeFile &ExeFile = gKeenFiles.exeFile;

    uint8_t* headerData = (uint8_t*) ExeFile.getHeaderData();

    for (const BE_EmbeddedGameFileDetails_T *embeddedfileDetailsBuffer = gameVerDetails.embeddedFiles; embeddedfileDetailsBuffer->fileDetails.filename; ++embeddedfileDetailsBuffer)
    {
        auto it = dataMap.find(embeddedfileDetailsBuffer->fileDetails.filename);

        if(it == dataMap.end())
            continue;

        uint8_t **data = it->second;

        const uint dataSize = embeddedfileDetailsBuffer->fileDetails.filesize;

        *data = (uint8_t*) malloc(dataSize);

        memcpy(*data, headerData+embeddedfileDetailsBuffer->offset, dataSize);
    }

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
            //int version = ExeFile.getEXEVersion();
            //unsigned char *p_exedata = ExeFile.getRawData();
            //const int Episode = ExeFile.getEpisode();

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


void DreamsEngine::GameLoop()
{
    // TODO: We should pipe this function to another thread,
    // so the main thread is kept free for
    // all the other lower functions defined through the GsKit.

    // TODO: Create Thread Object here!
    //mpThread = DemoLoop();
    struct GameLoopAction : public Action
    {
        int handle()
        {
            gDreamsForceClose = 0;
            DemoLoop();
            return 0;
        }
    };

    mpPlayLoopAction.reset( new GameLoopAction );
    mpPlayLoopThread.reset(threadPool->start(mpPlayLoopAction.get(), "Dreams Gameloop"));
}


void DreamsEngine::InitGame()
{
    id0_int_t i;

    MM_Startup();

#if 0
    // Handle piracy screen...
    //
    movedata(FP_SEG(PIRACY),(id0_unsigned_t)PIRACY,0xb800,displayofs,4000);
    while (BE_ST_BiosScanCode(0) != sc_Return);
    //while ((bioskey(0)>>8) != sc_Return);
#endif

#if GRMODE == EGAGR
    if (mminfo.mainmem < 335l*1024)
    {
//#pragma warn    -pro
//#pragma warn    -nod
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
        BE_ST_textcolor(7);
#endif
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
        if (refkeen_current_gamever == BE_GAMEVER_KDREAMSE113)
#endif
        {
            BE_ST_textbackground(0);
        }
//#pragma warn    +nod
//#pragma warn    +pro
        BE_ST_clrscr();                       // we can't include CONIO because of a name conflict
//#pragma warn    +nod
//#pragma warn    +pro
        BE_ST_puts ("There is not enough memory available to play the game reliably.  You can");
        BE_ST_puts ("play anyway, but an out of memory condition will eventually pop up.  The");
        BE_ST_puts ("correct solution is to unload some TSRs or rename your CONFIG.SYS and");
        BE_ST_puts ("AUTOEXEC.BAT to free up more memory.\n");
        BE_ST_puts ("Do you want to (Q)uit, or (C)ontinue?");
        //i = bioskey (0);
        //if ( (i>>8) != sc_C)
        i = BE_ST_BiosScanCode (0);
        if (i != sc_C)
            Quit ("");
    }
#endif

    US_TextScreen();

    VW_Startup ();
    RF_Startup ();
    IN_Startup ();
    SD_Startup ();
    US_Startup ();

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
    US_UpdateTextScreen();
#endif

    CA_Startup ();
    US_Setup ();

//
// load in and lock down some basic chunks
//

    CA_ClearMarks ();

    CA_MarkGrChunk(STARTFONT);
    CA_MarkGrChunk(STARTFONTM);
    CA_MarkGrChunk(STARTTILE8);
    CA_MarkGrChunk(STARTTILE8M);
    for (i=KEEN_LUMP_START;i<=KEEN_LUMP_END;i++)
        CA_MarkGrChunk(i);

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
    CA_CacheMarks (NULL);
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
    CA_CacheMarks (NULL, 0);
#endif

    MM_SetLock (&grsegs[STARTFONT],true);
    MM_SetLock (&grsegs[STARTFONTM],true);
    MM_SetLock (&grsegs[STARTTILE8],true);
    MM_SetLock (&grsegs[STARTTILE8M],true);
    for (i=KEEN_LUMP_START;i<=KEEN_LUMP_END;i++)
        MM_SetLock (&grsegs[i],true);

    //CA_LoadAllSounds ();
    setupAudio();

    fontcolor = WHITE;

    US_FinishTextScreen();
}


#define GFX_TEX_WIDTH 320
#define GFX_TEX_HEIGHT 200
#define VGA_TXT_TEX_WIDTH 720
#define VGA_TXT_TEX_HEIGHT 400
//#define EGACGA_TXT_TEX_WIDTH 640
//#define EGACGA_TXT_TEX_HEIGHT 200

void DreamsEngine::setScreenMode(const int mode)
{
    uint sdlTexWidth, sdlTexHeight;

    // Chaning the resolution still breaks the system so we leave at GFX_TEX_WIDTHxGFX_TEX_HEIGHT for now...

    /*switch (mode)
    {
    case 3:
        sdlTexWidth = VGA_TXT_TEX_WIDTH;
        sdlTexHeight = VGA_TXT_TEX_HEIGHT;
        break;
    case 4:
        sdlTexWidth = GFX_TEX_WIDTH;
        sdlTexHeight = GFX_TEX_HEIGHT;
        break;
    case 0xD:
        sdlTexWidth = GFX_TEX_WIDTH;
        sdlTexHeight = GFX_TEX_HEIGHT;
        break;
    case 0xE:
        sdlTexWidth = 2*GFX_TEX_WIDTH;
        sdlTexHeight = GFX_TEX_HEIGHT;
        break;
    }*/

    sdlTexWidth = GFX_TEX_WIDTH;
    sdlTexHeight = GFX_TEX_HEIGHT;

    mDreamsSurface.create(0, sdlTexWidth, sdlTexHeight, RES_BPP, 0, 0, 0, 0);
}




void DreamsEngine::start()
{
    // Global for the legacy refkeen code.
    gDreamsEngine = this;

    gKeenFiles.setupFilenames(7);

    setScreenMode(3);

    dreamsengine_datapath = const_cast<char*>(mDataPath.c_str());

    // This function extracts the embedded files. TODO: We should integrate that to our existing system
    // Load the Resources
    loadResources();

    //RefKeen_Patch_id_ca();
    //RefKeen_Patch_id_us();
    RefKeen_Patch_id_rf();
    setupObjOffset();

    // TODO: This seems to be the exe with main cycle. We need to break it into draw and logic routines.
    InitGame();
    //DemoLoop();
    //kdreams_exe_main();
}


void DreamsEngine::ponder(const float deltaT)
{
    {
        /*if (MousePresent)
        {
            if (INL_GetMouseButtons())
            {
                while (INL_GetMouseButtons())
                {
                    BE_ST_ShortSleep();
                }
                return;
            }
        }

        for (i = 0;i < MaxJoys;i++)
        {
            if (JoysPresent[i])
            {
                if (IN_GetJoyButtonsDB(i))
                {
                    while (IN_GetJoyButtonsDB(i))
                    {
                        BE_ST_ShortSleep();
                    }
                    return;
                }
            }
        }*/
    }




    std::vector<SDL_Event> evVec;
    gInput.readSDLEventVec(evVec);

    for(SDL_Event event : evVec)
    {
        BE_ST_PollEvents(event);
    }


    // Change that mGameState stuff to have more depth in the code
    if(mGameState == INTRO_TEXT) // Where the shareware test is shown
    {
        // If we press any switch to the next section -> where Dreams is really loaded into CGA/EGA mode and show the intro screen
        if( gInput.getPressedAnyCommand() || gInput.mouseClicked() )
        {
            mGameState = INTRO_SCREEN;
            VW_SetScreenMode (GRMODE);
            VW_ClearVideo (BLACK);
            GameLoop();
        }
    }


    //if(mGameState)
}




void DreamsEngine::updateHostDisplay()
{
    SDL_Surface *sfc = mDreamsSurface.getSDLSurface();
    SDL_Surface *blitSfc = gVideoDriver.getBlitSurface();

    BEL_ST_UpdateHostDisplay(sfc);

    SDL_Rect dstRect, srGsRect;
    srGsRect.x = srGsRect.y = 0;
    dstRect.x = dstRect.y = 0;

    srGsRect.w = sfc->w;
    srGsRect.h = sfc->h;
    dstRect.w = blitSfc->w;
    dstRect.h = blitSfc->h;

    // Watermark because I'm still testing the rendering. Sometimes it does not work...
    /*SDL_Rect waterRect;
    waterRect.x = 0;
    waterRect.y = 0;
    waterRect.w = 20;
    waterRect.h = 20;*/

    CVidConfig &vidConf = gVideoDriver.getVidConfig();

    // TODO: We need some render control here!
    //SDL_FillRect(sfc, NULL, SDL_MapRGB(sfc->format, 255, 0, 0));
    //SDL_FillRect(sfc, &waterRect, SDL_MapRGB(sfc->format, 255, 0, 0));

    blitScaled( sfc, srGsRect, blitSfc, dstRect, vidConf.m_ScaleXFilter );

    /*SDL_Surface *sfc = gVideoDriver.getBlitSurface();

    BEL_ST_UpdateHostDisplay(sfc);*/
}



void DreamsEngine::render()
{
    // Wait for Refkeen threads to finish
    //while(gRenderToken != 0);

    updateHostDisplay();

    // Unblock so Refkeen can write for rendering
    //gRenderToken = 1;
}

}
