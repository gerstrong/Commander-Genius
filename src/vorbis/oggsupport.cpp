/*
 * oggsupport.c
 *
 *  Created on: 06.01.2009
 *      Author: gerstrong
 */
#ifdef BUILD_WITH_OGG
#include <SDL.h>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define MAX_LEVELS     100 // Stupid! But you need it!

#include "../include/declarations.h"
#include "../hqp/hq_sound.h"
#include "../sdl/CVideoDriver.h"
#include "../CLogFile.h"

short openOGGSound(FILE *fp, SDL_AudioSpec *pspec, stHQSound *psound)
{
	// it is an ogg file
	// If Ogg detected, decode it into the stream psound->sound_buffer. It must fit to Audio_cvt structure, so that
	// it can be converted

	int result;
    OggVorbis_File  oggStream;     // stream handle
    vorbis_info*    vorbisInfo;    // some formatting data
    vorbis_comment* vorbisComment; // user comments

    if((result = ov_open(fp, &oggStream, NULL, 0)) < 0)
    {
        fclose(fp);
        return 1;
    }
    else
    {
        vorbisInfo = ov_info(&oggStream, -1);
        vorbisComment = ov_comment(&oggStream, -1);

        pspec->format = 32784; // Not sure, if this works with any ogg file
        pspec->channels = vorbisInfo->channels;
        pspec->freq = vorbisInfo->rate;
    }

    long length;
    int  section;
    long pos=0;
    long ret;
    char *stream;

    length = 4 * (long)ov_pcm_total(&oggStream,-1);

    stream = (char*) malloc(length*sizeof(char));

    if(stream == NULL)
    {
    	g_pLogFile->textOut(RED,"Error! Out of memory! in Loading the Ogg file<br>");
    	return 1;
    }

    int eof=0;

    while(!eof){
       ret=ov_read(&oggStream,stream + pos,sizeof(stream),0,2,1,&section);
      if (ret == 0) {
        /* EOF */
        eof=1;
      } else if (ret < 0) {
        /* error in the stream.  Not a problem, just reporting it in
		 case we (the app) cares.  In this case, we don't. */
      } else {
        /* we don't bother dealing with sample rate changes, etc, but
		 you'll have to*/
    	  if(pos > length)
    	  {
    		// Something went wrong here! Stream is already full!
    		  g_pLogFile->ftextOut("WARNING: Stream overflow while reading the ogg-file!<br>");
    		  ret = 0;
    		  break;
    	  }
    	  pos += ret;
      }
    }

    psound->sound_buffer = (Uint8*) malloc(length);
    psound->sound_len = pos;

    memcpy( psound->sound_buffer, stream, psound->sound_len);

    free(stream);

    ov_clear(&oggStream); // This also closes fp

    return 0;
}
#endif
