/*
 * icecannon.h
 *
 *  Created on: 06.08.2009
 *      Author: gerstrong
 */

#ifndef ICECANNON_H_
#define ICECANNON_H_

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"
#include <vector>

unsigned int rnd(void);

class CIceCannon : public CVorticonSpriteObject
{
public:
	CIceCannon(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y,
			int vector_x, int vector_y );
	bool isNearby(CSpriteObject &theObject);
	void process();

private:
	int vector_x, vector_y;
	int mTimer;
	bool silent;
};

class CIceChunk : public CVorticonSpriteObject
{
public:
	CIceChunk(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y, Uint32 vx, Uint32 vy);
	void getTouchedBy(CSpriteObject &theObject);
	void process();
	bool isNearby(CSpriteObject &theObject);
	void smash();
private:
	int vector_x, vector_y;
	int veloc_x, veloc_y;
	bool silent;
};

class CIceBit : public CVorticonSpriteObject
{
public:
	CIceBit(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y, Uint32 vec_x, Uint32 vec_y);
	void process();

	int vector_x, vector_y;
	int veloc_x, veloc_y;
	int timer;
};

#endif /* ICECANNON_H_ */
