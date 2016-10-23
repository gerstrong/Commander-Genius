#ifndef __BE_GAMEVER_H__
#define __BE_GAMEVER_H__

/* This is separate from be_cross.h since the compiled code
 * depends on version-related macros
 */

typedef enum {
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	BE_GAMEVER_KDREAMSC105,
#endif
#ifdef REFKEEN_VER_KDREAMS_ANYEGA_ALL
	BE_GAMEVER_KDREAMSE113,
	BE_GAMEVER_KDREAMSE193,
	BE_GAMEVER_KDREAMSE120,
#endif
#ifdef REFKEEN_VER_CAT3D
	BE_GAMEVER_CAT3D100,
	BE_GAMEVER_CAT3D122,
#endif
#ifdef REFKEEN_VER_CATABYSS
	BE_GAMEVER_CATABYSS113,
	BE_GAMEVER_CATABYSS124,
#endif
#ifdef REFKEEN_VER_CATARM
	BE_GAMEVER_CATARM102,
#endif
#ifdef REFKEEN_VER_CATAPOC
	BE_GAMEVER_CATAPOC101,
#endif
	BE_GAMEVER_LAST
} BE_GameVer_T;

extern BE_GameVer_T refkeen_current_gamever;
// These MUST have the same order as in the BE_GameVer_T enum
extern const char *refkeen_gamever_strs[BE_GAMEVER_LAST];

#endif // __BE_GAMEVER_H__
