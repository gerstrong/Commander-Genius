
#ifndef	_CRAY_H_
#define _CRAY_H_

#define RAY_DEFAULTSPEED   		108
#define RAY_AUTO_SPEED		120
#define RAY_ZAPZOT_TIME    	10

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"
#include "engine/core/objenums.h"
#include <vector>

// reference to ../misc.cpp
unsigned int rnd(void);

class CRay : public CVorticonSpriteObject
{
public:
    CRay(CMap *p_map, Uint32 x, Uint32 y,
        direction_t hdir, direction_t vdir,
         const int spriteVar,
         object_t byType=OBJ_RAY,
         size_t byID=0,
         size_t speed=RAY_DEFAULTSPEED);

	virtual void process();
	void moveinAir();
	void setOwner(object_t type, unsigned int index);
	void setSpeed(size_t speed);
	void getTouchedBy(CVorticonSpriteObject &theObject);
	void getShotByRay(object_t &obj_type);
	bool isFlying();
	void setZapped();
	void gotZapped();

	direction_t m_HorDir;
	direction_t m_VertDir;
	int m_speed;

	enum {
		RAY_STATE_FLY,
		RAY_STATE_SETZAPZOT,
		RAY_STATE_ZAPZOT
	} state;

protected:
	bool m_automatic_raygun;
	char m_pShotSpeed;

	char zapzottimer;

	unsigned char dontHitEnable;
	unsigned int dontHit;         // index of an object type ray will not harm

	struct{
		object_t obj_type;
		unsigned int ID;
	}owner;

	// for earth chunks
	int baseframe;
};

#endif //_CRAY_H_
