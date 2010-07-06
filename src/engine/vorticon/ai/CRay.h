
#define RAY_SPEED   		100
#define RAY_AUTO_SPEED		120
#define RAY_ZAPZOT_TIME    	10

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"
#include "../../../common/objenums.h"
#include <vector>

// reference to ../misc.cpp
unsigned int rnd(void);

class CRay : public CObject
{
public:
	CRay(CMap *p_map, Uint32 x, Uint32 y,
		direction_t dir, object_t byType, size_t byID);

	virtual void process();
	void setOwner(object_t type, unsigned int index);
	void getTouchedBy(CObject &theObject);
	bool isFlying();

	direction_t m_Direction;

protected:
	bool m_automatic_raygun;
	char m_pShotSpeed;

	enum {
		RAY_STATE_FLY,
		RAY_STATE_SETZAPZOT,
		RAY_STATE_ZAPZOT
	} state;
	char zapzottimer;

	unsigned char dontHitEnable;
	unsigned int dontHit;         // index of an object type ray will not harm

	struct{
		object_t obj_type;
		unsigned int ID;
	}owner;

	// for soundwave
	int animframe, animtimer;
	int offscreentime;

	// for earth chunks
	int baseframe;
};

