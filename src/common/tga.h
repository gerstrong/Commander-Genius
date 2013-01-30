/*
 * tga.h
 *
 *  Created on: 23.08.2009
 *      Author: gerstrong
 *
 *  Header the read and open code of TGA picture files. Only some icons and the rising points use TGA so far.
 *  There are much more efficient libs outside which do that job like SDL_Image. We might use those in future...
 */

#ifndef TGA_H_
#define TGA_H_

#include <SDL.h>
#include <string>

#ifdef TARGET_WIN32
#define ulong unsigned long
#define uint unsigned int
#endif

#ifdef __APPLE__
#define ulong unsigned long
#define uint unsigned int
#endif

#define uchar	unsigned char

#define TGA_NONE		0
#define TGA_INDEXED		1
#define TGA_RGB			2
#define TGA_GREYSCALE	3

typedef struct
{
    uchar identsize;           // size of ID field that follows 18 uchar header (usually 0)
    uchar colourmaptype;       // type of colour map 0=none, 1=has palette
    uchar imagetype;           // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    uint colourmapstart;       // first colour map entry in palette
    uint colourmaplength;      // number of colours in palette
    uchar colourmapbits;       // number of bits per palette entry 15,16,24,32

    uint xstart;               // image x origin
    uint ystart;               // image y origin
    uint width;                // image width in pixels
    uint height;               // image height in pixels
    uchar bpp;	               // image bits per pixel 8,16,24,32
    uchar descriptor;          // image descriptor bits (vh flip bits)

    // pixel data follows header
} TGA_HEADER;

#undef uchar

bool LoadTGA(const std::string& filename, Uint8 **image, Uint16 &width, Uint16 &height);

#endif /* TGA_H_ */
