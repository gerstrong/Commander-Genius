
#define RAY_SPEED   		100
#define RAY_AUTO_SPEED		120
#define RAY_ZAPZOT_TIME    	10

#include "../../../CObject.h"
#include "../../../CPlayer.h"
#include <vector>

// reference to ../misc.cpp
unsigned int rnd(void);

class CRay : public CObject
{
public:
	CRay(std::vector<CPlayer> &PlayerVect, CMap *p_map,
			bool automatic_raygun, char pShotSpeed, direction_t dir);

	void process();

private:
	std::vector<CPlayer> &m_PlayerVect;
	CMap *mp_map;
	bool m_automatic_raygun;
	char m_pShotSpeed;

	enum{
		RAY_STATE_FLY,
		RAY_STATE_SETZAPZOT,
		RAY_STATE_ZAPZOT
	}state;
	char zapzottimer;

	unsigned char dontHitEnable;
	unsigned int dontHit;         // index of an object type ray will not harm
	unsigned int owner;			  // Object number, from which the ray has been shot.
	bool shotbyplayer;

	// for soundwave
	int animframe, animtimer;
	int offscreentime;

	// for earth chunks
	int baseframe;

	direction_t m_Direction;
};

