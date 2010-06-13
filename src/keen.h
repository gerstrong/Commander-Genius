#ifndef __KEEN_H__
#define __KEEN_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifdef TARGET_WIN32
#include <windows.h>
#endif

#include "engine/sounds.h"
#include <string>

#define CSF    9
#define TILE_S			4
#define STC (CSF-TILE_S)

#define WM_MAP_NUM      80

#define MAX_LEVELS_VORTICON     20

// default sprites...when an object is spawned it's sprite is set to this
// sprite. the object AI will immediately reset the sprite frame, so it
// wouldn't really matter what these are...except that it does because
// the width and height of the default sprite will determine exactly when
// the object because active the first time it scrolls onto the screen
// from the top or left. if the default sprite is wrong the object may
// suddenly appear on the screen instead of smoothly scrolling on.

// some directions (mostly for OBJ_ICECHUNK and OBJ_ICEBIT)
#define DUPRIGHT         0
#define DUPLEFT          1
#define DUP              2
#define DDOWN            3
#define DDOWNRIGHT       4
#define DDOWNLEFT        5
#define DLEFT            6
#define DRIGHT           7

// directions for OBJ_EARTHCHUNK
#define EC_UPLEFTLEFT       0             // 22 degrees CC of UP/LEFT
#define EC_UPUPLEFT         1             // 22 degrees C of UP/LEFT
#define EC_UP               2             // straight UP
#define EC_UPUPRIGHT        3             // 22 degrees CC of UP/RIGHT
#define EC_UPRIGHTRIGHT     4             // 22 degrees C of UP/RIGHT
#define EC_DOWNLEFTLEFT     5             // 22 degrees CC of DOWN/LEFT
#define EC_DOWNDOWNLEFT     6             // 22 degrees C of DOWN/LEFT
#define EC_DOWN             7             // straight DOWN
#define EC_DOWNDOWNRIGHT    8             // 22 degrees CC of DOWN/RIGHT
#define EC_DOWNRIGHTRIGHT   9             // 22 degrees C of DOWN/RIGHT

#define EC_UPLEFT           10
#define EC_UPRIGHT          11
#define EC_DOWNLEFT         12
#define EC_DOWNRIGHT        13

// special level codes on worldmap
#define LVLS_SHIP                  20

#define TILE_SWITCH_UP             480
#define TILE_SWITCH_DOWN           493
#define TILE_LIGHTSWITCH           271
#define TILE_EXTENDING_PLATFORM    270

// special object markers
#define NESSIE_PATH             8192
#define NESSIE_WEED            	8448
#define NESSIE_LAND		       	8704

#define MAX_PLAYERS            4

#endif
