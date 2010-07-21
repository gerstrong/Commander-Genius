/*
 * objenums.h
 *
 *  Created on: 23.10.2009
 *      Author: gerstrong
 */

#ifndef OBJENUMS_H_
#define OBJENUMS_H_

enum object_t{

	// ** objects from KEEN1
	OBJ_NONE,
	OBJ_YORP,
	OBJ_GARG,
	OBJ_VORT,
	OBJ_BUTLER,
	OBJ_TANK,
	OBJ_RAY,                // keen's raygun blast
	OBJ_DOOR,               // an opening door
	OBJ_ICECHUNK,           // ice chunk from ice cannon
	OBJ_ICEBIT,             // piece of shattered ice chunk
	OBJ_PLAYER,
	OBJ_TELEPORTER,        // world map teleporter
	OBJ_ROPE,
	
	// ** objects from KEEN2 (some of these are in ep3 as well)
	OBJ_SCRUB,
	OBJ_TANKEP2,
	OBJ_PLATFORM,
	OBJ_VORTELITE,
	OBJ_SECTOREFFECTOR,
	OBJ_BABY,
	OBJ_EXPLOSION,
	OBJ_EARTHCHUNK,
	OBJ_SPARK,
	
	// ** objects from KEEN3
	OBJ_FOOB,
	OBJ_NINJA,
	OBJ_MEEP,
	OBJ_SNDWAVE,
	OBJ_MOTHER,
	OBJ_FIREBALL,
	OBJ_BALL,
	OBJ_JACK,
	OBJ_PLATVERT,
	OBJ_MESSIE,
	
	OBJ_DEMOMSG,
	
	// the guns that shoot periodically
	OBJ_AUTORAY,
	OBJ_AUTORAY_V,
	OBJ_ICECANNON,
	
	OBJ_GOTPOINTS,		// this thing is the rising point numbers
	OBJ_GHOST,			// ghosted object from map editor
	EGA_BITMAP,		// Used for bitmaps which are loaded into the system. Keen Bitmaps
	OBJ_BRIDGE,
	LAST_OBJ_TYPE
};

#endif /* OBJENUMS_H_ */
