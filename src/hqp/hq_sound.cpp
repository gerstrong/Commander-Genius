/*
 * hq_sound.c
 *
 *  Created on: 05.01.2009
 *      Author: gerstrong
 */
#include <SDL.h>
#include "../hqp/hq_sound.h"
#include "../keen.h"
#include "../sdl/CVideoDriver.h"
#include "../vorbis/oggsupport.h"
#include "../CLogFile.h"
#include "../FindFile.h"
#include "../fileio/ResourceMgmt.h"

#include <vector>

short HQSndDrv_Load(SDL_AudioSpec *AudioSpec, stHQSound *psound, const std::string& gamepath, const std::string& soundname)
{
	SDL_AudioSpec AudioFileSpec;
	SDL_AudioCVT  Audio_cvt;

	psound->sound_buffer = NULL;
	std::string buf;
	FILE *fp = NULL;


	buf = getResourceFilename("snd/" + soundname + ".OGG", gamepath, false, true); // Start with OGG

	if(buf != "")
		fp = OpenGameFile(buf.c_str(), "rb");

	if(fp != NULL)
	{
#ifdef OGG
		if(openOGGSound(fp, &AudioFileSpec, AudioSpec->format, psound) != 0)
		{
			std::string buf2;
			buf2 = "OGG file \"" + buf +"\"could not be opened. The file was detected, but appears to be damaged. Trying to load the classical sound<br>";
			g_pLogFile->ftextOut(PURPLE,buf2.c_str());
			return 1;
		}
		psound->enabled = true;

#endif

#ifndef OGG
		g_pLogFile->textOut(PURPLE,"NOTE: OGG-Support is disabled! Get another version or compile it yourself!<br>");
		buf = "games/hqp/snd/"+ soundname + ".WAV";

		std::string fullfname = GetFullFileName(buf);
		if(fullfname.size() == 0)
			return 1;
		
		// Check, if it is a wav file or go back to classic sounds
		  if (SDL_LoadWAV (Utf8ToSystemNative(fullfname).c_str(), &AudioFileSpec, &(psound->sound_buffer), &(psound->sound_len)) == NULL)
		  {
			  g_pLogFile->ftextOut(PURPLE,"Wave file %s could not be opened: \"%s\". Trying to load the classical sound<br>", buf.c_str(), SDL_GetError());
		      return 1;
		  }
#endif
	}
	else
	{
		buf = getResourceFilename("snd/" + soundname + ".WAV", gamepath, false); // Start with OGG

		if(buf == "")
			return 1;

		// Check, if it is a wav file or go back to classic sounds
		if (SDL_LoadWAV (Utf8ToSystemNative(GetFullFileName(buf)).c_str(), &AudioFileSpec, &(psound->sound_buffer), &(psound->sound_len)) == NULL)
		{
			g_pLogFile->textOut(GREEN, "Loading the classic sound : " + soundname);
			return 1;
		}
	}

	psound->sound_pos = 0;
	g_pLogFile->textOut(PURPLE,"File \"%s\" opened successfully!<br>", buf.c_str());

	int ret;
	/* Build AudioCVT (This is needed for the conversion from one format to the one used in the game)*/
	ret = SDL_BuildAudioCVT(&Audio_cvt,
							AudioFileSpec.format, AudioFileSpec.channels, AudioFileSpec.freq,
							AudioSpec->format, AudioSpec->channels, AudioSpec->freq);

	/* Check that the convert was built */
	if(ret == -1){
		g_pLogFile->textOut(PURPLE,"Couldn't convert the sound correctly!<br>");
		SDL_FreeWAV(psound->sound_buffer);
		return 1;
	}

	/* Setup for conversion, copy original data to new buffer*/
	Audio_cvt.buf = (Uint8*) malloc(psound->sound_len * Audio_cvt.len_mult);
	Audio_cvt.len = psound->sound_len;
	memcpy(Audio_cvt.buf, psound->sound_buffer, psound->sound_len);

	/* We can delete to original WAV data now */
	SDL_FreeWAV(psound->sound_buffer);

	/* And now we're ready to convert */
	SDL_ConvertAudio(&Audio_cvt);

	/* copy the converted stuff to the original music buffer*/
	psound->sound_len = Audio_cvt.len_cvt;
	psound->sound_buffer = (Uint8*) malloc(psound->sound_len);

	memcpy(psound->sound_buffer, Audio_cvt.buf, psound->sound_len);

	// Structure Audio_cvt must be freed!
	free(Audio_cvt.buf);

	return 0;
}

void HQSndDrv_Unload(stHQSound *psound)
{
	if(psound->sound_buffer){ free(psound->sound_buffer); psound->sound_buffer = NULL;}
}
