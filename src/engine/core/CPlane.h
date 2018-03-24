/*
 * CPlane.h
 *
 *  Created on: 10.06.2010
 *      Author: gerstrong
 */

#ifndef CPLANE_H_
#define CPLANE_H_

#include <base/TypeDefinitions.h>
#include <SDL.h>
#include <vector>

class CPlane
{
public:

    /**
     * @brief creates a plane of data.
     * @param width
     * @param height
     */
    void createDataMap(const int width, const int height);

    /**
     * @brief setMapDataAt  Set a tile at the plane
     * @param t the tile index to set
     * @param x coordinate x
     * @param y coordinate x
     */
    auto getMapDataPtr() -> word*
    {
        return mDataVec.data();
    }

    /**
     * @brief setMapDataAt Returns the set tile
     * @param x coordinate x
     * @param y coordinate x
     */
    auto getMapDataAt(const int x,
                      const int y) const -> word
    {
        if( x < mWidth && y < mHeight )
        {
            return mDataVec[size_t(y*mWidth+x)];
        }
        return 0;
    }


    /**
     * @brief setMapDataAt  Set a tile at the plane
     * @param t the tile index to set
     * @param x coordinate x
     * @param y coordinate x
     */
    auto setMapDataAt(const word t,
                      const int x,
                      const int y) -> void
    {
        mDataVec[size_t(y*mWidth+x)] = t;
    }

    /**
     * @brief getTimers Reference to timers defined for the plane.
     */
    auto getTimers() -> std::vector<int>&
    {
        return mTimers;
    }

    /**
     * @brief empty Is the plane empty
     * @return true if it is, otherwise not
     */
    auto empty() const -> bool
    {
        return mDataVec.empty();
    }


private:
    std::vector<word> mDataVec; // the map data of one plane

    // In animation cycles the time when something needs to be changed
    std::vector<int> mTimers;
    int mWidth = 0;
    int mHeight = 0;
};

#endif /* CPLANE_H_ */
