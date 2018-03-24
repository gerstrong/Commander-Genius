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
