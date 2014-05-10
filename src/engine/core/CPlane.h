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

class CPlane
{
public:
	CPlane();

	void createDataMap(Uint16 width, Uint16 height);
	word *getMapDataPtr();
	word getMapDataAt(Uint16 x, Uint16 y);
	void setMapDataAt(Uint16 t, Uint16 x, Uint16 y);
	bool empty();

	virtual ~CPlane();

private:
	word *mp_data;       		// the map foreground data
	Uint16 m_width;
	Uint16 m_height;
};

#endif /* CPLANE_H_ */
