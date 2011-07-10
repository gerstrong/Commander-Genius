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

#include "engine/CEventContainer.h"
#include "ActionFormat.h"
#include "CVec.h"
#include "direction.h"
#include "CBehaviorEngine.h"

// structures for each AI module's data
#include "engine/vorticon/ai/enemydata.h"
#include "CMap.h"
#include "graphics/CGfxEngine.h"
#include "options.h"
#include "sdl/sound/CSound.h"
#include "CVec.h"

// Enumerations are here
#include "objenums.h"


#define SAFE_DELETE_ARRAY(x) if(x) { delete [] x; x = NULL; }
#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }

// The bouncing box used by the object which is used to determine the collisions
struct BouncingBox{
	unsigned int x1, x2, y1, y2;
	BouncingBox(unsigned int l_x1 = 0, unsigned int l_x2 = 0,
			unsigned int l_y1 = 0, unsigned int l_y2 = 0 ) :
				x1(l_x1), x2(l_x2),
				y1(l_y1), y2(l_y2) {}

	void operator()(const unsigned int l_x1,
					const unsigned int l_y1,
					const unsigned int l_x2,
					const unsigned int l_y2 )
	{
		x1 = l_x1; x2 = l_x2;
		y1 = l_y1; y2 = l_y2;
	}


	unsigned int Width()
	{	return (x2-x1);	}

	unsigned int Height()
	{	return (y2-y1);	}
};


// Event that will be used to move the objects in the game
struct ObjMove : public CEvent
{
	VectorD2<int> m_Vec;
	ObjMove(const VectorD2<int>& Vector) : m_Vec(Vector) {}
	ObjMove(const int offx, const int offy) : m_Vec(offx, offy) {}
};

// supported by an object and you should look in player[].psupportingobj
// for it's index.
#define PSUPPORTEDBYOBJECT         0


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
	BouncingBox m_BBox;

	// if zero, priority tiles will not be honored and object will always
	// appear in front of the background
	bool honorPriority;
	bool dontdraw;	// tells the object whether to get drawn or not. The object is still existent in this while
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
	bool m_jumped;
	bool m_climbing;
	bool m_jumpdown;

	// This container will held the triggered events of the object
	CEventContainer m_EventCont;

	void setupObjectType(int Episode);
	void calcBouncingBoxes();
	void performCollisionsSameBox();
	void performCollisions();
	void setScrPos( int px, int py );
	bool calcVisibility();
	bool checkforScenario();


	/**
	 * \description This will verify whether object has to fall or not.
	 * 				This function must be called when it might be moving
	 * 				because it also checks the lower sloped tiles cases
	 *
	 * \todo 		This function should also be called by foes in some cases
	 */
	bool verifyForFalling();

	/**
	 * \description	This function will change the direction of an object when
	 * 				it detects a coming gap while moving
	 * \param		speed Amount of CSFed coordinates to move the object back
	 * 				when it over that gap
	 */
	void performCliffStop(const int &speed);



	// Moving parts
	void moveToForce(const VectorD2<int> &dir);
	void moveToForce(const int& new_x, const int& new_y);
	void moveDir(const VectorD2<int> &dir);
	void moveToHorizontal(const int& new_x);
	void moveToVertical(const int& new_y);
	void moveTo(const VectorD2<Uint32> &new_loc);
	void moveTo(const int new_x, const int new_y);
	void moveXDir(const int amount, const bool force = false);
	void moveYDir(const int amount);
	void moveLeft(const int amnt, const bool force = false);
	void moveRight(const int amnt, const bool force = false);
	void moveUp(const int amnt);
	void moveDown(const int amnt);

	// new moving parts
	/**
	 * \brief	This function checks if there is any collision and moves the object safely
	 * \param	dir	The direction where the object has to go to...
	 */

	/**
	 * This setups the object on the Map. This is galaxy engine only so far,
	 */
	void setupGalaxyObjectOnMap(const size_t ActionBaseOffset = 0x0,
								const size_t ActionNumber = 0x0 );
	void processMove(const VectorD2<int>& dir);
	void processMoveBitLeft();
	void processMoveBitRight();
	virtual void processMoveBitDown();
	void processMoveBitUp();
	void processMove(const int xoff, const int yoff);

	/*
	 * \brief As especially in Galaxy some tiles still can get into blocks where they shouldn't
	 *  	  So this function will pull them out. Same method is used in the original games
	 *  	  and will give a more Commander Keen like feeling
	 */
	void processPushOutCollision();

	void decreaseXInertia(const int& value);
	void InertiaAndFriction_X();

	virtual void process() { }
	
	// The object can hold events process them here!
	void processEvents();

	bool hitdetect(CObject &hitobject);
	bool hitdetectWithTilePropertyRect(const Uint16 Property, int &lx, int &ly, int &lw, int &lh, const int res);
	bool hitdetectWithTileProperty(const int& Property, const int& x, const int& y);
	virtual void kill();
	void blink(Uint16 frametime);

	// Collision parts
	/**
	 * \brief 	Those functions check the the collision states and return also a number of what type of block
	 * 			the collision is happening.
	 * \return	returns the number of the block property. 0 means no collision. Any other number depends.
	 * 			1 is blocked, the other depends on the engine. In Keen Galaxy they mostly represent sloped tiles
	 * 			In vorticons the up part have other numbers which represent ice and slippery.
	 */
	int checkSolidR( int x1, int x2, int y1, int y2);
	int checkSolidL( int x1, int x2, int y1, int y2);
	int checkSolidU( int x1, int x2, int y1, const bool push_mode=false );
	int checkSolidD( int x1, int x2, int y2, const bool push_mode=false );

	// special functions for sloped tiles
	bool checkslopedU( int c, int y1, char blocked);
	bool checkslopedD( int c, int y2, char blocked);
	void adjustSlopedTiles( int x, int y1, int y2, const int xspeed );
	bool moveSlopedTileDown( int x, int y, const int xspeed );
	void moveSlopedTileUp( int x, int y, const int xspeed );

	// getters for positions
	VectorD2<Uint32> getPosition() const;
	Uint32 getXPosition() const;
	Uint32 getYPosition() const;
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

	/**
	 *  \description plays certain sound of an object. Stereo will automatically applied when used
	 */
	void playSound( const GameSound snd,
				    const SoundPlayMode mode=PLAY_NOW );

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

	Uint8 transluceny;
};

#endif /* COBJECT_H_ */
