/* FINALE.C
  Code for displaying the FINALE.CK? files.
  Thanks to Andrew Durdin for FIN2BMP, from which I got
  the decompression algorithm.
*/

#include "keen.h"
#include "include/fileio.h"
#include "CGraphics.h"
#include "FindFile.h"

int finale_x;
int finale_y;
int finale_count;
int finale_planecol;
int finale_plane_length;
int finale_done;

// used internally by finale_draw()
void finale_plot(int pix)
{
int mask;

   mask = 128;
   do
   {
     if (pix & mask)
     {
       if (finale_planecol==1)
       {
    	   g_pGraphics->sb_setpixel(finale_x, finale_y, finale_planecol);
       }
       else
       {  // merge with previous planes
    	   g_pGraphics->sb_setpixel(finale_x, finale_y, g_pGraphics->sb_getpixel(finale_x, finale_y) | finale_planecol);
       }
     }
     else if (finale_planecol==1)
     {
    	 g_pGraphics->sb_setpixel(finale_x, finale_y, 0);
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

}

// draws a finale.ck? file into the upper-left corner of the scrollbuffer
void finale_draw(const std::string& filename, const std::string& path)
{
FILE *fp;
int cmdbyte;
int bytecount;
int repeatbyte;
int i;


	std::string buffer = formatPathString(path);


   map_unregister_all_animtiles();

	std::string fname = buffer + filename;
   fp = OpenGameFile(fname.c_str(), "rb");
   if (!fp)
   {
     crashflag = 1;
     why_term_ptr = "finale_draw(): cannot open finake.ck? file.";
     return;
   }

   finale_plane_length = fgetl(fp)*2;   //length of a plane when decompressed
   finale_planecol = 1;
   finale_x = finale_y = 0;
   finale_count = 0;
   finale_done = 0;

   /* decompress/draw the image */
   do
   {
     cmdbyte = fgetc(fp);
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
          finale_plot(fgetc(fp));
        }
     }
     else
     {
        //Repeat N + 3 of following byte
        bytecount = (cmdbyte + 3);
        repeatbyte = fgetc(fp);
        for(i=0;i<bytecount;i++)
        {
          finale_plot(repeatbyte);
        }
     }

   } while(!finale_done);

   fclose(fp);
}
