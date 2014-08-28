/*
 * CSpriteObjectCollision.cpp
 *
 *  Created on: 01.11.2010
 *      Author: gerstrong
 *
 *  This is the second cpp file for the CSpriteObject Class
 *  It is only meant to be used for collisions
 */

#include "CSpriteObject.h"
#include "engine/core/spritedefines.h"
#include <base/GsTimer.h>

/*
 * \brief Performs collision without bouncing box recalculation
 */
void CSpriteObject::performCollisionsSameBox()
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
 * \brief Calculate Bouncing Boxes
 */
void CSpriteObject::calcBoundingBoxes()
{
    GsSprite &rSprite = gGraphics.getSprite(0,sprite);

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
void getSlopePointsLowerTile(signed char slope, int &yb1, int &yb2)
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
void getSlopePointsUpperTile(signed  char slope, int &yb1, int &yb2)
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


void CSpriteObject::alignToTile()
{
    if(sprite == BLANKSPRITE)
        return;
    
	// Center the sprites on the map
	int moveup = (1<<CSF)-1;
	moveup -= m_BBox.y2;
    m_Pos.y += moveup;
    processMove(0, 1);
    processMove(0, -moveup);

	performCollisionsSameBox();
	
	// This should ensure no enemies get stuck in walls!
	if(blockedr)
	{
	    processMove( -(1<<CSF), 0 );
	    processMove( +(1<<CSF), 0 );
	}
	if(blockedl)
	{
	    processMove( +(1<<CSF), 0 );
	    processMove( -(1<<CSF), 0 );
	}	
}


/*
 * \brief This checks the collision. Very simple pixel based algorithm
 * 		  The collision is per pixel-based
 */
void CSpriteObject::performCollisions()
{
	blockedr = blockedl = false;
	blockedu = blockedd = false;

	if ( sprite != BLANKSPRITE )
	{
		calcBoundingBoxes();
		performCollisionsSameBox();
	}
}

// Basic slope move independent of the left or right move
void CSpriteObject::adjustSlopedTiles( int x, int y1, int y2, const int xspeed )
{
	if(!solid)
		return;

	// process the sloped tiles here. Galaxy only or special patch!!
	if(g_pBehaviorEngine->getEpisode() > 3)
	{
		if(!moveSlopedTileDown(x, y2, xspeed))
			moveSlopedTileUp(x, y1, xspeed);
	}
}

bool CSpriteObject::moveSlopedTileDown( int x, int y, const int xspeed )
{
	if(yinertia!=0)
		return false;

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	const Sint8 slope = TileProperty[mp_Map->at(x>>CSF, y>>CSF)].bup;

	// Check first, if there is a tile on objects level
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

		// Get the upper border pos of the tile
		int y_csf;
		y_csf = (y>>CSF)<<CSF;

		const Uint32 newpos_y = y_csf - m_BBox.y2 + y_rel - (1<<STC);
		if( m_Pos.y > newpos_y )
			m_Pos.y = newpos_y;

		onslope = true;
	}
	else
		onslope = false;

	return onslope;
}

void CSpriteObject::moveSlopedTileUp( int x, int y, const int xspeed )
{
	if(yinertia!=0)
		return;

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	const Sint8 slope = TileProperty[mp_Map->at(x>>CSF, y>>CSF)].bdown;

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

		onslope = true;
	}
	else
		onslope = false;
}

// returns nonzero if object1 overlaps object2
bool CSpriteObject::hitdetect(CSpriteObject &hitobject)
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


bool CSpriteObject::hitdetectWithTile(const int num, const int lx, const int ly, const int lw, const int lh, const int res)
{
    for( int i=0 ; i<lw ; i+=res )
    {
        for( int j=0 ; j<lh ; j+=res )
        {
            if( mp_Map->getPlaneDataAt(1, lx+i, ly+j) == num )
            {
                return true;
            }
        }
    }

    return false;
}



/**
 * \brief this new type of hit detection only checks if the foe touches something that has that property tile
 * \param Property The Tile Property we are looking
 * \param from x
 * \return true if detection worked with that tile having the property, else false
 */
bool CSpriteObject::hitdetectWithTilePropertyRect(const Uint16 Property, int &lx, int &ly, const int lw, const int lh, const int res)
{
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	
	int i,j;
	Sint8 behavior;

	for( i=0 ; i<lw ; i+=res )
	{
		for( j=0 ; j<lh ; j+=res )
		{
			behavior = Tile[mp_Map->getPlaneDataAt(1, lx+i, ly+j)].behaviour;
			if( (behavior&0x7f) == Property )
			{
				lx = lx+i;	ly = ly+j;
				return true;
			}						
		}
		
		behavior = Tile[mp_Map->getPlaneDataAt(1, lx+i, ly+lh)].behaviour;
		if( (behavior&0x7f) == Property )
		{
			lx = lx+i;	ly = ly+lh;
			return true;
		}								
	}
	
	behavior = Tile[mp_Map->getPlaneDataAt(1, lx+lw, ly+lh)].behaviour;
	if( (behavior&0x7f) == Property )
	{
		lx = lx+lw;	ly = ly+lh;
		return true;
	}							
	
	return false;
}

// Read only version. The detected position is not read. Just returns true and false. That's it!
bool CSpriteObject::hitdetectWithTilePropertyRectRO(const Uint16 Property, const int lx, const int ly, const int lw, const int lh, const int res)
{
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	
	int i,j;
	Sint8 behavior;

	for( i=0 ; i<lw ; i+=res )
	{
		for( j=0 ; j<lh ; j+=res )
		{
			behavior = Tile[mp_Map->getPlaneDataAt(1, lx+i, ly+j)].behaviour;
			if( (behavior&0x7f) == Property )
			    return true;
		}
		
		behavior = Tile[mp_Map->getPlaneDataAt(1, lx+i, ly+lh)].behaviour;
		if( (behavior&0x7f) == Property )
			return true;
	}
	
	behavior = Tile[mp_Map->getPlaneDataAt(1, lx+lw, ly+lh)].behaviour;
	if( (behavior&0x7f) == Property )
		return true;
	
	return false;    
}

bool CSpriteObject::hitdetectWithTilePropertyHor(const Uint16 Property, const int lxl, const int lxr, const int ly, const int res)
{
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	
	int i;
	Sint8 behavior;

	for( i=lxl ; i<lxr ; i+=res )
	{		
		behavior = Tile[mp_Map->getPlaneDataAt(1, i, ly)].behaviour;
		if( (behavior&0x7f) == Property )
			return true;
	}
	
	behavior = Tile[mp_Map->getPlaneDataAt(1, lxr, ly)].behaviour;
	if( (behavior&0x7f) == Property )
		return true;
	
	return false;       
}

bool CSpriteObject::hitdetectWithTilePropertyVert(const Uint16 Property, const int lx, const int lyu, const int lyd, const int res)
{
    	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	
	int i;
	Sint8 behavior;

	for( i=lyu ; i<lyd ; i+=res )
	{		
		behavior = Tile[mp_Map->getPlaneDataAt(1, lx, i)].behaviour;
		if( (behavior&0x7f) == Property )
			return true;
	}
	
	behavior = Tile[mp_Map->getPlaneDataAt(1, lx, lyd)].behaviour;
	if( (behavior&0x7f) == Property )
		return true;
	
	return false;       
}


/**
 * \brief this new type of hit detection only checks if the foe touches something that has that property
 * \param Property The Tile Property we are looking
 * \return true if detection worked with that tile having the property, else false
 */
bool CSpriteObject::hitdetectWithTileProperty(const int Property, const int x, const int y)
{
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	const int tileID = mp_Map->getPlaneDataAt(1, x, y);
	const Sint8 behavior = Tile[tileID].behaviour;
	if( (behavior&0x7F) == Property ) // 0x7F is the mask which covers for foreground properties
		return true;
	else
		return false;
}

bool CSpriteObject::turnAroundOnCliff( int x1, int x2, int y2 )
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	const int x_left = (x1-(1<<STC))>>CSF;
	const int x_right = (x2+(1<<STC))>>CSF;
	const int y_bottom = (y2+(1<<STC))>>CSF;

	const int floorleft = TileProperty[mp_Map->at(x_left, y_bottom)].bup;
	const int floorright = TileProperty[mp_Map->at(x_right, y_bottom)].bup;

    bool isSlope = false;

    if( floorleft == 0 && xDirection == LEFT && floorright>=1 )
	{
	    for(int x=x_left ; x<=x_right ; x++ )
	    {
            const int tile = TileProperty[mp_Map->at(x, y_bottom)].bup;
            if( tile>=2 && tile<=7  )
            {
                isSlope = true;
            }
	    }

        if(isSlope)
        {
            // look further
            if(TileProperty[mp_Map->at(x_left, y_bottom+1)].bup != 0)
            {
                return false;
            }
        }

	
	    blockedl = floorright;
	    return true;
	}

    if( floorright == 0 && xDirection == RIGHT && floorleft==1 )
	{
        for(int x=x_left ; x<=x_right ; x++ )
        {
            const int tile = TileProperty[mp_Map->at(x, y_bottom)].bup;
            if( tile>=2 && tile<=7  )
            {
                isSlope = true;
            }
        }

        if(isSlope)
        {
            // look further
            if(TileProperty[mp_Map->at(x_right, y_bottom+1)].bup != 0)
            {
                return false;
            }
        }


	    blockedr = floorleft;
	    return true;
	}	

	return false;
}


bool CSpriteObject::checkMapBoundaryR(const int x2)
{
	if( (Uint16)x2 > ((mp_Map->m_width)<<CSF) ) // Out of map?
	{
		exists = false; // deactivate it!
		return true;
	}
	return false;
}


bool CSpriteObject::checkMapBoundaryL(const int x1)
{
	if( x1 == 0 ) // Out of map?
	{
		exists = false; // deactivate it!
		return true;
	}

	return false;
}


bool CSpriteObject::checkMapBoundaryU(const int y1)
{
	if( y1 <= (1<<CSF) )
		return true;

	return false;
}


int CSpriteObject::checkSolidR( int x1, int x2, int y1, int y2)
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	int blocker;

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

	return checkMapBoundaryR(x2) ? 1 : 0;
}

int CSpriteObject::checkSolidL( int x1, int x2, int y1, int y2)
{
	int blocker;
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	x1 -= COLISION_RES;

	// Check for right from the object
	if(solid)
	{
		for(int c=y1 ; c<=y2 ; c += COLISION_RES)
		{
			blocker = TileProperty[mp_Map->at(x1>>CSF, c>>CSF)].bright;
			const bool slope = (TileProperty[mp_Map->at(x1>>CSF, c>>CSF)].bup > 1);
			if(blocker && !slope)
				return blocker;
			else if(slope)
				return 0;
		}

		blocker = TileProperty[mp_Map->at(x1>>CSF, y2>>CSF)].bright;
		const bool slope = (TileProperty[mp_Map->at(x1>>CSF, y2>>CSF)].bup > 1);
		if(blocker && !slope)
			return blocker;
		else if(slope)
			return 0;
	}

	return checkMapBoundaryL(x1) ? 1 : 0;
}

int CSpriteObject::checkSolidU(int x1, int x2, int y1, const bool push_mode )
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	y1 -= COLISION_RES;

	if( (((y1+COLISION_RES)>>STC) != (((y1+COLISION_RES)>>CSF)<<TILE_S)) && !push_mode )
		return 0;

	// Check for right from the object
	if(solid)
	{
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			Sint8 blocked = TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bdown;

			if(blocked)
				return blocked;
		}
	}

	return checkMapBoundaryU(y1) ? 1 : 0;
}

int CSpriteObject::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	y2 += COLISION_RES;

	if( ( (y2>>STC) != ((y2>>CSF)<<TILE_S) ) && !push_mode )
		return 0;

	// Check for down from the object
	if(solid)
	{
		Sint8 blocked;
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blocked = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup;

			if( blocked && (blocked < 2 || blocked > 7) )
				return blocked;
		}

		blocked = TileProperty[mp_Map->at((x2-(1<<STC))>>CSF, y2>>CSF)].bup;
		if( blocked && (blocked < 2 || blocked > 7) )
			return blocked;
	}

	if( (Uint32)y2 > ((mp_Map->m_height)<<CSF) )
		exists=false; // Out of map?

	return 0;
}

/* Special slope function for galaxy
	0	Jump through		1	Flat bottom
	2	Bottom-> Middle		3	Middle -> top
	4	Bottom -> top		5	Middle -> bottom
	6	Top -> middle		7	Top -> bottom
 */
bool CSpriteObject::checkslopedU( int c, int y1, Sint8 blocked)
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
bool CSpriteObject::checkslopedD( int c, int y2, Sint8 blocked )
{
	int yb1, yb2;

	getSlopePointsLowerTile(blocked, yb1, yb2);

	int dy = ((yb2-yb1)*(c%512))/512;
	int yh = yb1 + dy - 32;

	return ( y2%512 > yh );
}

const int MOVE_RES = 1;

void CSpriteObject::processMoveBitLeft()
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
	adjustSlopedTiles(x1-(1<<STC), y1, y2, -MOVE_RES);
}

void CSpriteObject::processMoveBitRight()
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
	adjustSlopedTiles(x2+(1<<STC), y1, y2, MOVE_RES);
}

void CSpriteObject::processMoveBitUp()
{
	/// Now check the neighboring tile to the up
	const unsigned int x1 = getXPosition()+m_BBox.x1;
	const unsigned int x2 = getXPosition()+m_BBox.x2;
	const unsigned int y1 = getYPosition()+m_BBox.y1;

	if( (blockedu = checkSolidU(x1, x2, y1)) == true)
    {
        if(g_pBehaviorEngine->getEpisode()<=3) // Galaxy only!
            return;

        // additionally if there is a narrow space and the object might fit in, try to move it into that space
        const int xMid = (x1+x2)/2;
        if(checkSolidU(x1, xMid, y1) == false)
        {
            processMoveBitLeft();
        }
        else if(checkSolidU(xMid, x2, y1) == false)
        {
            processMoveBitRight();
        }

		return;
    }

    // If we are here, the tiles aren't blocking us.
    m_Pos.y -= MOVE_RES;
}

void CSpriteObject::processMoveBitDown()
{

    /// Now check the neighboring tile to the down
    const unsigned int x1 = getXPosition()+m_BBox.x1;
    const unsigned int x2 = getXPosition()+m_BBox.x2;
    const unsigned int y2 = getYPosition()+m_BBox.y2;

    if( ( blockedd = checkSolidD(x1, x2, y2) ) == true )
    {
        return;
    }

    // if we are here, the tiles aren't blocking us.
    // TODO: Here we need the Object collision part
    m_Pos.y+=MOVE_RES;
}


void CSpriteObject::processMove(const Vector2D<int>& dir)
{
	processMove(dir.x, dir.y);
}

void CSpriteObject::processMove(const int move_x, const int move_y)
{
    const float fxoff = static_cast<float>(move_x);
    const float fyoff = static_cast<float>(move_y);
    
    int xoff = static_cast<int>(fxoff);
    int yoff = static_cast<int>(fyoff);
    
    // Let's check if we have to move left or right
    if(xoff > 0)
    {
	// move right
	for(int c = 0 ; c<xoff ; c++ )
	    processMoveBitRight();
    }
    else if(xoff < 0)
    {
    // move left
	for(int c = 0 ; c<-xoff ; c++ )
	    processMoveBitLeft();
    }
    
    // Let's check if we have to move up or down
    if(yoff > 0)
    {
	// move down
	for(int c = 0 ; c<yoff ; c++ )
	    processMoveBitDown();
    }
    else if(yoff < 0)
    {
    // move up
	for(int c = 0 ; c<-yoff ; c++ )
	    processMoveBitUp();
    }
}

void CSpriteObject::processPushOutCollision()
{
	// If he isn't solid don't even care
	if(!solid)
		return;

	/// Check if from the lower part (floor) we are blocked
	// Check colision on floor
	if(blockedd)
	{
		const unsigned int x1 = getXPosition()+m_BBox.x1;
		const unsigned int x2 = getXPosition()+m_BBox.x2;
		const unsigned int y2 = getYPosition()+m_BBox.y2-1;

		if( checkSolidD(x1, x2, y2, true) /*&& checkSolidU(x1, x2, y2)*/ )
		{
			// Push him up to the position where he is not blocked anymore
			int should_y = y2;

			while( checkSolidD(x1, x2, should_y, true) )
				should_y--;

			processMove(0, should_y-y2);
		}
	}
	else if(blockedu)
	{
		const unsigned int x1 = getXPosition()+m_BBox.x1;
		const unsigned int x2 = getXPosition()+m_BBox.x2;
		const unsigned int y1 = getYPosition()+m_BBox.y1+1;

		if( checkSolidU(x1, x2, y1) /*&& checkSolidD(x1, x2, y1)*/ )
		{
			// Push him down
			int should_y = y1;

			while( checkSolidU(x1, x2, should_y) )
				should_y++;

			processMove(0, should_y-y1);
		}
	}

	if(blockedl)
	{
		const unsigned int x1 = getXPosition()+m_BBox.x1+1;
		const unsigned int x2 = getXPosition()+m_BBox.x2;
		const unsigned int y1 = getYPosition()+m_BBox.y1;
		const unsigned int y2 = getYPosition()+m_BBox.y2;

		if( checkSolidL(x1, x2, y1, y2) )
		{
			// Push him right to the position where he is not blocked anymore
			int should_x = x1;

			while( checkSolidL(should_x, x2, y1, y2) )
				should_x++;

			processMove(should_x-x1, 0);
		}
	}
	else if(blockedr)
	{
		const unsigned int x1 = getXPosition()+m_BBox.x1;
		const unsigned int x2 = getXPosition()+m_BBox.x2-1;
		const unsigned int y1 = getYPosition()+m_BBox.y1;
		const unsigned int y2 = getYPosition()+m_BBox.y2;

		if( checkSolidR(x1, x2, y1, y2) /*&& checkSolidL(x1, x2, y1, y2)*/ )
		{
			// Push him right to the position where he is not blocked anymore
			int should_x = x2;

			while( checkSolidR(x1, should_x, y1, y2) )
				should_x--;

			processMove(should_x-x2, 0);
		}
	}
}

void CSpriteObject::pumpEvent(const CEvent *evPtr)
{
    /*if( ObjMove* pObjMove = dynamic_cast<ObjMove>(evPtr))
    {
        processMove(pObjMove->m_Vec);
        //m_EventCont.pop_Event();
    }*/
}

void CSpriteObject::processEvents()
{
    if(mMoveTasks.empty())
        return;

    for( auto task : mMoveTasks)
    {
        ObjMove *objMove = task;

        if( ObjMoveCouple* pObjMove = dynamic_cast<ObjMoveCouple*>(objMove) )
        {
            auto move = pObjMove->m_Vec;
            processMove(move);
            pObjMove->mSecond.processMove(move);
        }
        else if( ObjMoveCouples* pObjMove = dynamic_cast<ObjMoveCouples*>(objMove) )
        {
            auto move = pObjMove->m_Vec;
            auto playerVec = pObjMove->mCarriedObjVec;

            processMove(move);

            for(auto &player : playerVec)
            {
                if(!player)
                    continue;

                if(!player->m_jumpdownfromobject)
                    player->processMove(move);
            }
        }
        else
        {
            processMove(objMove->m_Vec);
        }

        delete task;
    }

    mMoveTasks.clear();
}
