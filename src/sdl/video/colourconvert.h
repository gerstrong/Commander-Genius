/*
 * colourconvert.h
 *
 *  Created on: 18.03.2009
 *      Author: gerstrong
 */

#ifndef COLOURCONVERT_H_
#define COLOURCONVERT_H_

Uint32 convert4to32BPPcolor(Uint8 c, SDL_Surface *BlitSurface);
Uint16 convert4to16BPPcolor(Uint8 c, SDL_Surface *BlitSurface);

#endif /* COLOURCONVERT_H_ */
