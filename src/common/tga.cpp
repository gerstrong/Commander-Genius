/*
 * tga.cpp
 *
 *  Created on: 23.08.2009
 *      Author: gerstrong
 */

// Targa image loader, included from "vgatiles.c"
#include "tga.h"
#include "../keen.h"
#include "../fileio.h"
#include "../FindFile.h"
#include <stdio.h>
#include "../CLogFile.h"


char LoadTGA(const std::string &file, unsigned char **image, int *widthout, int *heightout)
// load a 32-bit uncompressed RGBA targa file, and return a pointer to
// the raw image data. The width and height of the image are returned as well.
{
	TGA_HEADER header;
	FILE *fp;
	int bytesperpixel;
	unsigned long imgdatasize;

	if (!(fp=OpenGameFile(file, "rb")))
	{
		return 1;
	}
	
	// read the header
	header.identsize = fgetc(fp);
	fgetc(fp);
	header.imagetype = fgetc(fp);
	fgeti(fp); fgeti(fp); fgetc(fp);
	
	header.xstart = fgeti(fp);
	header.ystart = fgeti(fp);
	header.width = fgeti(fp);
	header.height = fgeti(fp);
	header.bpp = fgetc(fp);
	fgetc(fp);
	
	if (header.imagetype != TGA_RGB)
	{
		g_pLogFile->textOut(PURPLE,"<br>LoadTGA: " + file + ": imagetype must be RGBA uncompressed!<br>");
		fclose(fp);
		return 1;
	}
	if (header.bpp != 32)
	{
		g_pLogFile->textOut(PURPLE,"<br>LoadTGA: " + file + ": image bpp must be 32 (RGB w/ alpha channel)<br>");
		fclose(fp);
		return 1;
	}
	
	*widthout = header.width;
	*heightout = header.height;
	
	bytesperpixel = (header.bpp / 8);
	imgdatasize = header.width * header.height * bytesperpixel;
	*image = new unsigned char [imgdatasize];
	if (!*image)
	{
		fclose(fp);
		return 1;
	}
	
	fread(*image, imgdatasize, 1, fp);
	fclose(fp);
	
	return 0;
}
