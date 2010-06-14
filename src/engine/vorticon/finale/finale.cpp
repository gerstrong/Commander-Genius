/* FINALE.C
 Code for displaying the FINALE.CK? files.
 Thanks to Andrew Durdin for FIN2BMP, from which I got
 the decompression algorithm.
 */

#include "../../../fileio.h"
#include "../../../fileio/ResourceMgmt.h"
#include "../../../FindFile.h"

#include <fstream>

int finale_x;
int finale_y;
int finale_count;
int finale_planecol;
int finale_plane_length;
int finale_done;

// used internally by finale_draw()
void finale_plot( SDL_Surface *sfc, int pix )
{
	int mask;
	
	//TODO: This is a temporary solution. Might be changed later.
	mask = 128;
	
	if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
	Uint8* pixel = (Uint8*) sfc->pixels;
	
	do
	{
		if (pix & mask)
		{
			if (finale_planecol==1)
			{
				//g_pGraphics->sb_setpixel(finale_x, finale_y, finale_planecol);
				if( (finale_y*320 + finale_x) < 320*200 )
					pixel[finale_y*320 + finale_x] = finale_planecol;
			}
			else
			{  // merge with previous planes
				if( finale_y*finale_x < 320*200 )
					pixel[finale_y*320 + finale_x] |= finale_planecol;
			}
		}
		else if (finale_planecol==1)
		{
			//g_pGraphics->sb_setpixel(finale_x, finale_y, 0);
			if( (finale_y*320 + finale_x) < 320*200 )
				pixel[finale_y*320 + finale_x] = 0;
		}
		
		finale_x++;
		if (finale_x > 319)
		{
			finale_x = 0;
			finale_y++;
		}
		
		finale_count++;
		if (finale_count >= finale_plane_length)
		{
			finale_x = finale_y = 0;
			finale_count = 0;
			finale_planecol <<= 1;
			if (finale_planecol > 8) finale_done = 1;
		}
		
		if (mask==1)
		{
			return;
		}
		else
		{
			mask >>= 1;
		}
		
	} while(1);
	
	if(SDL_MUSTLOCK(sfc)) SDL_LockSurface(sfc);
}

// draws a filename file into the SDL_Surface we are using
void finale_draw( SDL_Surface *sfc, const std::string& filename, const std::string& path)
{
	int cmdbyte;
	int bytecount;
	int repeatbyte;
	int i;
	
	std::ifstream file;
	if (!OpenGameFileR(file, getResourceFilename(filename, path, true, false), std::ios::binary))
		return;
	
	finale_plane_length = fgetl(file)*2;   //length of a plane when decompressed
	finale_planecol = 1;
	finale_x = finale_y = 0;
	finale_count = 0;
	finale_done = 0;
	
	// decompress/draw the image
	do
	{
		cmdbyte = file.get();
		if (cmdbyte<0)
		{  // EOF
			return;
		}
		
		if (cmdbyte & 0x80)
		{
			//N + 1 bytes of data follows
			bytecount = (cmdbyte & 0x7F) + 1;
			for(i=0;i<bytecount;i++)
			{
				finale_plot( sfc, file.get() );
			}
		}
		else
		{
			//Repeat N + 3 of following byte
			bytecount = (cmdbyte + 3);
			repeatbyte = file.get();
			for(i=0;i<bytecount;i++)
			{
				finale_plot( sfc, repeatbyte );
			}
		}
		
	} while(!finale_done);
	
	file.close();
}

