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
        return mp_data;
    }

    word getMapDataAt(Uint16 x, Uint16 y)
    {
        if( x < m_width && y < m_height )
            return mp_data[y*m_width+x];
        return 0;
    }


    void setMapDataAt(Uint16 t, Uint16 x, Uint16 y)
    {
        mp_data[y*m_width+x] = t;
    }

    std::vector<Uint8> &getTimers()
    {
        return mTimers;
    }

	bool empty();

	virtual ~CPlane();

private:
	word *mp_data;       		// the map foreground data

    // In animation cycles the time when something needs to be changed
    std::vector<Uint8> mTimers;
	Uint16 m_width;
	Uint16 m_height;
};

#endif /* CPLANE_H_ */
