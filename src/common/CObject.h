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
#include "CVec.h"

// structures for each AI module's data
#include "engine/vorticon/ai/enemydata.h"
#include "CMap.h"
#include "graphics/CGfxEngine.h"
#include "options.h"

// Enumerations are here
#include "objenums.h"

#define SAFE_DELETE_ARRAY(x) if(x) { delete [] x; x = NULL; }
#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }

// supported by an object and you should look in player[].psupportingobj
// for it's index.
#define PSUPPORTEDBYOBJECT         0

enum direction_t{
	NONE,
	RIGHT,
	LEFT,
	UP,
	DOWN
};

class CObject {
public:
	CObject(CMap *pmap, Uint32 x, Uint32 y, object_t type);
	
	object_t m_type;        		// yorp, vorticon, etc.
	unsigned int m_index;        	// Like an ID for some objects that need this implementation
	char HealthPoints;              // episode 1 style four-shots-to-kill
	bool exists;
	bool onscreen;    				// true=(scrx,scry) position is visible onscreen
	bool hasbeenonscreen;
	unsigned int sprite;      		// which sprite should this object be drawn with
	direction_t m_hDir;				// the direction to where the object is looking/heading to
	direction_t m_vDir;				// same for vertical

	int scrx, scry;           		// x,y pixel position on screen
	
	// Bouncing Boxes
	Uint32 bboxX1, bboxX2;
	Uint32 bboxY1, bboxY2;

	// if zero, priority tiles will not be honored and object will always
	// appear in front of the background
	bool honorPriority;
	bool dontdraw;
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

	bool m_canturnaround;

	// This is used for action format only
	// Hopefully in Vorticons we also can use that structure
	uint16_t m_ActionTicker;
	uint16_t m_ActionNumber;
	size_t m_ActionBaseOffset;
	bool m_climbing;

	void setupObjectType(int Episode);
	void calcBouncingBoxes();
	void calcBouncingBoxeswithPlacement();
	void performCollisionsSameBox();
	void performCollisionOnSlopedTiles();
	void performCollisions();
	void setScrPos( int px, int py );
	bool calcVisibility();
	bool checkforScenario();

	// Moving parts
	void moveToForce(const VectorD2<int> &dir);
	void moveToForce(const int& new_x, const int& new_y);
	void moveDir(const VectorD2<int> &dir);
	void moveToHorizontal(const int& new_x);
	void moveToVertical(const int& new_y);
	void moveTo(const VectorD2<Uint32> &new_loc);
	void moveTo(const int &new_x, const int &new_y);
	void moveXDir(const int& amount, const bool& force = false);
	void moveYDir(const int& amount);
	void moveLeft(const int& amnt, const bool& force = false);
	void moveRight(const int& amnt, const bool& force = false);
	void moveUp(const int& amnt);
	void moveDown(const int& amnt);

	void decreaseXInertia(const int& value);
	void InertiaAndFriction_X();

	virtual void process() { }
	
	bool hitdetect(CObject &hitobject);
	bool hitdetectWithTilePropertyRect(const Uint16 Property, int &lx, int &ly, int &lw, int &lh, const int res);
	bool hitdetectWithTileProperty(const int& Property, const int& x, const int& y);
	virtual void kill();
	void blink(Uint16 frametime);

	// Collision parts
	bool checkSolidR( int x1, int x2, int y1, int y2);
	bool checkSolidL( int x1, int x2, int y1, int y2);
	bool checkSolidU( int x1, int x2, int y1);
	bool checkSolidD( int x1, int x2, int y2);

	// special functions for sloped tiles
	bool checkslopedU( int c, int y1, char blocked);
	bool checkslopedD( int c, int y2, char blocked);
	void moveSlopedTiles( int x, int y1, int y2, int xspeed );
	bool moveSlopedTileDown( int x, int y, int xspeed );
	void moveSlopedTileUp( int x, int y, int xspeed );

	// getters for positions
	Uint32 getXPosition();
	Uint32 getYPosition();
	Uint32 getXLeftPos();
	Uint32 getXRightPos();
	Uint32 getXMidPos();
	Uint32 getYUpPos();
	Uint32 getYDownPos();
	Uint32 getYMidPos();
	
	void processFalling();
	virtual void getTouchedBy(CObject &theObject) {};
	virtual void getShotByRay(object_t &obj_type);
	void kill_intersecting_tile(int mpx, int mpy, CObject &theObject);
	CMap *getMapPtr() { return mp_Map; }

	bool getActionNumber(int16_t ActionNumber);
	bool getActionStatus(int16_t ActionNumber);
	int16_t getActionNumber();
	void setActionForce(size_t ActionNumber);
	void setAction(size_t ActionNumber);
	void setActionSprite();
	void processActionRoutine();

	void draw();

	virtual ~CObject();

protected:
	CMap *mp_Map;

	Uint16 m_blinktime;
	bool m_invincible;
	VectorD2<Uint32> m_Pos; 	// x,y location in map coords, CSFed, represent as 2D Vector

	static int m_number_of_objects;

	// Action Format related stuff
	ActionFormatType m_Action;
};

#endif /* COBJECT_H_ */
