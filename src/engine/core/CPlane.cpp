/*
 * CPlane.cpp
 *
 *  Created on: 10.06.2010
 *      Author: gerstrong
 */

#include "CPlane.h"

#include <cassert>

void CPlane::createDataMap(const int width, const int height)
{
    assert(width > 0);
    assert(height > 0);

    mWidth = width;
    mHeight = height;

    mDataVec.assign(size_t(mWidth*mHeight), 0);
    mTimers.assign(size_t(mWidth*mHeight), 0);
}


bool CPlane::findTile(const unsigned int tile,
                      int &xout,
                      int &yout)
{
    for(int y=2;y<mHeight-2;y++)
    {
        for(int x=2;x<mWidth-2;x++)
        {
            if (getMapDataAt(x,y) == tile)
            {
                xout = x;
                yout = y;
                return true;
            }
        }
    }
    return false;
}
