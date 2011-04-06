/*
 * CObjectCollision.cpp
 *
 *  Created on: 01.11.2010
 *      Author: gerstrong
 *
 *  This is the second cpp file for the CObject Class
 *  It is only meant to be used for collisions
 */

#include "CObject.h"
#include "engine/spritedefines.h"

/*
 * \brief Performs collision without bouncing box recalculation
 */
void CObject::performCollisionsSameBox()
{
	const unsigned int abs_x1 = m_Pos.x+m_BBox.x1;
	const unsigned int abs_x2 = m_Pos.x+m_BBox.x2;
	const unsigned int abs_y1 = m_Pos.y+m_BBox.y1;
	const unsigned int abs_y2 = m_Pos.y+m_BBox.y2;

	// Left/Right borders
	blockedl = checkSolidL(abs_x1, abs_x2, abs_y1, abs_y2);
	blockedr = checkSolidR(abs_x1, abs_x2, abs_y1, abs_y2);

	// Upper/Lower borders
	blockedu = checkSolidU(abs_x1, abs_x2, abs_y1);
	blockedd = checkSolidD(abs_x1, abs_x2, abs_y2);
}

/*
 * \brief Calculate Bouncing Boxes with extra placement.
 */
void CObject::calcBouncingBoxeswithPlacement()
{
	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);

    const int diff_y =  m_BBox.y2==0 ? 0 :(int)m_BBox.y2-(int)rSprite.m_bboxY2;

    calcBouncingBoxes();

    moveYDir(diff_y);
}

/*
 * \brief Calculate Bouncing Boxes
 */
void CObject::calcBouncingBoxes()
{
	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);

	m_BBox.x1 = rSprite.m_bboxX1;
	m_BBox.x2 = rSprite.m_bboxX2;
   	m_BBox.y1 = rSprite.m_bboxY1;
   	m_BBox.y2 = rSprite.m_bboxY2;
}

/**
 * So far only used in Galaxy. This is the code for sloped tiles downside
 * This is performed when Keen walks into a sloped tile
 *
 * 0	Fall through		1	Flat
 * 2	Top -> Middle		3	Middle -> bottom
 * 4	Top -> bottom		5	Middle -> top
 * 6	Bottom -> middle	7	Bottom -> top
 * 8	Unused			9	Deadly, can't land on in God mode
 */
void getSlopePointsLowerTile(char slope, int &yb1, int &yb2)
{
	// Calculate the corner y coordinates
	if( slope == 2 )
		yb1 = 0,	yb2 = 256;
	else if( slope == 3 )
		yb1 = 256,	yb2 = 512;
	else if( slope == 4 )
		yb1 = 0,	yb2 = 512;
	else if( slope == 5 )
		yb1 = 256,	yb2 = 0;
	else if( slope == 6 )
		yb1 = 512,	yb2 = 256;
	else if( slope == 7 )
		yb1 = 512,	yb2 = 0;
	else
		yb1 = 0, yb2 = 0;
}

/**
 * So far only used in Galaxy. This is the code for sloped tiles upside
 *
 * 0	Jump through		1	Flat bottom
 * 2	Bottom-> Middle		3	Middle -> top
 * 4	Bottom -> top		5	Middle -> bottom
 * 6	Top -> middle		7	Top -> bottom
 *
 */
void getSlopePointsUpperTile(char slope, int &yb1, int &yb2)
{
	if( slope == 2 )
		yb1 = 512,	yb2 = 256;
	else if( slope == 3 )
		yb1 = 256,	yb2 = 0;
	else if( slope == 4 )
		yb1 = 512,	yb2 = 0;
	else if( slope == 5 )
		yb1 = 256,	yb2 = 512;
	else if( slope == 6 )
		yb1 = 0,	yb2 = 256;
	else if( slope == 7 )
		yb1 = 0,	yb2 = 512;
	else // 1
		yb1 = 0,	yb2 = 0;
}

/*
 * \brief This checks the collision. Very simple pixel based algorithm
 * 		  The collision is per pixel-based
 */
void CObject::performCollisions()
{
	blockedr = blockedl = false;
	blockedu = blockedd = false;

	if ( sprite != BLANKSPRITE )
	{
		calcBouncingBoxes();
		performCollisionsSameBox();
	}
}

// Basic slope move independent of the left or right move
void CObject::adjustSlopedTiles( int x, int y1, int y2 )
{
	// process the sloped tiles here. Galaxy only or special patch!!
	if(g_pBehaviorEngine->getEpisode() > 3)
	{
		if(!moveSlopedTileDown(x, y2))
			moveSlopedTileUp(x, y1);
	}
}

bool CObject::moveSlopedTileDown( int x, int y )
{
	if(yinertia!=0)
		return false;

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	const char slope = TileProperty[mp_Map->at(x>>CSF, y>>CSF)].bup;

	// Check first, if there is a tile on players level
	if( slope >=2 && slope<=7 )
	{
		int yb1, yb2;
		getSlopePointsLowerTile(slope, yb1, yb2);

		// Get relative position of the X corner
		const int L = 512;
		const int x_r = (x%L);

		// get the dy position so the new pos can be computed
		int dy = ((yb2-yb1)*x_r)/L;

		// get the new position of the lower corner
		const int y_rel = yb1 + dy;

		// Get the upper border pos if the tile
		int y_csf;
		y_csf = (y>>CSF)<<CSF;

		const Uint32 newpos_y = y_csf - m_BBox.y2 + y_rel - (1<<STC);
		if( m_Pos.y > newpos_y )
			m_Pos.y = newpos_y;

		return true;
	}
	else
		return false;
}

void CObject::moveSlopedTileUp( int x, int y )
{
	if(yinertia!=0)
		return;

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	char slope = TileProperty[mp_Map->at(x>>CSF, y>>CSF)].bdown;

	// Check first, if there is a tile on players level
	if( slope >=2 && slope<=7 )
	{
		int yb1, yb2;
		getSlopePointsUpperTile(slope, yb1, yb2);

		// Get relative position of the X corner
		const int L = 512;
		const int x_r = (x%L);

		// get the dy position so the new pos can be computed
		int dy = ((yb2-yb1)*x_r)/L;

		// get the new position of the lower corner
		const int y_rel = yb1 + dy;

		// Get the upper border pos if the tile
		int y_csf;
		y_csf = (y>>CSF)<<CSF;

		Uint32 newpos_y = y_csf - m_BBox.y1 + y_rel + (1<<STC);
		if( m_Pos.y < newpos_y )
			m_Pos.y = newpos_y;
	}
}

// returns nonzero if object1 overlaps object2
bool CObject::hitdetect(CObject &hitobject)
{
	unsigned int rect1x1, rect1y1, rect1x2, rect1y2;
	unsigned int rect2x1, rect2y1, rect2x2, rect2y2;

	// get the bounding rectangle of the first object
	rect1x1 = m_Pos.x + m_BBox.x1;
	rect1y1 = m_Pos.y + m_BBox.y1;
	rect1x2 = m_Pos.x + m_BBox.x2;
	rect1y2 = m_Pos.y + m_BBox.y2;

	// get the bounding rectangle of the second object
	rect2x1 = hitobject.getXPosition() + hitobject.m_BBox.x1;
	rect2y1 = hitobject.getYPosition() + hitobject.m_BBox.y1;
	rect2x2 = hitobject.getXPosition() + hitobject.m_BBox.x2;
	rect2y2 = hitobject.getYPosition() + hitobject.m_BBox.y2;

	// find out if the rectangles overlap
	if ((rect1x1 <= rect2x1) && (rect1x2 <= rect2x1)) return false;
	if ((rect1x1 >= rect2x2) && (rect1x2 >= rect2x2)) return false;
	if ((rect1y1 <= rect2y1) && (rect1y2 <= rect2y1)) return false;
	if ((rect1y1 >= rect2y2) && (rect1y2 >= rect2y2)) return false;

	return true;
}


/**
 * \brief this new type of hit detection only checks if the foe touches something that has that property tile
 * \param Property The Tile Property we are looking
 * \param from x
 * \return true if detection worked with that tile having the property, else false
 */
bool CObject::hitdetectWithTilePropertyRect(const Uint16 Property, int &lx, int &ly, int &lw, int &lh, const int res)
{
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);

	for( int i=0 ; i<lw ; i+=res )
	{
		for( int j=0 ; j<lh ; j+=res )
		{
			const char behavior = Tile[mp_Map->getPlaneDataAt(1, lx+i, ly+j)].behaviour;
			if(behavior == Property || behavior == Property-128 ) // -128 for foreground properties
			{
				lx = lx+i;
				ly = ly+j;
				return true;
			}
		}
	}
	return false;
}


/**
 * \brief this new type of hit detection only checks if the foe touches something that has that property
 * \param Property The Tile Property we are looking
 * \return true if detection worked with that tile having the property, else false
 */
bool CObject::hitdetectWithTileProperty(const int& Property, const int& x, const int& y)
{
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	const char behavior = Tile[mp_Map->getPlaneDataAt(1, x, y)].behaviour;
	if(behavior == Property || behavior == Property-128 ) // +128 for foreground properties
		return true;
	else
		return false;
}

const int COLISION_RES = (1<<STC);

int CObject::checkSolidR( int x1, int x2, int y1, int y2)
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	int blocker;
	bool vorticon = (g_pBehaviorEngine->getEpisode() <= 3);

	x2 += COLISION_RES;

	if( (x2>>STC) != ((x2>>CSF)<<TILE_S) )
		return false;

	// Check for right from the object
	if(solid)
	{
		for(int c=y1 ; c<=y2 ; c += COLISION_RES)
		{
			blocker = TileProperty[mp_Map->at(x2>>CSF, c>>CSF)].bleft;
			if(blocker)
				return blocker;
		}

		blocker = TileProperty[mp_Map->at(x2>>CSF, y2>>CSF)].bleft;
		if(blocker)
			return blocker;
	}

	if( m_type == OBJ_PLAYER && solid )
	{
		if(vorticon)
		{
			if( x2 >= (int)((mp_Map->m_width-2)<<CSF) ) return 1;
		}
		else
		{
			if( x2 >= (int)((mp_Map->m_width-1)<<CSF) ) return 1;
		}

	}
	else
	{
		if( (Uint16)x2 > ((mp_Map->m_width)<<CSF) )
		{
			exists=false; // Out of map?
			return 1;
		}
	}

	return 0;
}

int CObject::checkSolidL( int x1, int x2, int y1, int y2)
{
	bool vorticon = (g_pBehaviorEngine->getEpisode() <= 3);
	int blocker;
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	x1 -= COLISION_RES;

	// Check for right from the object
	if(solid)
	{
		for(int c=y1 ; c<=y2 ; c += COLISION_RES)
		{
			blocker = TileProperty[mp_Map->at(x1>>CSF, c>>CSF)].bright;
			if(blocker)
				return blocker;
		}

		blocker = TileProperty[mp_Map->at(x1>>CSF, y2>>CSF)].bright;
		if(blocker)
			return blocker;
	}

	// borders
	if( m_type == OBJ_PLAYER && solid )
	{
		if( vorticon && (x1 <= (2<<CSF)) ) return 1;
		else if( x1 <= (1<<CSF) ) return 1;
	}
	else
	{
		if( x1 == 0 )
		{
			exists = false; // Out of map?
			return 1;
		}
	}
	return 0;
}

int CObject::checkSolidU(int x1, int x2, int y1)
{
	bool vorticon = (g_pBehaviorEngine->getEpisode() <= 3);
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	y1 -= COLISION_RES;

	// Check for sloped tiles here. They must be handled differently
	if(!vorticon && solid)
	{
		char blocked;

		if(m_climbing)
		{
			x1 += 4*COLISION_RES;
			x2 -= 4*COLISION_RES;
		}

		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blocked = TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bdown;

			if(blocked == 17 && m_climbing)
				return 0;

			if( blocked >= 2 && blocked <= 7 && checkslopedU(c, y1, blocked))
				return blocked;
		}

		blocked = TileProperty[mp_Map->at(x2>>CSF, y1>>CSF)].bdown;
		if( blocked >= 2 && blocked <= 7 && checkslopedU(x2, y1, blocked ))
			return 1;

		if(blocked == 17 && m_climbing)
			return 0;
	}

	if( ((y1+COLISION_RES)>>STC) != (((y1+COLISION_RES)>>CSF)<<TILE_S)  )
		return 0;

	// Check for right from the object
	if(solid)
	{
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			char blocked = TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bdown;

			if(blocked)
			{
				if(vorticon)
					return blocked;
				else if(blocked == 1)
					return blocked;
			}
		}
	}

	if( y1 <= ( ((m_type == OBJ_PLAYER) ? 2 : 1)<<CSF) )
		return 1;

	return 0;
}

int CObject::checkSolidD( int x1, int x2, int y2 )
{
	bool vorticon = (g_pBehaviorEngine->getEpisode() <= 3);
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	y2 += COLISION_RES;

	// Check for sloped tiles here. They must be handled differently
	if(!vorticon && solid)
	{
		char blockedu;

		if(m_climbing)
		{
			x1 += 4*COLISION_RES;
			x2 -= 4*COLISION_RES;
		}

		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blockedu = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup;

			if( blockedu == 17 && m_climbing)
				return 0;

			if( blockedu >= 2 && blockedu <= 7 && checkslopedD(c, y2, blockedu) )
				return blockedu;
		}

		blockedu = TileProperty[mp_Map->at(x2>>CSF, y2>>CSF)].bup;

		if(blockedu == 17 && m_climbing)
			return 0;

		if( blockedu >= 2 && blockedu <= 7 && checkslopedD(x2, y2, blockedu)  )
		//if( blocked )
			return blockedu;
	}

	if( (y2>>STC) != ((y2>>CSF)<<TILE_S) )
		return false;

	// Check for down from the object
	if(solid)
	{
		char blocked;
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blocked = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup;

			if(blocked)
			{
				if( blocked < 2 || blocked > 7 )
				{
					char blockedd = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bdown;

					if(blockedd == 0 && m_jumpdown)
						return 0;

					// Jump through
					//if( blockedd != 0 )
						//m_jumpdown = false;

					return blocked;
				}
			}
		}

		blocked = TileProperty[mp_Map->at((x2-(1<<STC))>>CSF, y2>>CSF)].bup;
		if(blocked)
		{
			if( blocked < 2 || blocked > 7 )
				return blocked;
		}
	}

	if( (Uint32)y2 > ((mp_Map->m_height)<<CSF) )
		exists=false; // Out of map?

	// This is a special case for foes which can turn around when they walk over an edge before they fall
	if(m_canturnaround &&
		( !TileProperty[mp_Map->at((x1-(1<<STC))>>CSF, (y2+(1<<STC))>>CSF)].bup ||
		  !TileProperty[mp_Map->at((x2+(1<<STC))>>CSF, (y2+(1<<STC))>>CSF)].bup ) )
	{
		blockedl = TileProperty[mp_Map->at((x2+(1<<STC))>>CSF, (y2+(1<<STC))>>CSF)].bup;
		blockedr = TileProperty[mp_Map->at((x1-(1<<STC))>>CSF, (y2+(1<<STC))>>CSF)].bup;

		return 1;
	}

	return false;
}

/* Special slope function for galaxy
	0	Jump through		1	Flat bottom
	2	Bottom-> Middle		3	Middle -> top
	4	Bottom -> top		5	Middle -> bottom
	6	Top -> middle		7	Top -> bottom
 */
bool CObject::checkslopedU( int c, int y1, char blocked)
{
	int yb1, yb2;

	getSlopePointsUpperTile(blocked, yb1, yb2);

	int dy = ((yb2-yb1-32)*(c%512))/512;
	int yh = yb1 + dy;

	return ( (y1%512) < yh );
}

/* Special slope function for galaxy
0	Fall through		1	Flat
2	Top -> Middle		3	Middle -> bottom
4	Top -> bottom		5	Middle -> top
6	Bottom -> middle	7	Bottom -> top
8	Unused				9	Deadly, can't land on in God mode
 */
bool CObject::checkslopedD( int c, int y2, char blocked )
{
	int yb1, yb2;

	getSlopePointsLowerTile(blocked, yb1, yb2);

	int dy = ((yb2-yb1)*(c%512))/512;
	int yh = yb1 + dy - 32;

	return ( y2%512 > yh );
}

void CObject::doBouncingBoxResizal(const BouncingBox& new_BBox)
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	// Setup up the new bouncing box
	m_BBox = new_BBox;
	unsigned int x1 = getXPosition()+m_BBox.x1;
	unsigned int x2 = getXPosition()+m_BBox.x2;
	unsigned int y1 = getYPosition()+m_BBox.y1;
	unsigned int y2 = getYPosition()+m_BBox.y2;


	for(unsigned int c=y1 ; c<=y2 ; c += COLISION_RES)
	{
		// Adjust the left side
		while(TileProperty[mp_Map->at(x1>>CSF, c>>CSF)].bright)
		{
			processMove(1,0);
			x1 = getXPosition()+m_BBox.x1;
		}

		// Adjust the right side
		while(TileProperty[mp_Map->at(x2>>CSF, c>>CSF)].bleft)
		{
			processMove(-1,0);
			x2 = getXPosition()+m_BBox.x2;
		}
	}

	for(unsigned int c=x1 ; c<=x2 ; c += COLISION_RES)
	{
		// Adjust the lower side
		while(TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bup)
		{
			processMove(0,-1);
			y1 = getYPosition()+m_BBox.y1;
		}

		// Adjust the upper side
		while(TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bdown)
		{
			processMove(0,1);
			y2 = getYPosition()+m_BBox.y2;
		}
	}
}

const int MOVE_RES = 1;

void CObject::processMoveBitLeft()
{
	/// Now check the neighboring tile to the left
	const unsigned int x1 = getXPosition()+m_BBox.x1;
	const unsigned int x2 = getXPosition()+m_BBox.x2;
	const unsigned int y1 = getYPosition()+m_BBox.y1;
	const unsigned int y2 = getYPosition()+m_BBox.y2;

	if( (blockedl = checkSolidL(x1, x2, y1, y2)) == true)
		return;

	// if we are here, the tiles aren't blocking us.
	// TODO: Here we need the Object collision part
	m_Pos.x-=MOVE_RES;
	adjustSlopedTiles(x1-(1<<STC), y1, y2);
}

void CObject::processMoveBitRight()
{
	/// Now check the neighboring tile to the right
	const unsigned int x1 = getXPosition()+m_BBox.x1;
	const unsigned int x2 = getXPosition()+m_BBox.x2;
	const unsigned int y1 = getYPosition()+m_BBox.y1;
	const unsigned int y2 = getYPosition()+m_BBox.y2;

	if( (blockedr = checkSolidR(x1, x2, y1, y2)) == true)
		return;

	// if we are here, the tiles aren't blocking us.
	// TODO: Here we need the Object collision part
	m_Pos.x+=MOVE_RES;
	adjustSlopedTiles(x2+(1<<STC), y1, y2);
}

void CObject::processMoveBitUp()
{
	/// Now check the neighboring tile to the up
	const unsigned int x1 = getXPosition()+m_BBox.x1;
	const unsigned int x2 = getXPosition()+m_BBox.x2;
	const unsigned int y1 = getYPosition()+m_BBox.y1;

	if( (blockedu = checkSolidU(x1, x2, y1)) == true)
		return;

	// if we are here, the tiles aren't blocking us.
	// TODO: Here we need the Object collision part
	m_Pos.y-=MOVE_RES;
}

void CObject::processMoveBitDown()
{
	/// Now check the neighboring tile to the down
	const unsigned int x1 = getXPosition()+m_BBox.x1;
	const unsigned int x2 = getXPosition()+m_BBox.x2;
	const unsigned int y2 = getYPosition()+m_BBox.y2;

	if( ( blockedd = checkSolidD(x1, x2, y2) ) == true )
		return;

	// if we are here, the tiles aren't blocking us.
	// TODO: Here we need the Object collision part
	m_Pos.y+=MOVE_RES;
}

void CObject::processMove(const VectorD2<int>& dir)
{
	processMove(dir.x, dir.y);
}

void CObject::processMove(const int xoff, const int yoff)
{
	// Let's check if we have to move left or right
	if(xoff>0)
	{
		// move right
		for(int c=0 ; c<xoff ; c+=MOVE_RES )
			processMoveBitRight();
	}
	else if(xoff<0)
	{
		// move right
		for(int c=0 ; c<-xoff ; c+=MOVE_RES )
			processMoveBitLeft();
	}

	// Let's check if we have to move up or down
	if(yoff>0)
	{
		// move down
		for(int c=0 ; c<yoff ; c+=MOVE_RES )
			processMoveBitDown();
	}
	else if(yoff<0)
	{
		// move right
		for(int c=0 ; c<-yoff ; c+=MOVE_RES )
			processMoveBitUp();
	}
}

void CObject::processEvents()
{
	while(!m_EventCont.empty())
	{
		if( ObjResizeBB* p_ObjResizeBB =  m_EventCont.occurredEvent<ObjResizeBB>())
		{
			doBouncingBoxResizal(p_ObjResizeBB->m_BB);
			m_EventCont.pop_Event();
		}

		if( ObjMove* p_ObjMove =  m_EventCont.occurredEvent<ObjMove>())
		{
			processMove(p_ObjMove->m_Vec);
			m_EventCont.pop_Event();
		}
	}
}
