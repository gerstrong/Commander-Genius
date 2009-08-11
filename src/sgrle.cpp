/* SGRLE.C
  RLE compression and decompression functions for the SGRLE format...
  These are used for saved games (hence the name SG RLE). Could be used
  for other things too, I guess.

  After opening a file for decompression you must call sgrle_reset() before
  the first time you use sgrle_decompress().
*/
#include "keen.h"

void sgrle_reset(void);
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
void sgrle_decompress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
unsigned char sgrle_get_next_byte(FILE *fp);

#define SGRLE_RLEMARKER         255
#define SGRLE_MAXRLELEN         65530
int sgrle_runlen;
unsigned char sgrle_runchar;

/* decompresses the next byte from file FP. */
/* used internally by sgrle_decompress(). */
unsigned char sgrle_get_next_byte(FILE *fp)
{
  // are we currently in a RLE run?
  if (sgrle_runlen)
  {
    // decrease length of RLE run and return the previously
    // read char for the run
    sgrle_runlen--;
    return sgrle_runchar;
  }
  else
  { // not currently in a RLE run
    sgrle_runchar = fgetc(fp);
    if (sgrle_runchar==SGRLE_RLEMARKER)
    {  // start of a RLE run
      sgrle_runlen = fgeti(fp);
      sgrle_runchar = fgetc(fp);
      return sgrle_get_next_byte(fp);
    }
    else return sgrle_runchar;
  }
}

/* decompresses nbytes bytes of SGRLE-compressed data from */
/* file pointer *fp to the memory area pointed to by *ptr. */
void sgrle_decompress(FILE *fp, unsigned char *ptr, unsigned long nbytes)
{
  for(unsigned long i=0;i<nbytes;i++)
    ptr[i] = sgrle_get_next_byte(fp);
}

/* given a memory area *ptr of length nbytes, compresses the data */
/* using the SGRLE algorithm and saves it to file *fp */
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes)
{
	int byt;
	unsigned long compress_index, run_ahead_index;
	unsigned int runlength;
	int readbyt;
	unsigned int i;


  compress_index = 0;
  while(compress_index < nbytes)
  {
    // read a byte from the buffer
    readbyt = ptr[compress_index];

    /* is the next byte the same? if so find the length of the run */
    if (ptr[compress_index+1] == readbyt)
    {

      /* find how long readbyt runs */
      run_ahead_index = compress_index + 1;
      runlength = 1;
      do
      {
        byt = ptr[run_ahead_index];

        // we've run is over when either the byte is different
        // or run_ahead_index is at the end of the buffer,
        // or runlength is approaching FFFF (max possible RLE run length)
        if (byt != readbyt || run_ahead_index >= (nbytes-1) || runlength >= SGRLE_MAXRLELEN)
        {
          // if it's a forced stop because we hit the end of the buffer
          // we need to add 1 to runlength
          if (run_ahead_index >= (nbytes-1))
          {
            runlength++;
          }
          break;
        }
        run_ahead_index++;
        runlength++;
      } while(1);

       // it takes 4 bytes to code a RLE run, so if the run is less than
       // 4 bytes, it would actually be smaller if we didn't compress it
       if (runlength < 4 && readbyt!=SGRLE_RLEMARKER)
       {
         // RLE run, but too small to bother with
         for(i=0;i<runlength;i++) fputc(readbyt, fp);
       }
       else
       {
         // save a RLE run
         fputc(SGRLE_RLEMARKER, fp);
         fputc(runlength & 255, fp);
         fputc(runlength >> 8, fp);
         fputc(readbyt, fp);
       }
       // advance
       compress_index += runlength;
    }
    else
    {
      // next byte is different, this is not a run it's just a single char
      if (readbyt != SGRLE_RLEMARKER)
      {
        fputc(readbyt, fp);
      }
      else
      {
        // it's a single uncompressed byte which is equal to the RLE marker.
        // delimit it by placing it in a RLE run of length 1.
        runlength = 1;
        fputc(SGRLE_RLEMARKER, fp);
        fputc(runlength & 255, fp);
        fputc(runlength >> 8, fp);
        fputc(readbyt, fp);
      }
      compress_index++;
    }

  }

}

/* resets the decompression engine. (must call this before the first
   time you use sgrle_decompress(), each time you open a new file) */
void sgrle_reset(void)
{
  sgrle_runlen = 0;
}
