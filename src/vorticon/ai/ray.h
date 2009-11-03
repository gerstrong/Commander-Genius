
#define RAY_SPEED   		100
#define RAY_AUTO_SPEED		120
#define RAY_ZAPZOT_TIME    	10

#define RAY_FRAME_ZAP_EP1     110
#define RAY_FRAME_ZOT_EP1     111

#define RAY_FRAME_ZAP_EP2     124
#define RAY_FRAME_ZOT_EP2     125

#define RAY_FRAME_ZAP_EP3     105
#define RAY_FRAME_ZOT_EP3     106

enum ray_state{
	RAY_STATE_FLY,
	RAY_STATE_SETZAPZOT,
	RAY_STATE_ZAPZOT
};

// reference to ../game.cpp
void delete_object(int o);

// reference to ../misc.cpp
unsigned int rnd(void);
