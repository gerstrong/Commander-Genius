#include "dreamsengine.h"

#include "../../refkeen/be_cross.h"

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


}



namespace dreams
{

void DreamsEngine::start()
{
    dreamsengine_datapath = const_cast<char*>(mDataPath.c_str());

    // This function extracts the embedded files. TODO: We should integrate that to our existing system
    BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_kdreamse113, dreamsengine_datapath, "Keen Dreams EGA v1.13 (Local)");

    RefKeen_Patch_id_ca();
    RefKeen_Patch_id_us();
    RefKeen_Patch_id_rf();
    RefKeen_Patch_kd_play();

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
