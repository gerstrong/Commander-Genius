/*
 * CMap.h
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#ifndef CMAP_H_
#define CMAP_H_

#include <SDL.h>
#include <string>
#include <vector>
#include <list>
#include <base/GsTimer.h>


#include "graphics/GsTilemap.h"
#include <base/TypeDefinitions.h>
#include "CPlane.h"
#include <base/GsEvent.h>
#include <base/utils/Geometry.h>
#include <map>

// animation rate of animated tiles
#define ANIM_TILE_TIME      256

#define CSF    9
#define TILE_S			4
#define STC (CSF-TILE_S)


class CMap
{

public:
	CMap();

	void setTileMap( GsTilemap &Tilemap );

    void setLevel( const Uint16 level );
	Uint16 getLevel();

	void setLevelName( const std::string& name );
	std::string getLevelName();


    /**
     * @brief setupEmptyDataPlanes  Allocates data for the the planes to be loaded
     * @param numPlanes     Number of planes to setup for the whole map
     * @param width
     * @param height
     * @return  true if everything went allright, otherwise false
     */
    bool setupEmptyDataPlanes(int numPlanes, Uint32 width, Uint32 height);

	bool gotoPos( int x, int y );


	void resetScrolls();
    void refreshStripes();


	// If force is enabled it will ignore scroll blockers
	bool scrollLeft(const bool force=false);
	bool scrollRight(const bool force=false);
	bool scrollUp(const bool force=false);
	bool scrollDown(const bool force=false);

    void calcVisibleArea();
    void refreshVisibleArea();
	void redrawAt(const Uint32 mx, const Uint32 my);
	void drawAll();
	void drawHstripe( unsigned int y, unsigned int mpy );
	void drawVstripe( unsigned int x, unsigned int mpx );

    /**
     * @brief shakMap will make the currently running map shake for a certain amount of time
     * @param cycles
     * @param vAmount
     */
    void triggerShake(const unsigned int cycles, const unsigned int vAmount );

    /**
     * @brief renderShaking internal shaking routine for blooglet or when in EP3 Mortimer Machine is getting destroyed
     */
    void renderShaking();
	void _drawForegroundTiles();

	Uint16 at(Uint16 x, Uint16 y, Uint16 t=1);
	Uint16 getObjectat(Uint16 x, Uint16 y);
	/*
	 * \brief
	 * This will check in horizontal direction if there is a scroll blocker.
	 * As far known, this is only used in the Galaxy engine
	 * \param y The y coordinate (CSFed) of the row where to look for that blocker
	 */
	bool findObject(unsigned int obj, int *xout, int *yout);
	bool findTile(unsigned int tile, int *xout, int *yout, int plane=1);

	bool setTile(Uint16 x, Uint16 y, Uint16 t, Uint16 plane=1);

    /**
     * @brief setTile   Sets the tile index t
     * @param x         position x
     * @param y         position y
     * @param t         tile index
     * @param redraw    redraw the tile. This is required if you want to see the results immediately.  (For example bridges)
     * @param plane     Plane on which the tile is set.
     * @return
     */
    bool setTile(Uint16 x, Uint16 y, Uint16 t, bool redraw, Uint16 plane=1);
	bool changeTile(Uint16 x, Uint16 y, Uint16 t);
	void changeTileArrayY(Uint16 x, Uint16 y, Uint16 h, Uint16 tile);

	// Animation methods
	Uint8 getAnimtiletimer();
	void drawAnimatedTile(SDL_Surface *dst, Uint16 mx, Uint16 my, Uint16 tile);
	void animateAllTiles();

	unsigned int getlevelat(unsigned int x, unsigned int y)
	{
		return m_Plane[2].getMapDataAt(x>>TILE_S,y>>TILE_S);
	}

	Uint16 getPlaneDataAt(int plane, unsigned int x, unsigned int y)
	{
		return m_Plane[plane].getMapDataAt(x>>CSF, y>>CSF);
	}

    Uint16 getPlaneDataAt(int plane, Vector2D<Uint32> pos)
	{
		return m_Plane[plane].getMapDataAt(pos.x>>CSF, pos.y>>CSF);
	}

	word *getData(Uint8 PlaneNum);
	word *getInfoData();
	word *getForegroundData();
	word *getBackgroundData();


    /**
     * @brief setupAnimationTimer   Set the animation timer to the coordinates instead of starting the first time with zero.
     *                              This fixes some tile animation issues seen in the Keen 9 especially
     */
    void setupAnimationTimer();

	/**
     * This method collects the coordinates in where the blockers are, so the checking routine obtains them at a faster way
	 */
	void collectBlockersCoordiantes();

	void fetchNearestVertBlockers(const int x, int &leftCoord, int &rightCoord);
	void fetchNearestHorBlockers(const int y, int &upCoord, int &downCoord);

    /**
     *  Locks the map, the way, no one can switch to a new level. This is important when another player tries to enter
     *  a level, and the other hasn't even thrown the Flag (Galaxy)
     */
    void unlock();
    void lock();

    bool locked()
    { return mLocked; }


    void setSpriteOrigin(const int sprId, const Vector2D<int> &origin)
    {
        mSpriteOriginList[sprId] = origin;
    }

    Vector2D<int> getSpriteOrigin(const int sprId)
    {
        return mSpriteOriginList[sprId];
    }


	Uint16 m_scrollx;      		// Amount of how much is scrolled on the map relative to (0,0) in X
	Uint16 m_scrolly;    		// Amount of how much is scrolled on the map relative to (0,0) in Y

    Uint32 m_width, m_height;            // size of the map (in tile units)
	bool m_worldmap;             // if 1, this is the world map

	std::string m_gamepath;

	bool m_animation_enabled;
	bool m_Dark;
	bool isSecret;
	int mNumFuses;
    bool mFuseInLevel;


    Vector2D<int> mGamePlayPos;

private:

    bool findVerticalScrollBlocker(const int x);
    bool findHorizontalScrollBlocker(const int y);



	Uint8 m_scrollpix;     	// (0-7) for tracking when to draw a stripe
	Uint16 m_mapx;           	// map X location shown at scrollbuffer row 0
	Uint16 m_mapxstripepos;  	// X pixel position of next stripe row

	Uint8 m_scrollpixy;    	// (0-7) for tracking when to draw a stripe
	Uint16 m_mapy;         	// map Y location shown at scrollbuffer column 0
	Uint16 m_mapystripepos;  	// Y pixel position of next stripe column

	std::vector<GsTilemap> &m_Tilemaps;

	float mAnimtileTimer;

	CPlane m_Plane[3];
	Uint16 m_Level;
	std::string m_LevelName;
	std::vector<int> scrollBlockX;
	std::vector<int> scrollBlockY;

    bool mLocked;

    GsRect<int> mVisArea;

    std::map< int, Vector2D<int> > mSpriteOriginList;

    int mShakeCounter;
    int mMaxShakeCounter;
    int mMaxShakeVAmt;
    int mShakeDir;
};



// CMap related Events
/**
 * \description	you might use this event in case you want to release the scrollbuffer and give another instance
 * 				chance to use it.
 * 				You should invoke this, when an instance with a CMap Object owned the Scrollbuffers
 */
struct ResetScrollSurface : CEvent
{};


#endif /* CMAP_H_ */
