/*
 * CMusicPlayer.h
 *
 *  Created on: 17.02.2011
 *      Author: gerstrong
 *
 *  This is standard music player bas class.
 *  In order to work properly it needs to be overloaded with
 *  a derived class
 */

#ifndef CMUSICPLAYER_H_
#define CMUSICPLAYER_H_

#include <SDL.h>
#include <string>

class CMusicPlayer
{
public:
    virtual ~CMusicPlayer() {}

    virtual bool loadMusicTrack(const int track) = 0;

    virtual bool loadMusicFromFile(const std::string& filename) = 0;
	virtual bool open() = 0;
	void reload();
	virtual void readBuffer(Uint8* buffer, Uint32 length) = 0;
	void play(const bool value);
    virtual void close() = 0;

	bool playing() const { return m_playing; }

protected:
    bool m_playing = false;
};

#endif /* CMUSICPLAYER_H_ */
