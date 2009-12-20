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


// For Episode 2
int eseq2_HeadsForEarth();
int eseq2_LimpsHome();
int eseq2_SnowedOutside();

// For Episode 3
char eseq3_AwardBigV();

// common functions
void eseq_ToBeContinued( SDL_Surface *extrascreen = NULL );

// Reference to ../gamedo.cpp
void gamedo_render_drawobjects();
void gamedo_frameskipping_blitonly();

#endif /* COMMONENDING_H_ */
