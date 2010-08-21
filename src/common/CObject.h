/*
 * CObject.h
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 *
 *  This class handles the objects like sprites that are draw on the screen
 *  It also handles the AI if any.
 */

#ifndef COBJECT_H_
#define COBJECT_H_

#include "CBehaviorEngine.h"
#include "ActionFormat.h"
#include "objenums.h"
#include "CVec.h"

#define SAFE_DELETE_ARRAY(x) if(x) { delete [] x; x = NULL; }
#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }

// supported by an object and you should look in player[].psupportingobj
// for it's index.
#define PSUPPORTEDBYOBJECT         0

enum direction_t{
	NONE,
	RIGHT,
	RIGHTUP,
	RIGHTDOWN,
	LEFT,
	LEFTUP,
	LEFTDOWN,
	UP,
	DOWN
};

// structures for each AI module's data
#include "../engine/vorticon/ai/enemydata.h"
#include "../common/CMap.h"
#include "../graphics/CGfxEngine.h"
#include "options.h"
#include <vector>

// Enumerations are here
#include "objenums.h"

class CObject {
public:
	CObject(CMap *pmap, Uint32 x, Uint32 y, object_t type);
	
	object_t m_type;        		// yorp, vorticon, etc.
	unsigned int m_index;        	// Like an ID for some objects that need this implementation
	char HealthPoints;                		// episode 1 style four-shots-to-kill
	bool exists;
	bool onscreen;    				// true=(scrx,scry) position is visible onscreen
	bool hasbeenonscreen;
	unsigned int sprite;      			// which sprite should this object be drawn with
	direction_t m_direction;			// the direction to where the object is looking/heading to
	int scrx, scry;           		// x,y pixel position on screen
	
	// Bouncing Boxes
	Uint32 bboxX1, bboxX2;
	Uint32 bboxY1, bboxY2;

	// if zero, priority tiles will not be honored and object will always
	// appear in front of the background
	bool honorPriority;
	bool solid;
	
	bool canbezapped;         // if 0 ray will not stop on hitdetect

	bool inhibitfall;         // if true common_enemy_ai will not do falling

	bool cansupportplayer;
	std::vector<CObject*> *mp_object;
	
	bool blockedl, blockedr, blockedu, blockedd;
	bool onslope;
	bool falling;
	signed int xinertia, yinertia;
	unsigned char xinertiatimer;
	unsigned int psupportingobject, lastsupportingobject;
	bool supportedbyobject;
	
	unsigned char touchPlayer;      // 1=hit detection with player
	unsigned char touchedBy;        // which player was hit
	
	bool dead, dying;
	
	bool needinit;

	// This is used for action format only
	// Hopefully in Vorticons we also can use that structure
	int16_t m_ActionTicker;
	int16_t m_ActionNumber;
	size_t m_ActionBaseOffset;


	void setupObjectType(int Episode);
	void calcBouncingBoxes();
	void performCollisionsSameBox();
	void performCollisionOnSlopedTiles();
	void pushOutofSolidTiles();
	void performCollisions();
	void setScrPos( int px, int py );
	bool calcVisibility();
	bool checkforScenario();
	
	// Moving parts
	void moveToForce(int new_x, int new_y);
	void moveDir(const VectorD2<int> &dir);
	void moveTo(const VectorD2<Uint32> &new_loc);
	void moveTo(int x, int y);
	void moveXDir(int amount, bool force = false);
	void moveYDir(int amount);
	void moveLeft(int amount, bool force = false);
	void moveRight(int amount, bool force = false);
	void moveUp(int amount);
	void moveDown(int amount);

	void decreaseXInertia(int value);
	void InertiaAndFriction_X();

	virtual void process() { }
	
	bool hitdetect(CObject &hitobject);
	virtual void kill();
	void blink(Uint16 frametime);

	// Collision parts
	bool checkSolidR( int x2, int y1, int y2);
	bool checkSolidL( int x1, int y1, int y2);
	bool checkSolidU( int x1, int x2, int y1);
	bool checkSolidD( int x1, int x2, int y2);


	// special functions for sloped tiles
	bool checkslopedU( int c, int y1, char blocked);
	bool checkslopedD( int c, int y2, char blocked);
	void moveSlopedTiles( int x, int y1, int y2, int xspeed );
	bool moveSlopedTileDown( int x, int y, int xspeed );
	void moveSlopedTileUp( int x, int y, int xspeed );

	// getters for positions
	unsigned int getXPosition();
	unsigned int getYPosition();
	unsigned int getXLeftPos();
	unsigned int getXRightPos();
	unsigned int getXMidPos();
	unsigned int getYUpPos();
	unsigned int getYDownPos();
	unsigned int getYMidPos();
	
	void processFalling();
	virtual void getTouchedBy(CObject &theObject) {};
	virtual void getShotByRay();
	void kill_intersecting_tile(int mpx, int mpy, CObject &theObject);
	CMap *getMapPtr() { return mp_Map; }

	int16_t getActionNumber(int16_t ActionNumber);
	void setActionForce(size_t ActionNumber);
	void setAction(size_t ActionNumber);
	void processActionRoutine();

	void draw();

	virtual ~CObject();

protected:
	CMap *mp_Map;

	Uint16 m_blinktime;
	bool m_invincible;
	unsigned int x, y;        			// x,y location in map coords, CSFed

	static int m_number_of_objects;

	// Action Format related stuff
	ActionFormatType m_Action;
};

#endif /* COBJECT_H_ */
