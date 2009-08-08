/*
 * commonending.h
 *
 *  Created on: 07.08.2009
 *      Author: gerstrong
 */

#ifndef COMMONENDING_H_
#define COMMONENDING_H_

// For Episode 1
int eseq1_ShipFlys(stCloneKeenPlus *pCKP);
int eseq1_ReturnsToShip(stCloneKeenPlus *pCKP);
int eseq1_BackAtHome(stCloneKeenPlus *pCKP);

// For Episode 2
int eseq2_HeadsForEarth(stCloneKeenPlus *pCKP);
int eseq2_LimpsHome(stCloneKeenPlus *pCKP);
int eseq2_SnowedOutside(stCloneKeenPlus *pCKP);

// For Episode 3
char eseq3_AwardBigV(stCloneKeenPlus *pCKP);

// Reference to ../gamedo.cpp
void gamedo_render_drawobjects();
void gamedo_frameskipping_blitonly();

#endif /* COMMONENDING_H_ */
