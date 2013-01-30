/*
 * tga.cpp
 *
 *  Created on: 23.08.2009
 *      Author: gerstrong
 */

#include <stdio.h>
#include "tga.h"
#include "fileio.h"
#include "FindFile.h"
#include "CLogFile.h"


/** 
 *  \brief loads a 32-bit uncompressed RGBA targa file, and return a pointer to
 *         the raw image data. The width and height of the image are returned as well.
 *  \param filename Name to the file that will used to read the picture
 *  \param image    Pointer to an array that will contain the image data. This array will be allocated
 *  \param width    Width of the picture that will be set after the picture was read
 *  \param height   Same here for the height
 */
bool LoadTGA(const std::string& filename, Uint8 **image, Uint16 &width, Uint16 &height)
{
	TGA_HEADER header;
	FILE *fp;
	uint bytes_per_pixel;
	unsigned long img_data_size;

	// First check if the files exists
	if ((fp=OpenGameFile(filename, "rb")) == NULL)
		return false;
	
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
	
	// Check if the header meet our requirements
	if (header.imagetype != TGA_RGB)
	{
		g_pLogFile->textOut(PURPLE,"<br>LoadTGA: " + filename + ": imagetype must be RGBA uncompressed!<br>");
		fclose(fp);
		return false;
	}

	if (header.bpp != 32)
	{
		g_pLogFile->textOut(PURPLE,"<br>LoadTGA: " + filename + ": image bpp must be 32 (RGB w/ alpha channel)<br>");
		fclose(fp);
		return false;
	}
	
	// Set some variables values so the pictures can be used outside this code
	width = header.width;
	height = header.height;
	
	bytes_per_pixel = (header.bpp / 8);
	img_data_size = header.width * header.height * bytes_per_pixel;

	// allocate memory for the image buffer
	*image = new byte [img_data_size];
	if (*image == NULL)
	{
		fclose(fp);
		return false;
	}
	
	// Read the picture data
	const uint elem_read = fread(*image, img_data_size, 1, fp);
	fclose(fp);

	// Check if the picture was read correctly
	if(elem_read != 1)
	{
		delete [] *image;
		*image  = NULL;
		return false;
	}
	else
		return true;
}
