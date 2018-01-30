/*
 * CMusic.h
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 *
 *  This class is a singleton and management class.
 *  The Music player itself is called here!
 */

#ifndef CMUSIC_H_
#define CMUSIC_H_

#include "fileio/CExeFile.h"
#include "CMusicPlayer.h"

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>
#include <base/GsEvent.h>

#include <base/Singleton.h>

#if defined(USE_SDLMIXER)
#include <SDL_mixer.h>
#endif


#define gMusicPlayer CMusic::get()

class CMusic : public GsSingleton<CMusic>
{
public:

    virtual ~CMusic();

	/**
     * Loads given track of the Keen Music
	 */
    bool loadTrack(const int track);

    bool load(const std::string &musicfile);

	void reload();
	void play();
	void pause();
	void stop();
	void readWaveform(Uint8* buffer, size_t length); // Reads the next chunk of the ogg stream


    bool LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename);

    int readTrackNofromMusicTable(const std::string &gamepath,
                                  const std::string &levelfilename);

	bool LoadfromSonglist(const std::string &gamepath, const int &level);

    bool paused()
    {
        if(!active())
        {
            return false;
        }

        #if defined(USE_SDLMIXER)
        return Mix_PausedMusic();
        #else
        return mpPlayer->playing();
        #endif
    }

	bool playing()
    {
        if(!active())
        {
            return false;
        }

        #if defined(USE_SDLMIXER)
        return Mix_PlayingMusic();
        #else
        return mpPlayer->playing();
        #endif
    }

    bool active()
    {
        #if defined(USE_SDLMIXER)
             return Mix_PlayingMusic();
        #else
        if(mpPlayer)
            return true;
        else
            return false;
        #endif
    }


private:

#if !defined(USE_SDLMIXER)
	std::unique_ptr<CMusicPlayer> mpPlayer;
#endif


#if defined(USE_SDLMIXER)
    Mix_Music *mpMixMusic = nullptr;
#endif


};

struct EventPlayTrack : CEvent {
    const uint32_t track;
    EventPlayTrack(const uint16_t t) : track(t) {}
};

#endif /* CMUSIC_H_ */
