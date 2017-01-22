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
	CPlane();

	void createDataMap(Uint16 width, Uint16 height);

    word *getMapDataPtr()
    {
        return mDataVec.data();
    }

    word getMapDataAt(Uint16 x, Uint16 y) const
    {
        if( x < m_width && y < m_height )
        {
            return mDataVec[y*m_width+x];
        }
        return 0;
    }


    void setMapDataAt(Uint16 t, Uint16 x, Uint16 y)
    {
        mDataVec[y*m_width+x] = t;
    }

    std::vector<Uint8> &getTimers()
    {
        return mTimers;
    }

    bool empty() const
    {
        return mDataVec.empty();
    }


private:
    std::vector<word> mDataVec; // the map data of one plane

    // In animation cycles the time when something needs to be changed
    std::vector<Uint8> mTimers;
	Uint16 m_width;
	Uint16 m_height;
};

#endif /* CPLANE_H_ */
