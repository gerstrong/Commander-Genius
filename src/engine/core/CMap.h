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
#include <base/Vector2D.h>


#include "graphics/GsTilemap.h"
#include <base/TypeDefinitions.h>
#include "CPlane.h"
#include "scrollingplane.h"
#include <base/GsEvent.h>
#include <base/utils/Geometry.h>
#include <map>
#include <set>


class CMap
{

public:
	CMap();

	void setTileMap( GsTilemap &Tilemap );

    void setLevel( const Uint16 level ) {   m_Level = level;    }
    Uint16 getLevel() const {    return m_Level;    }

    void setLevelName( const std::string& name )
    {
        m_LevelName = name;
    }

    std::string getLevelName() const
    {
        return m_LevelName;
    }


    bool isEmpty() const;

    /**
     * @brief setupEmptyDataPlanes  Allocates data for the the planes to be loaded
     * @param numScrollingPlanes    Number of scrolling planes to setup for the whole map
     *                              One Infomap is always allocated
     * @param tileSize      Square size of a tile (Keen uses 16x16 by default)
     *                      (NOTE: Only 16 and 8 are supported)
     * @param width
     * @param height
     * @return  true if everything went allright, otherwise false
     */
    bool setupEmptyDataPlanes(const unsigned int numScrollingPlanes,
                              const int tileSize,
                              const Uint32 width,
                              const Uint32 height);

    bool gotoPos(const int x, const int y);


	void resetScrolls();
    void refreshStripes();


	// If force is enabled it will ignore scroll blockers
	bool scrollLeft(const bool force=false);
	bool scrollRight(const bool force=false);
	bool scrollUp(const bool force=false);
	bool scrollDown(const bool force=false);

    void calcVisibleArea();

    void setSubscrollUnits(const int plane, const int subscrollUnits);

    void insertHorBlocker(const int y);
    void insertVertBlocker(const int x);


    void resetScrollBlocker();

    void refreshVisibleArea();
    void refreshVisibleArea(const GsVec2D<int> scrollCoord);

    /**
     * @brief collectBlockersCoordiantes collects all the blocker coordinates in where the blockers are
     * @note it also clears already existing blocker coordinates
     */
    void collectBlockersCoordiantes();


	void redrawAt(const Uint32 mx, const Uint32 my);

    void drawAllOfPlane(const int planeIdx);
	void drawAll();

    void drawHstripeOfPlane(const int planeIdx,
                            const unsigned int y,
                            const unsigned int mpy);

    void drawHstripe(const unsigned int y, const unsigned int mpy );


    void drawVstripeOfPlane(const int planeIdx,
                            const unsigned int x,
                            const unsigned int mpx);

    void drawVstripe(const unsigned int x, const unsigned int mpx );

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

    Uint16 at(int x, int y, int t=1);
    Uint16 getObjectat(const Uint16 x, const Uint16 y);
	/*
	 * \brief
	 * This will check in horizontal direction if there is a scroll blocker.
	 * As far known, this is only used in the Galaxy engine
	 * \param y The y coordinate (CSFed) of the row where to look for that blocker
	 */
	bool findObject(unsigned int obj, int *xout, int *yout);
    bool findTile(const unsigned int tile, int &xout, int &yout, const int plane=1);

    bool setTile( const Uint16 x, const Uint16 y, const Uint16 t, const Uint16 plane=1);

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
	void animateAllTiles();

    auto getlevelat(const int x,
                    const int y) -> int;

    auto getPlaneDataAt(const int plane,
                        const int x,
                        const int y) const -> int;

    Uint16 getPlaneDataAt(const int plane, GsVec2D<Uint32> pos) const;

    word *getData(const Uint8 PlaneNum);
	word *getInfoData();
    Uint16 getInfoData(const GsVec2D<Uint32> pos) const;    
    bool setInfoTile(const GsVec2D<Uint32> pos,
                     const Uint16 tile);

	word *getForegroundData();
	word *getBackgroundData();


    void fetchNearestVertBlockers(const int x,
                                  int &leftCoord,
                                  int &rightCoord);

    void fetchNearestHorBlockers(const int y,
                                 int &upCoord,
                                 int &downCoord);


    void setupAnimationTimerOfTile(const int tilemapIdx);

    /**
     * @brief setupAnimationTimer   Set the animation timer to the coordinates instead of starting the first time with zero.
     *                              This fixes some tile animation issues seen in the Keen 9 especially
     */
    void setupAnimationTimer();

    /**
     *  Locks the map, the way, no one can switch to a new level. This is important when another player tries to enter
     *  a level, and the other hasn't even thrown the Flag (Galaxy)
     */
    void unlock();
    void lock();

    bool locked() const;


    void setSpriteOrigin(const int sprId, const GsVec2D<int> &origin);

    GsVec2D<int> getSpriteOrigin(const int sprId);

    GsVec2D<int> getScrollCoords(const unsigned int idx) const;

    GsVec2D<int> getMainScrollCoords() const;

    void lockAxisY(const unsigned int idx, const bool value);

    ScrollingPlane &getScrollingPlane(const int idx)
    {
        return mScrollingPlanes[idx];
    }

    auto getNumScrollingPlanes() const
    {
        return mScrollingPlanes.size();
    }


    Uint32 m_width = 0, m_height = 0;            // size of the map (in tile units)
    bool m_worldmap = false;             // if 1, this is the world map

	std::string m_gamepath;

    bool m_animation_enabled = true;
    bool m_Dark = false;
	bool isSecret;
    int mNumFuses = 0;
    bool mFuseInLevel = false;

    GsVec2D<int> mGamePlayPos;    



private:

    bool detectTopBlocker(const int y);
    bool detectBottomBlocker(const int y);
    bool detectLeftBlocker(const int x);
    bool detectRightBlocker(const int x);


	std::vector<GsTilemap> &m_Tilemaps;

    float mAnimtileTimer = 0.0f;

    CPlane mInfoPlane;
    std::vector<ScrollingPlane> mScrollingPlanes;

	Uint16 m_Level;
	std::string m_LevelName;

    bool mLocked = false;

    GsRect<int> mVisArea;

    std::map< int, GsVec2D<int> > mSpriteOriginList;

    int mShakeCounter = 0;
    int mMaxShakeCounter = 0;
    int mMaxShakeVAmt = 0;
    int mShakeDir = 0;
};



// CMap related Events
/**
 * @description	you might use this event in case you want to release the scrollbuffer and give another instance
 * 				chance to use it.
 * 				You should invoke this, when an instance with a CMap Object owned the Scrollbuffers
 */
struct ResetScrollSurface : CEvent
{};


#endif /* CMAP_H_ */
