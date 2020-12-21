#ifndef SCROLLINGPLANE_H
#define SCROLLINGPLANE_H


#include "graphics/GsTilemap.h"

#include "CPlane.h"
#include <set>

// animation rate of animated tiles
#define ANIM_TILE_TIME      256

#define CSF    9
#define TILE_S			4
#define STC (CSF-TILE_S)

/**
 * @brief The ScrollingPlane class represents for every plane everything
 *        on the screen which is part of a level and scrolls
 *        The games depending use multiple scrolling planes
 *        for handling foreground, backgrounds and in special cases
 *        backdrop (whatever is behind background)
 */

class ScrollingPlane : public CPlane
{
public:
    ScrollingPlane();

    /**
     * @brief creates a plane of data.
     * @param width
     * @param height
     */
    void createDataMap(const int width,
                       const int height,
                       const int tileSize);

    GsVec2D<int> getScrollCoords() const;

    int getTilemapIdx() const;

    void resetScrolling();

    void drawTile(GsTilemap &tilemap,
                  const GsVec2D<Uint16> pos,
                  const Uint16 t);

    bool scrollLeft(GsTilemap &tilemap);
    bool scrollRight(GsTilemap &tilemap);
    bool scrollUp(GsTilemap &tilemap, const bool force);
    bool scrollDown(GsTilemap &tilemap, const bool force);

    void drawHstripe(GsTilemap &tilemap,
                     const unsigned int y,
                     const unsigned int mpy);


    void drawVstripe(GsTilemap &tilemap,
                     const unsigned int x,
                     const unsigned int mpx);

    void drawAll(GsTilemap &tilemap);

    bool gotoPos(GsTilemap &tilemap, GsVec2D<int> pos);

    void refreshStripes(GsTilemap &tilemap);

    /**
     * @brief collectBlockersCoordiantes collects all the blocker coordinates in where the blockers are
     * @note it also clears already existing blocker coordinates
     */
    void collectBlockersCoordiantes();

    /**
     * @brief insertVertBlocker  Insert a horizontal blocker coordinate
     * @param x CSFed coordinate that would scroll-block
     */
    void insertVertBlocker(const int x);
    void fetchNearestVertBlockers(const int x, int &leftCoord, int &rightCoord);

    /**
     * @brief insertHorBlocker  Insert a horizontal blocker coordinate
     * @param y CSFed coordinate that would scroll-block
     */
    void insertHorBlocker(const int y);
    void fetchNearestHorBlockers(const int y, int &upCoord, int &downCoord);


    void redrawPlaneAt(GsTilemap &tilemap,
                       const Uint32 mx,
                       const Uint32 my);

private:

    GsVec2D<int> mScrollCoords; // Amount of how much is scrolled on the map relative to (0,0) and per plane index

    int mTileSizeBase = 4; // Keen games have 16x16 tile size which is the base of

    int mScrollSfcIdx = -1;

    Uint8 m_scrollpix;     	// (0-7) for tracking when to draw a stripe
    Uint16 m_mapx;           	// map X location shown at scrollbuffer row 0
    Uint16 m_mapxstripepos;  	// X pixel position of next stripe row

    Uint8 m_scrollpixy;    	// (0-7) for tracking when to draw a stripe
    Uint16 m_mapy;         	// map Y location shown at scrollbuffer column 0
    Uint16 m_mapystripepos;  	// Y pixel position of next stripe column

    std::set<int> scrollBlockX;
    std::set<int> scrollBlockY;
};

#endif // SCROLLINGPLANE_H
