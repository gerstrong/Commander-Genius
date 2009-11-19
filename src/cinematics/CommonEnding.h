/*
 * commonending.h
 *
 *  Created on: 07.08.2009
 *      Author: gerstrong
 */

#ifndef COMMONENDING_H_
#define COMMONENDING_H_

#include <SDL.h>

// Common Ending functions
void eseq_showmsg(const std::string& text, int boxleft, int boxtop,
				  int boxwidth, int boxheight, bool autodismiss, SDL_Surface *spritesurface = NULL);

// For Episode 1
int eseq1_ShipFlys(stCloneKeenPlus *pCKP, bool flyback);
int eseq1_ReturnsToShip(stCloneKeenPlus *pCKP);
int eseq1_BackAtHome(stCloneKeenPlus *pCKP);
void eseq1_showEndingText(std::string &Path);

// For Episode 2
int eseq2_HeadsForEarth(stCloneKeenPlus *pCKP);
int eseq2_LimpsHome(stCloneKeenPlus *pCKP);
int eseq2_SnowedOutside(stCloneKeenPlus *pCKP);

// For Episode 3
char eseq3_AwardBigV(stCloneKeenPlus *pCKP);

// common functions
void eseq_ToBeContinued( SDL_Surface *extrascreen = NULL );

// Reference to ../gamedo.cpp
void gamedo_render_drawobjects();
void gamedo_frameskipping_blitonly();

#endif /* COMMONENDING_H_ */
