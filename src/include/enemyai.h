/*
 * enemyai.h
 *
 *  Created on: 17.01.2009
 *      Author: gerstrong
 */

// .ai functions
// ep1
void yorp_ai(int o, stLevelControl levelcontrol);
void garg_ai(int o, bool hardmode);
void vort_ai(int o, stLevelControl *p_levelcontrol);
void butler_ai(int o, bool hardmode);
void tank_ai(int o, bool hardmode);
void ray_ai(int o, int episode, bool automatic_raygun, char pShotSpeed);
void icechunk_ai(int o);
void icebit_ai(int o);
void icecannon_ai(int o);
void door_ai(int o, char DoorOpenDir);
void teleporter_ai(int o, stLevelControl levelcontrol);
void rope_ai(int o);

// ep2
void walker_ai(int o, stLevelControl levelcontrol);
void tankep2_ai(int o, bool hardmode);
void platform_ai(int o, stLevelControl levelcontrol);
void vortelite_ai(int o, bool darkness);
void se_ai(int o, stLevelControl *p_levelcontrol);
void explosion_ai(int o);
void earthchunk_ai(int o);
void spark_ai(int o, int *p_sparks_left);

// ep3
void foob_ai(int o, bool hardmode);
void ninja_ai(int o, bool hardmode);
void meep_ai(int o, stLevelControl levelcontrol);
void sndwave_ai(int o, bool hardmode);
void mother_ai(int o, stLevelControl levelcontrol);
void fireball_ai(int o, bool hard);
void ballandjack_ai(int o);
void platvert_ai(int o);
void nessie_ai(int o);

// Special
void autoray_ai(int o);
