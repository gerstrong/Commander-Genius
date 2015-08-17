#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "refkeen_config.h" // MUST precede other contents due to e.g., endianness-based ifdefs

// For template implementations of reads/writes of enums from/to 16 little-endian integers...
#ifdef REFKEEN_VER_KDREAMS
#include "kd_def.h"
#elif defined REFKEEN_VER_CAT3D
#include "c3_def.h"
#elif defined REFKEEN_VER_CATADVENTURES
#include "def.h"
#else
#error "FATAL ERROR: No Reflection port game macro is defined!"
#endif

//#include "be_st.h" // For BE_ST_ExitWithErrorMsg

#ifdef REFKEEN_PLATFORM_WINDOWS
#include <shlwapi.h> // SHGetValue
#include <io.h> // mkdir
#endif

//#include "be_cross.h"
#include "crc32/crc32.h"
#include "unlzexe/unlzexe.h"

#define BE_CROSS_PATH_LEN_BOUND 256
#define BE_CROSS_MAX_GAME_INSTALLATIONS 4

#define BE_ALL_REWRITABLE_FILES_DIR "refkeen_internal_files"

// Describes a required file from a specific game version
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

typedef struct {
	const BE_GameFileDetails_T *reqFiles;
	const BE_EmbeddedGameFileDetails_T *embeddedFiles;
	const char *writableFilesDir;
	const char *exeName;
	int decompExeSize;
	BE_ExeCompression_T compressionType;
	BE_GameVer_T verId;
} BE_GameVerDetails_T;

typedef struct {
	const char *descStr;
	char path[BE_CROSS_PATH_LEN_BOUND];
	char writableFilesPath[BE_CROSS_PATH_LEN_BOUND];
	char writableVanillaFilesPath[BE_CROSS_PATH_LEN_BOUND];
	char embeddedRsrcPath[BE_CROSS_PATH_LEN_BOUND];
	BE_GameVer_T verId;
} BE_GameInstallation_T;

BE_GameVer_T refkeen_current_gamever;
// These MUST have the same order as in the BE_GameVer_T enum
const char *refkeen_gamever_strs[BE_GAMEVER_LAST] = {
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	"kdreamsc105",
#endif
#ifdef REFKEEN_VER_KDREAMS_ANYEGA_ALL
	"kdreamse113",
	"kdreamse193",
	"kdreamse120",
#endif
#ifdef REFKEEN_VER_CAT3D
	"cat3d100",
	"cat3d122",
#endif
#ifdef REFKEEN_VER_CATABYSS
	"catabyss113",
	"catabyss124",
#endif
#ifdef REFKEEN_VER_CATARM
	"catarm102",
#endif
#ifdef REFKEEN_VER_CATAPOC
	"catapoc101",
#endif
};

static BE_GameInstallation_T* g_be_selectedGameInstallation;

static BE_GameInstallation_T g_be_gameinstallations[BE_CROSS_MAX_GAME_INSTALLATIONS];
static int g_be_gameinstallations_num;

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
static const BE_GameFileDetails_T g_be_reqgameverfiles_kdreamsc105[] = {
	{"AUDIO.KDR", 3498, 0x80ac85e5},
	{"CGAGRAPH.KDR", 134691, 0x05e32626},
	{"GAMEMAPS.KDR", 65736, 0x4b92df5f},
	{"KDREAMS.EXE", 78253, 0x5af7ce2b},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_kdreamsc105[] = {
	{"AUDIODCT.KDR", 1024, 0x8b6116d7, 0x29c90},
	{"AUDIOHHD.KDR", 340, 0x499e0cbf, 0x20350},
	{"CGADICT.KDR", 1024, 0xaba89759, 0x29494},
	{"CGAHEAD.KDR", 12068, 0x36d48226, 0x1a5f0},
	{"CONTEXT.KDR", 4759, 0x5bae2337, 0x204b0},
	{"GAMETEXT.KDR", 4686, 0x046c5328, 0x21750},
	{"MAPDICT.KDR", 1020, 0xfa8362f3, 0x29894},
	{"MAPHEAD.KDR", 11824, 0x66c122b4, 0x1d520},
	{"STORY.KDR", 2487, 0xed0ea5fe, 0x229a0},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_kdreamsc105 = {
	g_be_reqgameverfiles_kdreamsc105,
	g_be_embeddedgameverfiles_kdreamsc105,
	"kdreamsc_105",
	"KDREAMS.EXE",
	202320,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_KDREAMSC105
};
#endif

#ifdef REFKEEN_VER_KDREAMS_ANYEGA_ALL
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

// Keen Dreams v1.93 and v1.20 actually share the exact same game data,
// minus the EXE (and v1.92 is also the same, except for KDREAMS.CMP).

static const BE_GameFileDetails_T g_be_reqgameverfiles_kdreamse193[] = {
	{"KDREAMS.AUD", 3498, 0x80ac85e5},
	{"KDREAMS.CMP", 14620, 0x1681bc21},
	{"KDREAMS.EGA", 213045, 0x2dc94687},
	{"KDREAMS.EXE", 80715, 0x3d0cd9ed},
	{"KDREAMS.MAP", 65674, 0x78bd24fc},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_kdreamse193[] = {
	{"AUDIODCT.KDR", 1024, 0x8b6116d7, 0x2bbba},
	{"AUDIOHHD.KDR", 340, 0x499e0cbf, 0x23590},
	{"CONTEXT.KDR", 1283, 0x5a33439d, 0x236f0},
	{"EGADICT.KDR", 1024, 0xa69af202, 0x2b3be},
	{"EGAHEAD.KDR", 12068, 0xb9d789ee, 0x1d830},
	{"GAMETEXT.KDR", 4256, 0xbfe72f94, 0x23c00},
	{"MAPDICT.KDR", 1020, 0x6bb0de32, 0x2b7be},
	{"MAPHEAD.KDR", 11824, 0x2b821e29, 0x20760},
	{"STORY.KDR", 2526, 0xcafc1d15, 0x24ca0},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_kdreamse193 = {
	g_be_reqgameverfiles_kdreamse193,
	g_be_embeddedgameverfiles_kdreamse193,
	"kdreamse_193",
	"KDREAMS.EXE",
	213200,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_KDREAMSE193
};

static const BE_GameFileDetails_T g_be_reqgameverfiles_kdreamse120[] = {
	{"KDREAMS.AUD", 3498, 0x80ac85e5},
	{"KDREAMS.CMP", 14620, 0x1681bc21},
	{"KDREAMS.EGA", 213045, 0x2dc94687},
	{"KDREAMS.EXE", 81519, 0xf36f0edf},
	{"KDREAMS.MAP", 65674, 0x78bd24fc},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_kdreamse120[] = {
	{"AUDIODCT.KDR", 1024, 0x8b6116d7, 0x2c26c},
	{"AUDIOHHD.KDR", 340, 0x499e0cbf, 0x23970},
	{"CONTEXT.KDR", 1283, 0x5a33439d, 0x23ad0},
	{"EGADICT.KDR", 1024, 0xa69af202, 0x2ba70},
	{"EGAHEAD.KDR", 12068, 0xb9d789ee, 0x1dc10},
	{"GAMETEXT.KDR", 4256, 0xbfe72f94, 0x23fe0},
	{"MAPDICT.KDR", 1020, 0x6bb0de32, 0x2be70},
	{"MAPHEAD.KDR", 11824, 0x2b821e29, 0x20b40},
	{"STORY.KDR", 2526, 0xcafc1d15, 0x25080},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_kdreamse120 = {
	g_be_reqgameverfiles_kdreamse120,
	g_be_embeddedgameverfiles_kdreamse120,
	"kdreamse_120",
	"KDREAMS.EXE",
	214912,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_KDREAMSE120
};
#endif

#ifdef REFKEEN_VER_CAT3D
/*** v1.00 ***/
static const BE_GameFileDetails_T g_be_reqgameverfiles_cat3d100[] = {
	{"AUDIO.C3D", 5062, 0x074f7525},
	{"EGAGRAPH.C3D", 258007, 0x7b756d01},
	{"GAMEMAPS.C3D", 14288, 0x1d8d501e},
	{"CAT3D.EXE", 78483, 0xe20992c7},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_cat3d100[] = {
	{"AUDIODCT.C3D", 1024, 0xd3dbe849, 0x24064},
	{"AUDIOHHD.C3D", 368, 0xb83933bc, 0x1be60},
	{"EGADICT.C3D", 1024, 0xab94fb6c, 0x24464},
	{"EGAHEAD.C3D", 1437, 0x33772bb0, 0x1bfd0},
	{"INTROSCN.SCN", 4008, 0xec236c5c, 0x1a6c0},
	{"MTEMP.TMP", 618, 0x6b7cc556, 0x1c570},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_cat3d100 = {
	g_be_reqgameverfiles_cat3d100,
	g_be_embeddedgameverfiles_cat3d100,
	"cat3d_100",
	"CAT3D.EXE",
	191536,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_CAT3D100
};

/*** v1.22 ***/
static const BE_GameFileDetails_T g_be_reqgameverfiles_cat3d122[] = {
	{"AUDIO.C3D", 5062, 0x074f7525},
	{"EGAGRAPH.C3D", 256899, 0xbc122f40},
	{"GAMEMAPS.C3D", 14288, 0x1ecb107b},
	{"CAT3D.EXE", 78285, 0xfaa27410},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_cat3d122[] = {
	{"AUDIODCT.C3D", 1024, 0xd3dbe849, 0x241d8},
	{"AUDIOHHD.C3D", 368, 0xb83933bc, 0x1bd10},
	{"EGADICT.C3D", 1024, 0xb26a70a6, 0x245d8},
	{"EGAHEAD.C3D", 1437, 0x3fde00c4, 0x1be80},
	// INTROSCN.SCN isn't displayed in vanilla v1.22, but there's still 
	// allocated and in use, so it's safer to require this chunk
	{"INTROSCN.SCN", 4008, 0xcf9696af, 0x1a570},
	{"MTEMP.TMP", 618, 0x6b7cc556, 0x1c420},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_cat3d122 = {
	g_be_reqgameverfiles_cat3d122,
	g_be_embeddedgameverfiles_cat3d122,
	"cat3d_122",
	"CAT3D.EXE",
	191904,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_CAT3D122
};
#endif

// TODO: Port DEMOCAT/HINTCAT and figure out a way to support the relevant data
// (possibly optionally, except for Abyss v1.13, where INTRO may call DEMOCAT)

#ifdef REFKEEN_VER_CATABYSS
/*** v1.13 ***/
static const BE_GameFileDetails_T g_be_reqgameverfiles_catabyss113[] = {
	{"ARMAPC.ABS", 10489, 0x9562020e},
	{"AUDIO.ABS", 6904, 0x462d2eb2},
	{"CATABYSS.EXE", 82662, 0xbc3b015e},
	{"EGAGRAPH.ABS", 336143, 0x3b2464a2},
	{"GAMEMAPS.ABS", 20433, 0x97ddb354},
	{"LAST.ABS", 1290, 0xc36ba646},
	{"SHP01.ABS", 1376, 0xe04a6d1e},
	{"SHP02.ABS", 15181, 0x0198b8f7},
	{"SHP03.ABS", 13158, 0x0d6b4986},
	{"SHP04.ABS", 12648, 0x3bd5ccbc},
	{"SHP05.ABS", 724, 0x857573eb},
	{"SHP06.ABS", 9182, 0x86ce4004},
	{"SHP07.ABS", 9930, 0x6a22743f},
	{"SHP08.ABS", 398, 0x99404417},
	{"SHP09.ABS", 417, 0x201133a8},
	{"SHP10.ABS", 329, 0xaa51f92a},
	{"SHP11.ABS", 10595, 0x5a95691e},
	{"SHP12.ABS", 9182, 0x378b5984},
	// Looks like DEMOCAT stuff; Let's require these since
	// vanilla INTRO may launch DEMOCAT.
	{"ALTAR.CAT", 15014, 0x833d1ca7},
	{"APC.CAT", 12828, 0x009ec08a},
	{"CEMETRY.CAT", 14983, 0x8cfb3741},
	{"COOKIE.CAT", 16837, 0xee83c4f4},
	{"CRYSTAL.CAT", 15320, 0x16fa18f0},
	{"DRAGONS.CAT", 7107, 0x54ea7eb5},
	{"EGYPT.CAT", 8863, 0xc78451d7},
	{"FLAME.CAT", 7062, 0xe422aa64},
	{"FOLLOW.CAT", 10437, 0xbec4bd75},
	{"GRATE.CAT", 8306, 0x8f7cb569},
	{"GROUPIC.CAT", 20563, 0x93531dd8},
	{"HOMEBBS.CAT", 3233, 0xaf3d0e16},
	{"INFERNO.CAT", 13934, 0x3ddd36e9},
	{"INVITED.CAT", 9723, 0xacc2ce73},
	{"LAST.CAT", 1247, 0x73f1ec9e},
	{"LEGEND.CAT", 10977, 0xc99a5540},
	{"LOUNGE.CAT", 10665, 0xb799f029},
	{"OPEN.CAT", 4445, 0xe90e743b},
	{"ORCMINE.CAT", 8903, 0xf8821eda},
	{"QUEST.CAT", 13076, 0x252de058},
	{"REDHALL.CAT", 8214, 0x0d9bd58e},
	{"SCRIPT.CAT", 174, 0x45973aa6},
	{"TRILOGY.CAT", 11397, 0xe09a1995},
	{"VAULT.CAT", 8737, 0x43d6dba7},
	{"VICTORY.CAT", 7194, 0x70846fc6},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_catabyss113[] = {
	{"AUDIODCT.ABS", 1024, 0xe9088011, 0x26f4c},
	{"AUDIOHHD.ABS", 416, 0xfbfff495, 0x1bc10},
	{"EGADICT.ABS", 1024, 0xbb760f1d, 0x2734c},
	{"EGAHEAD.ABS", 1888, 0x58062b03, 0x1bdb0},
	{"MTEMP.TMP", 848, 0x6517c97d, 0x1c510},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_catabyss113 = {
	g_be_reqgameverfiles_catabyss113,
	g_be_embeddedgameverfiles_catabyss113,
	"catabyss_113",
	"CATABYSS.EXE",
	201120,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_CATABYSS113
};

/*** v1.24 ***/
static const BE_GameFileDetails_T g_be_reqgameverfiles_catabyss124[] = {
	{"ABYSGAME.EXE", 82479, 0x5f0319db},
	{"AUDIO.ABS", 6904, 0x462d2eb2},
	{"EGAGRAPH.ABS", 335994, 0xfd9995ad},
	{"GAMEMAPS.ABS", 20433, 0x97ddb354},
	{"SHP01.ABS", 1376, 0xe04a6d1e},
	{"SHP02.ABS", 14912, 0xffc72620},
	{"SHP03.ABS", 13186, 0x7b73bb7b},
	{"SHP04.ABS", 12648, 0x3bd5ccbc},
	{"SHP05.ABS", 724, 0x857573eb},
	{"SHP06.ABS", 9182, 0x86ce4004},
	{"SHP07.ABS", 9930, 0x6a22743f},
	{"SHP08.ABS", 398, 0x99404417},
	{"SHP09.ABS", 417, 0x201133a8},
	{"SHP10.ABS", 329, 0xaa51f92a},
	{"SHP11.ABS", 10328, 0x08ee65f4},
	{"SHP12.ABS", 1652, 0xbeb87fbd},
	// HINTCAT stuff similar to DEMOCAT; Maybe not accessible via the
	// version of INTRO used in v1.24 (actually named CATABYSS.EXE) but
	// it may be good to require these too.
	{"AQUDUCT.HNT", 7886, 0x66a11ac1},
	{"CEMETRY.HNT", 10376, 0xf7b3f888},
	{"COVEN.HNT", 13550, 0xc3fdd8a4},
	{"CRYPT.HNT", 11233, 0x46ca0a4b},
	{"DEN.HNT", 10520, 0xbea64562},
	{"GARDEN.HNT", 11125, 0x8c2385b6},
	{"HAUNT.HNT", 7743, 0x2b9ef944},
	{"HINTGO1.HNT", 9751, 0x4fa00485},
	{"HINTGO2.HNT", 9902, 0x6640c7b6},
	{"HINTGO3.HNT", 8372, 0x8e743b2c},
	{"INFERNO.HNT", 11658, 0x9d59623a},
	{"MAUSGND.HNT", 11305, 0x70655f30},
	{"MAUSOL.HNT", 12075, 0xee9b8745},
	{"ORCMINE.HNT", 12664, 0x35502713},
	{"PASSAGE.HNT", 10159, 0x126e01a5},
	{"SANCTUM.HNT", 12030, 0x16c785e6},
	{"SCRIPT.HNT", 155, 0x985da39a},
	{"SUBVALT.HNT", 12110, 0xfb4d589d},
	{"TITANS.HNT", 9821, 0xd491ac71},
	{"TROLLS.HNT", 13409, 0x68a6a6e9},
	{"VICTORY.HNT", 7301, 0xb2a16e89},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_catabyss124[] = {
	{"AUDIODCT.ABS", 1024, 0xe9088011, 0x26e3a},
	{"AUDIOHHD.ABS", 416, 0xfbfff495, 0x1bb40},
	{"EGADICT.ABS", 1024, 0x63eb06d3, 0x2723a},
	{"EGAHEAD.ABS", 1881, 0x94967205, 0x1bce0},
	{"MTEMP.TMP", 834, 0x5d9ccfb3, 0x1c440},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_catabyss124 = {
	g_be_reqgameverfiles_catabyss124,
	g_be_embeddedgameverfiles_catabyss124,
	"catabyss_124",
	"ABYSGAME.EXE",
	200848,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_CATABYSS124
};
#endif

#ifdef REFKEEN_VER_CATARM
static const BE_GameFileDetails_T g_be_reqgameverfiles_catarm102[] = {
	{"ARMGAME.EXE", 82198, 0x7bf5e3d5},
	// Looks like ARM_SLIB.ARM is mentioned in DEF.H but unused
	{"AUDIO.ARM", 7104, 0xb6c28bc7},
	{"EGAGRAPH.ARM", 355832, 0x8eea3210},
	{"GAMEMAPS.ARM", 17653, 0x787794bf},
	{"SHP10.ARM", 9930, 0x6a22743f},
	{"SHP11.ARM", 398, 0x99404417},
	{"SHP12.ARM", 417, 0x201133a8},
	{"SHP13.ARM", 329, 0xaa51f92a},
	{"SHP14.ARM", 1688, 0x98e0af99},
	{"SHP1.ARM", 1376, 0xe04a6d1e},
	{"SHP2.ARM", 3991, 0x1b4bb85b},
	{"SHP3.ARM", 15036, 0xe7c6a5a3},
	{"SHP4.ARM", 15568, 0x9fa3faee},
	{"SHP5.ARM", 13234, 0x3f90b11d},
	{"SHP6.ARM", 10861, 0x42faf6e5},
	{"SHP7.ARM", 9414, 0x1a969278},
	{"SHP8.ARM", 749, 0x086438f0},
	{"SHP9.ARM", 9182, 0x86ce4004},
	// HINTCAT stuff
	{"HINT01.HNT", 9664, 0xb6ecee18},
	{"HINT02.HNT", 9833, 0xa01e9cba},
	{"HINT03.HNT", 12060, 0xa8857173},
	{"HINT04.HNT", 10438, 0x60866c28},
	{"HINT05.HNT", 11555, 0x0973da05},
	{"HINT06.HNT", 10339, 0x1e5089d5},
	{"HINT07.HNT", 12888, 0x0bc7121d},
	{"HINT08.HNT", 10400, 0x0b33d707},
	{"HINT09.HNT", 10754, 0x4f787690},
	{"HINT10.HNT", 10262, 0xbddb06e5},
	{"HINT11.HNT", 12006, 0x47104911},
	{"HINT12.HNT", 12757, 0x340d8383},
	{"HINT13.HNT", 11662, 0xe2e1890f},
	{"HINT14.HNT", 9271, 0x74bbe8c7},
	{"HINT15.HNT", 11624, 0x8e842f18},
	{"HINT16.HNT", 12067, 0xffccf406},
	{"HINT17.HNT", 12342, 0xf24371ec},
	{"HINT18.HNT", 8896, 0x18567f5d},
	{"HINT19.HNT", 9308, 0xdb5b1b65},
	{"HINT20.HNT", 7301, 0xb2a16e89},
	{"SCRIPT.HNT", 90, 0x79a7f746},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_catarm102[] = {
	{"AUDIODCT.ARM", 1024, 0x8f1d4dd2, 0x260b0},
	{"AUDIOHHD.ARM", 428, 0x5f863ad2, 0x1db20},
	{"EGADICT.ARM", 1024, 0xab662db8, 0x264b0},
	{"EGAHEAD.ARM", 1977, 0x711cbf10, 0x1dcd0},
	{"MTEMP.TMP", 834, 0x546f00d1, 0x1d7d0},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_catarm102 = {
	g_be_reqgameverfiles_catarm102,
	g_be_embeddedgameverfiles_catarm102,
	"catarm_102",
	"ARMGAME.EXE",
	198304,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_CATARM102
};
#endif

#ifdef REFKEEN_VER_CATAPOC
static const BE_GameFileDetails_T g_be_reqgameverfiles_catapoc101[] = {
	{"APOCGAME.EXE", 82386, 0x48ca6808},
	// No APC_SLIB.APC file was found, although it is mentioned in DEF.H
	{"AUDIO.APC", 7366, 0x23934cec},
	{"EGAGRAPH.APC", 382078, 0x291893c3},
	{"GAMEMAPS.APC", 18855, 0xd317b2c9},
	{"SHP10.APC", 9930, 0x6a22743f},
	{"SHP11.APC", 398, 0x99404417},
	{"SHP12.APC", 417, 0x201133a8},
	{"SHP13.APC", 329, 0xaa51f92a},
	{"SHP14.APC", 1668, 0xa4a8e44e},
	{"SHP1.APC", 1376, 0xe04a6d1e},
	{"SHP2.APC", 6329, 0xff48bc57},
	{"SHP3.APC", 13187, 0x5800b7f5},
	{"SHP4.APC", 13592, 0x1c498a90},
	{"SHP5.APC", 12698, 0xf2fa1802},
	{"SHP6.APC", 11131, 0x110da23b},
	{"SHP7.APC", 13266, 0x9ec74514},
	{"SHP8.APC", 640, 0x864e6d0c},
	{"SHP9.APC", 9182, 0x86ce4004},
	// HINTCAT stuff
	{"HINT01.HNT", 9664, 0x1641c8f0},
	{"HINT02.HNT", 9823, 0x0d319717},
	{"HINT03.HNT", 10822, 0x994d94cd},
	{"HINT04.HNT", 11654, 0xe754554f},
	{"HINT05.HNT", 8021, 0xfd819cab},
	{"HINT06.HNT", 9839, 0x13f601ce},
	{"HINT07.HNT", 10314, 0xa049c277},
	{"HINT08.HNT", 9616, 0xa5990371},
	{"HINT09.HNT", 9074, 0x61e1681e},
	{"HINT10.HNT", 9744, 0x38ae11c9},
	{"HINT11.HNT", 14563, 0x7c233efa},
	{"HINT12.HNT", 9261, 0x714bd15a},
	{"HINT13.HNT", 10056, 0xebf72992},
	{"HINT14.HNT", 6552, 0x5039141c},
	{"HINT15.HNT", 10738, 0x60fd9f81},
	{"HINT16.HNT", 8302, 0xcace81b8},
	{"HINT17.HNT", 8348, 0x00783edb},
	{"HINT18.HNT", 9990, 0x04e56bbe},
	{"HINT19.HNT", 7301, 0xb2a16e89},
	{"SCRIPT.HNT", 88, 0x8dd529b2},
	{0}
};

static const BE_EmbeddedGameFileDetails_T g_be_embeddedgameverfiles_catapoc101[] = {
	{"AUDIODCT.APC", 1024, 0x26658498, 0x2659c},
	{"AUDIOHHD.APC", 452, 0x76adb051, 0x1df80},
	{"EGADICT.APC", 1024, 0xb2ed57fd, 0x2699c},
	{"EGAHEAD.APC", 2049, 0xd7548ed8, 0x1e150},
	{"MTEMP.TMP", 834, 0x90742162, 0x1dc30},
	{0}
};

static const BE_GameVerDetails_T g_be_gamever_catapoc101 = {
	g_be_reqgameverfiles_catapoc101,
	g_be_embeddedgameverfiles_catapoc101,
	"catapoc_101",
	"APOCGAME.EXE",
	200064,
	BE_EXECOMPRESSION_LZEXE9X,
	BE_GAMEVER_CATAPOC101
};
#endif

// C99
BE_FILE_T BE_Cross_IsFileValid(BE_FILE_T fp);
int BE_Cross_seek(BE_FILE_T fp, long int offset, int origin);
int BE_Cross_putc(int character, BE_FILE_T fp);
int BE_Cross_getc(BE_FILE_T fp);
void BE_Cross_close(BE_FILE_T fp);

// WARNING: Do *not* assume this is recursive!!
static void BEL_Cross_mkdir(const char *path)
{
#ifdef REFKEEN_PLATFORM_WINDOWS
	mkdir(path);
#else
	mkdir(path, 0755);
#endif
}

// Opens *existing* file from given directory in a case-insensitive manner
/*static*/ BE_FILE_T BEL_Cross_open_from_dir(const char *filename, bool isOverwriteRequest, const char *searchdir)
{
	/*** TODO - Any reason to save (cache) DIR handles? ***/
	DIR *dir;
	struct dirent *direntry;
	dir = opendir(searchdir);
	if (!dir)
		return NULL;

	char fullpath[BE_CROSS_PATH_LEN_BOUND];

	for (direntry = readdir(dir); direntry; direntry = readdir(dir))
	{
		if (!BE_Cross_strcasecmp(direntry->d_name, filename))
		{
			// Just a little sanity check
			if (strlen(searchdir) + 1 + strlen(direntry->d_name) >= BE_CROSS_PATH_LEN_BOUND)
			{
				closedir(dir);
				return NULL;
			}
			char *fullpathEnd = fullpath + sizeof(fullpath);
			BE_Cross_safeandfastcstringcopy_3strs(fullpath, fullpathEnd, searchdir, "/", direntry->d_name);

			closedir(dir);
			return fopen(fullpath, isOverwriteRequest ? "wb" : "rb");
		}
	}
	closedir(dir);
	// If tried to open for reading, we return NULL, otherwise we attempt create new file
	if (!isOverwriteRequest)
		return NULL;
	char *fullpathEnd = fullpath + sizeof(fullpath);
	char *fullpathPtr = BE_Cross_safeandfastcstringcopy_2strs(fullpath, fullpathEnd, searchdir, "/");
	// Create actual new files with a lower case, just because that's a common pattern in Unix-like setups
	// (basically a modified BE_Cross_safeandfastcstringcopy).
	//
	// Note: fullpathPtr should initially point to an instance of '\0', so fullpathPtr < fullpathEnd.
	char ch;
	do
	{
		ch = *filename++;
		*fullpathPtr++ = BE_Cross_tolower(ch); // This includes the null terminator if there's the room
	} while ((fullpathPtr < fullpathEnd) && ch);
	// These work in case fullpathPtr == fullpathEnd, and also if not
	--fullpathPtr;
	*fullpathPtr = '\0';

	return fopen(fullpath, "wb");
}

static bool BEL_Cross_CheckGameFileDetails(const BE_GameFileDetails_T *details, const char *searchdir)
{
	BE_FILE_T fp = BEL_Cross_open_from_dir(details->filename, false, searchdir);
	if (!fp)
		return false;

	if (details->filesize == BE_Cross_FileLengthFromHandle(fp))
	{
		uint32_t crc32;
		if (!Crc32_ComputeFile(fp, &crc32) && (crc32 == details->crc32))
		{
			BE_Cross_close(fp);
			return true;
		}
	}
	BE_Cross_close(fp);
	return false;
}

// ***ASSUMPTION: descStr points to a C string literal which is never modified nor deleted!!!***
static void BEL_Cross_ConditionallyAddGameInstallation(const BE_GameVerDetails_T *details, const char *searchdir, const char *descStr)
{
	for (const BE_GameFileDetails_T *fileDetailsBuffer = details->reqFiles; fileDetailsBuffer->filename; ++fileDetailsBuffer)
		if (!BEL_Cross_CheckGameFileDetails(fileDetailsBuffer, searchdir))
			return;

	if (g_be_gameinstallations_num >= BE_CROSS_MAX_GAME_INSTALLATIONS)
		BE_ST_ExitWithErrorMsg("BEL_Cross_ConditionallyAddGameInstallation: Too many game installations!");

	BE_GameInstallation_T *gameInstallation = &g_be_gameinstallations[g_be_gameinstallations_num++];
	// If used correctly then these SHOULD have enough space
	BE_Cross_safeandfastcstringcopy(gameInstallation->path, gameInstallation->path+sizeof(gameInstallation->path), searchdir);
	gameInstallation->verId = details->verId;
	gameInstallation->descStr = descStr; // ASSUMPTION: This is a C string literal!!!

	char writableFilesPath[BE_CROSS_PATH_LEN_BOUND];
	char *endPtr = writableFilesPath + sizeof(writableFilesPath);
	BE_Cross_safeandfastcstringcopy_3strs(writableFilesPath, endPtr, BE_ALL_REWRITABLE_FILES_DIR, "/", details->writableFilesDir);
	endPtr = gameInstallation->embeddedRsrcPath + sizeof(gameInstallation->embeddedRsrcPath);
	BE_Cross_safeandfastcstringcopy_2strs(gameInstallation->embeddedRsrcPath, endPtr, writableFilesPath, "/embedded");

	endPtr = gameInstallation->writableVanillaFilesPath + sizeof(gameInstallation->writableVanillaFilesPath);
	BE_Cross_safeandfastcstringcopy_2strs(gameInstallation->writableVanillaFilesPath, endPtr, writableFilesPath, "/vanilla");

	unsigned char *decompexebuffer = NULL;
	char errorMsg[100];

	for (const BE_EmbeddedGameFileDetails_T *embeddedfileDetailsBuffer = details->embeddedFiles; embeddedfileDetailsBuffer->fileDetails.filename; ++embeddedfileDetailsBuffer)
		if (!BEL_Cross_CheckGameFileDetails(&embeddedfileDetailsBuffer->fileDetails, gameInstallation->embeddedRsrcPath))
		{
			if (!decompexebuffer)
			{
				// First time we do this, so create dir (creation isn't recursive)
				BEL_Cross_mkdir(BE_ALL_REWRITABLE_FILES_DIR); // Non-recursive
				BEL_Cross_mkdir(writableFilesPath);
				BEL_Cross_mkdir(gameInstallation->embeddedRsrcPath);

				FILE *exeFp = BEL_Cross_open_from_dir(details->exeName, false, searchdir);
				if (!exeFp)
				{
					snprintf(errorMsg, sizeof(errorMsg), "BEL_Cross_ConditionallyAddGameInstallation: Can't open EXE after checking it!\nFilename: %s", details->exeName);
					BE_ST_ExitWithErrorMsg(errorMsg);
				}

				decompexebuffer = (unsigned char *)malloc(details->decompExeSize);
				if (!decompexebuffer)
				{
					fclose(exeFp);
					snprintf(errorMsg, sizeof(errorMsg), "BEL_Cross_ConditionallyAddGameInstallation: Can't allocate memory for unpacked EXE copy!\nFilename: %s", details->exeName);
					BE_ST_ExitWithErrorMsg(errorMsg);
				}

				bool success;
				switch (details->compressionType)
				{
				case BE_EXECOMPRESSION_NONE:
					success = (fread(decompexebuffer, details->decompExeSize, 1, exeFp) != 1);
					break;
				case BE_EXECOMPRESSION_LZEXE9X:
					success = Unlzexe_unpack(exeFp, decompexebuffer, details->decompExeSize);
					break;
				}

				fclose(exeFp);
				if (!success)
				{
					free(decompexebuffer);
					snprintf(errorMsg, sizeof(errorMsg), "BEL_Cross_ConditionallyAddGameInstallation: Failed to copy EXE in unpacked form!\nFilename: %s", details->exeName);
					BE_ST_ExitWithErrorMsg(errorMsg);
				}
			}
			FILE *outFp = BEL_Cross_open_from_dir(embeddedfileDetailsBuffer->fileDetails.filename, true, gameInstallation->embeddedRsrcPath);
			if (!outFp)
			{
				free(decompexebuffer);
				snprintf(errorMsg, sizeof(errorMsg), "BEL_Cross_ConditionallyAddGameInstallation: Can't open file for writing!\nFilename: %s", embeddedfileDetailsBuffer->fileDetails.filename);
				BE_ST_ExitWithErrorMsg(errorMsg);
			}
			if (fwrite(decompexebuffer + embeddedfileDetailsBuffer->offset, embeddedfileDetailsBuffer->fileDetails.filesize, 1, outFp) != 1)
			{
				fclose(outFp);
				free(decompexebuffer);
				snprintf(errorMsg, sizeof(errorMsg), "BEL_Cross_ConditionallyAddGameInstallation: Can't write to file!\nFilename: %s", embeddedfileDetailsBuffer->fileDetails.filename);
				BE_ST_ExitWithErrorMsg(errorMsg);
			}
			fclose(outFp);
		}

	free(decompexebuffer);
}


// Opens file for reading from a "search path" in a case-insensitive manner
BE_FILE_T BE_Cross_open_for_reading(const char *filename)
{
	// Just in case, at least for now
	BE_FILE_T fp = BEL_Cross_open_from_dir(filename, false, g_be_selectedGameInstallation->writableVanillaFilesPath);
	if (fp)
		return fp;
	return BEL_Cross_open_from_dir(filename, false, g_be_selectedGameInstallation->path);
}

// Opens file for overwriting from a "search path" (if exists) in a case-insensitive manner
BE_FILE_T BE_Cross_open_for_overwriting(const char *filename)
{
	return BEL_Cross_open_from_dir(filename, true, g_be_selectedGameInstallation->writableVanillaFilesPath);
}

// Loads a file originally embedded into the EXE (for DOS) to a newly allocated
// chunk of memory. Should be freed using BE_Cross_free_mem_loaded_file.
// Returns chunk size if successful, or a negative number in case of failure.
int BE_Cross_load_embedded_rsrc_to_mem(const char *filename, void **ptr)
{
	BE_FILE_T fp = BEL_Cross_open_from_dir(filename, false, g_be_selectedGameInstallation->embeddedRsrcPath);
	if (!fp)
		return -1;

	int filesize = BE_Cross_FileLengthFromHandle(fp);
	*ptr = malloc(filesize);
	bool success = (*ptr) && (fread(*ptr, filesize, 1, fp) == 1);
	if (!success)
		free(*ptr); // This is OK with NULL

	fclose(fp);
	return success ? filesize : -1;
}

// Frees file loaded using BE_Cross_load_file_to_mem. Accepts a NULL pointer.
void BE_Cross_free_mem_loaded_embedded_rsrc(void *ptr)
{
	free(ptr);
}

// MICRO-OPTIMIZATION: Not needed for all games
#ifdef REFKEEN_VER_CATADVENTURES
int BE_Cross_GetSortedRewritableFilenames_AsUpperCase(char *outFilenames, int maxNum, int strLenBound, const char *suffix)
{
	struct dirent *direntry;
	size_t sufLen = strlen(suffix);
	char *nextFilename = outFilenames, *outFilenamesEnd = outFilenames + maxNum*strLenBound, *outFilenamesLast = outFilenamesEnd - strLenBound;
	char *checkFilename, *checkCh;
	DIR *dir;
	// For the sake of consistency we go over same paths as in file open for reading function
	const char *searchpaths[] = {g_be_selectedGameInstallation->writableVanillaFilesPath, g_be_selectedGameInstallation->path};
	for (int loopvar = 0; loopvar < (int)(sizeof(searchpaths)/sizeof(*searchpaths)); ++loopvar)
	{
		dir = opendir(searchpaths[loopvar]);
		if (!dir)
		{
			continue;
		}
		for (direntry = readdir(dir); direntry; direntry = readdir(dir))
		{
			size_t len = strlen(direntry->d_name);
			if ((len < sufLen) || BE_Cross_strcasecmp(direntry->d_name+len-sufLen, suffix))
			{
				continue;
			}
			len -= sufLen;
			/*** Possibly a HACK - Modify d_name itself ***/
			len = (len >= (size_t)strLenBound) ? (strLenBound-1) : len;
			direntry->d_name[len] = '\0';
			// Convert to upper case
			for (checkCh = direntry->d_name; *checkCh; ++checkCh)
			{
				*checkCh = BE_Cross_toupper(*checkCh);
			}
			// This is basically insertion-sort, but we store
			// the *last* entries if there isn't enough room.
			for (checkFilename = outFilenames; checkFilename < nextFilename; checkFilename += strLenBound)
			{
				if (strcmp(checkFilename, direntry->d_name) > 0)
				{
					break;
				}
			}
			// Gone over all inserted entries
			if (checkFilename == nextFilename)
			{
				if (nextFilename < outFilenamesEnd)
				{
					memcpy(nextFilename, direntry->d_name, 1+len);
					nextFilename += strLenBound;
				}
				else
				{
					memmove(outFilenames, outFilenames+strLenBound, strLenBound*(maxNum-1));
					memcpy(outFilenamesLast, direntry->d_name, 1+len);
				}
			}
			// Shift existing entries and insert new one
			else
			{
				// If there's room for another entry, shift "forward"
				if (nextFilename < outFilenamesEnd)
				{
					memmove(checkFilename + strLenBound, checkFilename, outFilenamesEnd-checkFilename-strLenBound);
					memcpy(checkFilename, direntry->d_name, 1+len);
					nextFilename += strLenBound;
				}
				// Otherwise shift "backwards", but only if there's already an entry "smaller" than current one
				else if (checkFilename != outFilenames)
				{
					memmove(outFilenames, outFilenames+strLenBound, (checkFilename-strLenBound)-outFilenames);
					memcpy(checkFilename-strLenBound, direntry->d_name, 1+len);
				}
			};
		}
		closedir(dir);
	}
	return (nextFilename-outFilenames)/strLenBound;
}
#endif

void BE_Cross_PrepareGameInstallations(void)
{
	/*** Some misc. preparation ***/
#ifdef REFKEEN_PLATFORM_UNIX
#if (defined REFKEEN_VER_KDREAMS_CGA_ALL)
	const char *homedir = getenv("HOME");
	char path[BE_CROSS_PATH_LEN_BOUND];
	char *pathEnd = path + sizeof(path);
#endif
#endif

#ifdef REFKEEN_PLATFORM_WINDOWS
#if (defined REFKEEN_VER_CAT3D) || (defined REFKEEN_VER_CATABYSS) || (defined REFKEEN_VER_CATARM) || (defined REFKEEN_VER_CATAPOC)
	char path[BE_CROSS_PATH_LEN_BOUND];
	char gog_catacombs_path[BE_CROSS_PATH_LEN_BOUND];
	DWORD dwType = 0;
	DWORD dwSize = sizeof(gog_catacombs_path);
	LSTATUS status = SHGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\GOG.COM\\GOGCATACOMBSPACK", "PATH", &dwType, gog_catacombs_path, &dwSize);
	bool isGogCatacombsPathFound = ((status == ERROR_SUCCESS) && (dwType == REG_SZ));
	char *gog_catacombs_path_end;
	if (isGogCatacombsPathFound)
	{
		path_gog_catacombs_prefix_end = path/*NOT gog_catacombs_path*/ + strlen(gog_catacombs_path);
	}
#endif
#endif

	/*** Now handling each version separately ***/

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_kdreamsc105, ".", "Keen Dreams CGA v1.05 (Local)");
#ifdef REFKEEN_PLATFORM_UNIX
	if (homedir)
	{
		BE_Cross_safeandfastcstringcopy_2strs(path, pathEnd, homedir, "/.steam/steam/SteamApps/common/Keen Dreams/cga");
		BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_kdreamsc105, path, "Keen Dreams CGA v1.05 (Steam)");
	}
#endif
#endif

#ifdef REFKEEN_VER_KDREAMS_ANYEGA_ALL
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_kdreamse113, ".", "Keen Dreams EGA v1.13 (Local)");
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_kdreamse193, ".", "Keen Dreams EGA v1.93 (Local)");
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_kdreamse120, ".", "Keen Dreams EGA v1.20 (Local)");
#endif

#ifdef REFKEEN_VER_CAT3D
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_cat3d100, ".", "Catacomb 3-D v1.00 (Local)");
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_cat3d122, ".", "Catacomb 3-D v1.22 (Local)");
#ifdef REFKEEN_PLATFORM_WINDOWS
	if (isGogCatacombsPathFound)
	{
		memcpy(path, gog_catacombs_path, sizeof(path));
		BE_Cross_safeandfastcstringcopy(path_gog_catacombs_prefix_end, ptrEnd, "\\Cat3D");
		BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_cat3d122, path, "Catacomb 3-D v1.22 (GOG.com)");
	}
#endif
#endif

#ifdef REFKEEN_VER_CATABYSS
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_catabyss113, ".", "Catacomb Abyss v1.13 (Local)");
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_catabyss124, ".", "Catacomb Abyss v1.24 (Local)");
#ifdef REFKEEN_PLATFORM_WINDOWS
	if (isGogCatacombsPathFound)
	{
		memcpy(path, gog_catacombs_path, sizeof(path));
		BE_Cross_safeandfastcstringcopy(path_gog_catacombs_prefix_end, ptrEnd, "\\Abyss");
		BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_catabyss124, path, "Catacomb Abyss v1.24 (GOG.com)");
	}
#endif
#endif

#ifdef REFKEEN_VER_CATARM
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_catarm102, ".", "Catacomb Armageddon v1.02 (Local)");
#ifdef REFKEEN_PLATFORM_WINDOWS
	if (isGogCatacombsPathFound)
	{
		memcpy(path, gog_catacombs_path, sizeof(path));
		BE_Cross_safeandfastcstringcopy(path_gog_catacombs_prefix_end, ptrEnd, "\\Armageddon");
		BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_catarm102, path, "Catacomb Armageddon v1.02 (GOG.com)");
	}
#endif
#endif

#ifdef REFKEEN_VER_CATAPOC
	BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_catapoc101, ".", "Catacomb Apocalypse v1.01 (Local)");
#ifdef REFKEEN_PLATFORM_WINDOWS
	if (isGogCatacombsPathFound)
	{
		memcpy(path, gog_catacombs_path, sizeof(path));
		BE_Cross_safeandfastcstringcopy(path_gog_catacombs_prefix_end, ptrEnd, "\\Apocalypse");
		BEL_Cross_ConditionallyAddGameInstallation(&g_be_gamever_catapoc101, path, "Catacomb Apocalypse v1.01 (GOG.com)");
	}
#endif
#endif
}

// gameVer should be BE_GAMEVER_LAST if no specific version is desired
void BE_Cross_SelectGameInstallation(int gameVerVal)
{
	if (gameVerVal == BE_GAMEVER_LAST)
	{
		if (!g_be_gameinstallations_num)
		{
			BE_ST_ExitWithErrorMsg("BE_Cross_SelectGameInstallation: No compatible game installation found!");
		}
		g_be_selectedGameInstallation = &g_be_gameinstallations[0];
	}
	else
	{
		int gameInstNum;
		for (gameInstNum = 0; gameInstNum < g_be_gameinstallations_num; ++gameInstNum)
		{
			if (g_be_gameinstallations[gameInstNum].verId == gameVerVal)
			{
				break;
			}
		}
		if (gameInstNum == g_be_gameinstallations_num)
		{
			char errorBuffer[80];
			BE_Cross_safeandfastcstringcopy_2strs(errorBuffer, errorBuffer+sizeof(errorBuffer), "BE_Cross_SelectGameInstallation: Can't find game installation: ",  refkeen_gamever_strs[gameVerVal]);
			BE_ST_ExitWithErrorMsg(errorBuffer);
		}
		g_be_selectedGameInstallation = &g_be_gameinstallations[gameInstNum];
	}

	refkeen_current_gamever = g_be_selectedGameInstallation->verId;

	extern void RefKeen_Patch_id_ca(void);
	RefKeen_Patch_id_ca();
	extern void RefKeen_Patch_id_us(void);
	RefKeen_Patch_id_us();
#ifdef REFKEEN_VER_KDREAMS
	extern void RefKeen_Patch_id_rf(void);
	RefKeen_Patch_id_rf();
	extern void RefKeen_Patch_kd_play(void);
	RefKeen_Patch_kd_play();
#endif
#ifdef REFKEEN_VER_CAT3D
	extern void RefKeen_Patch_c3_game(void);
	RefKeen_Patch_c3_game();
	extern void RefKeen_Patch_c3_play(void);
	RefKeen_Patch_c3_play();
#endif
#ifdef REFKEEN_VER_CATABYSS
	extern void RefKeen_Patch_c4_main(void);
	RefKeen_Patch_c4_main();
	extern void RefKeen_Patch_c4_play(void);
	RefKeen_Patch_c4_play();
	extern void RefKeen_Patch_gelib(void);
	RefKeen_Patch_gelib();
#endif
#ifdef REFKEEN_VER_CATADVENTURES
	extern void RefKeen_Patch_intro(void);
	RefKeen_Patch_intro();
#endif
	extern void RefKeen_FillObjStatesWithDOSPointers(void);
	RefKeen_FillObjStatesWithDOSPointers(); // Saved games compatibility

}

int32_t BE_Cross_FileLengthFromHandle(BE_FILE_T fp)
{
	long int origOffset = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long int len = ftell(fp);
	fseek(fp, origOffset, SEEK_SET);
	return len;
}

size_t BE_Cross_readInt8LE(BE_FILE_T fp, void *ptr)
{
	return fread(ptr, 1, 1, fp);
}

size_t BE_Cross_readInt16LE(BE_FILE_T fp, void *ptr)
{
	size_t bytesread = fread(ptr, 1, 2, fp);
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	if (bytesread == 2)
	{
		*(uint16_t *)ptr = BE_Cross_Swap16LE(*(uint16_t *) ptr);
	}
#endif
	return bytesread;
}

size_t BE_Cross_readInt32LE(BE_FILE_T fp, void *ptr)
{
	size_t bytesread = fread(ptr, 1, 4, fp);
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	if (bytesread == 4)
	{
		*(uint32_t *)ptr = BE_Cross_Swap32LE(*(uint32_t *) ptr);
	}
#endif
	return bytesread;
}

size_t BE_Cross_readInt8LEBuffer(BE_FILE_T fp, void *ptr, size_t nbyte)
{
	return fread(ptr, 1, nbyte, fp);
}

size_t BE_Cross_readInt16LEBuffer(BE_FILE_T fp, void *ptr, size_t nbyte)
{
#ifndef REFKEEN_ARCH_BIG_ENDIAN
	return fread(ptr, 1, nbyte, fp);
#else
	size_t result = fread(ptr, 1, nbyte, fp);
	for (uint16_t *currptr = (uint16_t *)ptr, *endptr = currptr + result/2; currptr < endptr; ++currptr)
	{
		*currptr = BE_Cross_Swap16LE(*currptr);
	}
	return result;
#endif
}

size_t BE_Cross_readInt32LEBuffer(BE_FILE_T fp, void *ptr, size_t nbyte)
{
#ifndef REFKEEN_ARCH_BIG_ENDIAN
	return fread(ptr, 1, nbyte, fp);
#else
	size_t result = fread(ptr, 1, nbyte, fp);
	for (uint32_t *currptr = (uint32_t *)ptr, *endptr = currptr + result/4; currptr < endptr; ++currptr)
	{
		*currptr = BE_Cross_Swap32LE(*currptr);
	}
	return result;
#endif
}

// This exists for the EGAHEADs from the Catacombs
size_t BE_Cross_readInt24LEBuffer(BE_FILE_T fp, void *ptr, size_t nbyte)
{
#ifndef REFKEEN_ARCH_BIG_ENDIAN
	return fread(ptr, 1, nbyte, fp);
#else
	size_t result = fread(ptr, 1, nbyte, fp);
	uint8_t tempbyte;
	// Let's ensure there's no buffer overflow in case nbyte is not divisible by 3
	for (uint8_t *currbyteptr = (uint8_t *)ptr, *endbyteptr = currbyteptr + (nbyte/3)*3; currbyteptr < endbyteptr; currbyteptr += 3)
	{
		tempbyte = *currbyteptr;
		*currbyteptr = *(currbyteptr+2);
		*(currbyteptr+2) = tempbyte;
	}
	return result;
#endif
}

size_t BE_Cross_writeInt8LE(BE_FILE_T fp, const void *ptr)
{
	return fwrite(ptr, 1, 1, fp);
}

size_t BE_Cross_writeInt16LE(BE_FILE_T fp, const void *ptr)
{
#ifndef REFKEEN_ARCH_BIG_ENDIAN
	return fwrite(ptr, 1, 2, fp);
#else
	uint16_t val = BE_Cross_Swap16LE(*(uint16_t *) ptr);
	return fwrite(&val, 1, 2, fp);
#endif
}

size_t BE_Cross_writeInt32LE(BE_FILE_T fp, const void *ptr)
{
#ifndef REFKEEN_ARCH_BIG_ENDIAN
	return fwrite(ptr, 1, 4, fp);
#else
	uint32_t val = BE_Cross_Swap32LE(*(uint32_t *) ptr);
	return fwrite(&val, 1, 4, fp);
#endif
}

size_t BE_Cross_writeInt8LEBuffer(BE_FILE_T fp, const void *ptr, size_t nbyte)
{
	return fwrite(ptr, 1, nbyte, fp);
}

size_t BE_Cross_writeInt16LEBuffer(BE_FILE_T fp, const void *ptr, size_t nbyte)
{
#ifndef REFKEEN_ARCH_BIG_ENDIAN
	return fwrite(ptr, 1, nbyte, fp);
#else
	size_t result = 0;
	for (uint16_t *currptr = (uint16_t *)ptr, *endptr = currptr + nbyte/2; currptr < endptr; ++currptr)
	{
		uint16_t val = BE_Cross_Swap16LE(*currptr);
		size_t bytesread = fwrite(&val, 1, 2, fp);
		result += bytesread;
		if (bytesread < 2)
		{
			break;
		}
	}
	return result;
#endif
}

// Template implementation of enum reads/writes
#define BE_CROSS_IMPLEMENT_FP_READWRITE_16LE_FUNCS(ourSampleEnum) \
size_t BE_Cross_read_ ## ourSampleEnum ## _From16LE (BE_FILE_T fp, ourSampleEnum *ptr) \
{ \
	uint16_t val; \
	size_t bytesread = fread(&val, 1, 2, fp); \
	if (bytesread == 2) \
	{ \
		*ptr = (ourSampleEnum)BE_Cross_Swap16LE(val); \
	} \
	return bytesread; \
} \
\
size_t BE_Cross_write_ ## ourSampleEnum ## _To16LE (BE_FILE_T fp, const ourSampleEnum *ptr) \
{ \
	uint16_t val = BE_Cross_Swap16LE((uint16_t)(*ptr)); \
	return fwrite(&val, 1, 2, fp); \
}

BE_CROSS_IMPLEMENT_FP_READWRITE_16LE_FUNCS(SDMode)
BE_CROSS_IMPLEMENT_FP_READWRITE_16LE_FUNCS(SMMode)
BE_CROSS_IMPLEMENT_FP_READWRITE_16LE_FUNCS(ControlType)
BE_CROSS_IMPLEMENT_FP_READWRITE_16LE_FUNCS(classtype)
#if (defined REFKEEN_VER_CAT3D) || (defined REFKEEN_VER_CATADVENTURES)
BE_CROSS_IMPLEMENT_FP_READWRITE_16LE_FUNCS(dirtype)
#endif

size_t BE_Cross_read_boolean_From16LE(BE_FILE_T fp, bool *ptr)
{
	uint16_t val;
	size_t bytesread = fread(&val, 1, 2, fp);
	if (bytesread == 2)
	{
		*ptr = val; // No need to swap byte-order here
	}
	return bytesread;
}

size_t BE_Cross_read_booleans_From16LEBuffer(BE_FILE_T fp, bool *ptr, size_t nbyte)
{
	uint16_t val;
	size_t totalbytesread = 0, currbytesread;
	for (size_t curbyte = 0; curbyte < nbyte; curbyte += 2, ++ptr)
	{
		currbytesread = fread(&val, 1, 2, fp);
		totalbytesread += currbytesread;
		if (currbytesread < 2)
		{
			return totalbytesread;
		}
		*ptr = val; // No need to swap byte-order here
	}
	return totalbytesread;
}


size_t BE_Cross_write_boolean_To16LE(BE_FILE_T fp, const bool *ptr)
{
	uint16_t val = BE_Cross_Swap16LE((uint16_t)(*ptr)); // Better to swap just in case...
	return fwrite(&val, 1, 2, fp);
}

size_t BE_Cross_write_booleans_To16LEBuffer(BE_FILE_T fp, const bool *ptr, size_t nbyte)
{
	uint16_t val;
	size_t totalbyteswritten = 0, currbyteswritten;
	for (size_t curbyte = 0; curbyte < nbyte; curbyte += 2, ++ptr)
	{
		val = BE_Cross_Swap16LE((uint16_t)(*ptr)); // Better to swap just in case...
		currbyteswritten = fwrite(&val, 1, 2, fp);
		totalbyteswritten += currbyteswritten;
		if (currbyteswritten < 2)
		{
			return totalbyteswritten;
		}
	}
	return totalbyteswritten;
}
