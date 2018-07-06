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

#include <SDL_mixer.h>


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


    bool LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename);

    int readTrackNofromMusicTable(const std::string &gamepath,
                                  const std::string &levelfilename);

	bool LoadfromSonglist(const std::string &gamepath, const int &level);

    bool paused()
    {
        return Mix_PausedMusic();
    }

	bool playing()
    {
        return Mix_PlayingMusic();
    }

private:

    Mix_Music *mpMixMusic = nullptr;


};

struct EventPlayTrack : CEvent {
    const uint32_t track;
    EventPlayTrack(const uint16_t t) : track(t) {}
};

#endif /* CMUSIC_H_ */
