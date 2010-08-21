/*
 * colourconvert.h
 *
 *  Created on: 18.03.2009
 *      Author: gerstrong
 */

#ifndef COLOURCONVERT_H_
#define COLOURCONVERT_H_

void fadetoColour(Uint32 &m_colour, Uint32 dest_colour, Uint8 speed);

void BlitSurfaceMerge( SDL_Surface *src, SDL_Rect *p_src_rect, SDL_Surface *dst, SDL_Rect *p_dst_rect );

#endif /* COLOURCONVERT_H_ */
