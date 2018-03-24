/* FINALE.C
 Code for displaying the FINALE.CK? files.
 Thanks to Andrew Durdin for FIN2BMP, from which I got
 the decompression algorithm.
 */

#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include <base/utils/FindFile.h>

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
	const int numPixels = sfc->w*sfc->h;
	int pixeloffset = 0;
	Uint8* posPointer = pixel;
	
	do
	{
		pixeloffset = finale_y*(sfc->w) + finale_x;

		if(pixeloffset < numPixels)
		{
			posPointer = pixel+finale_y*sfc->w + finale_x;
			if (pix & mask)
			{
				if (finale_planecol==1)
					*posPointer = finale_planecol;
				else
				  // merge with previous planes
					*posPointer |= finale_planecol;
			}
			else if (finale_planecol==1)
			{
				*posPointer = 0;
			}
		}
		
		finale_x++;
		if (finale_x > sfc->w-1)
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
	
	if(SDL_MUSTLOCK(sfc)) SDL_UnlockSurface(sfc);
}

// draws a filename file into the SDL_Surface we are using
bool finale_draw( SDL_Surface *sfc, const std::string& filename, const std::string& path)
{
	int cmdbyte;
	int bytecount;
	int repeatbyte;
	int i;
	
 	std::ifstream file;
	if (!OpenGameFileR(file, getResourceFilename(filename, path, false, false), std::ios::binary))
		return false;
	

	// TODO: Here is big bad bug which makes the game crash...
	finale_plane_length = fgetl(file)*2;   //length of a plane when decompressed
	finale_planecol = 1;
	finale_x = finale_y = 0;
	finale_count = 0;
	finale_done = 0;
	
	// In case the surface is bigger than the compressed image, fill it with black first
    SDL_FillRect( sfc, nullptr, 0);

	// decompress/draw the image
	do
	{
		if ( file.eof() || (cmdbyte = file.get()) < 0 )
		{  // EOF (End of File)
			break;
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
			for( i=0 ; i<bytecount ; i++ )
			{
				finale_plot( sfc, repeatbyte );
			}
		}
		
	} while( !finale_done );
	
	file.close();

	return true;
}

