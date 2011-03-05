#include "id_sd.h"
//#include "fmopl.h"
#include "sdl/music/CIMFPlayer.h"
#include <SDL.h>
#include <string>

CIMFPlayer *g_imfplayer;
SDL_AudioSpec *actual_audiospec;
COPLEmulator* p_opl_emulator;

void SDL_IMFMusicPlayer(void *udata, Uint8 *stream, int len)
{
    g_imfplayer->readBuffer(stream, len);
/*  SDL_MixAudio(stream, 0, len, SDL_MIX_MAXVOLUME);*/
}


///////////////////////////////////////////////////////////////////////////
//
//      SD_Startup() - starts up the Sound Mgr
//              Detects all additional sound hardware and installs my ISR
//
///////////////////////////////////////////////////////////////////////////
void
SD_Startup(int mixer_rate)
{
    // Open the audio device
    SDL_AudioSpec *desired;

    // Allocate a desired SDL_AudioSpec
    desired = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
    actual_audiospec = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
    desired->freq=mixer_rate;
    desired->format=AUDIO_S16SYS;
    desired->channels=2;
    desired->samples = (mixer_rate * 2048) / 44100;
    desired->callback=SDL_IMFMusicPlayer;
    desired->userdata=NULL;

    // Open the audio device
    if ( SDL_OpenAudio(desired, actual_audiospec) < 0 ){
      fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
      return;
    }

    // desired spec is no longer needed
    free(desired);

    p_opl_emulator = new COPLEmulator(*actual_audiospec);
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_StartMusic() - starts playing the music pointed to
//
///////////////////////////////////////////////////////////////////////////
void
SD_StartIMF(const std::string& filename, int imf_clock_rate, int opl_rate)
{
	actual_audiospec->freq=opl_rate; // Yeah, a little "cheating"...

	p_opl_emulator->setIMFClockrate(imf_clock_rate);

	g_imfplayer = new CIMFPlayer(filename, *actual_audiospec, *p_opl_emulator);
	SDL_PauseAudio(0);
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_Shutdown() - shuts down the Sound Mgr
//              Removes sound ISR and turns off whatever sound hardware was active
//
///////////////////////////////////////////////////////////////////////////
void
SD_Shutdown(void)
{
	if(g_imfplayer)
		free(g_imfplayer);
	if (actual_audiospec)
		free(actual_audiospec);

	if(p_opl_emulator)
		delete p_opl_emulator;
}
