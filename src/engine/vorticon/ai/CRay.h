
#define RAY_SPEED   		100
#define RAY_AUTO_SPEED		120
#define RAY_ZAPZOT_TIME    	10

enum ray_state{
	RAY_STATE_FLY,
	RAY_STATE_SETZAPZOT,
	RAY_STATE_ZAPZOT
};

// reference to ../misc.cpp
unsigned int rnd(void);
