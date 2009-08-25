/*
 * palette.h
 *
 *  Created on: 23.08.2009
 *      Author: gerstrong
 */

#ifndef PALETTE_H_
#define PALETTE_H_

void fade(unsigned char type, unsigned char rate);
char fade_in_progress(void);

char pal_load(int episode);
void pal_fade(int fadeamt);

void pal_init(int dark);
void pal_fade(int fadeamt);


#endif /* PALETTE_H_ */
