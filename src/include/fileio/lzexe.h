/*
 * lzexe.h
 *
 *  Created on: 24.01.2009
 *      Author: gerstrong
 */

#include <stdio.h>

int get_bit(int *p_bit_count, FILE **fin); /* only used for the conversion of sounds from episode 2 and 3 by sound_extraction_of_exe_files */

int unlzexe(FILE *fin, unsigned char *outbuffer);

int getEXEVersion(int episode, int bufsize);
