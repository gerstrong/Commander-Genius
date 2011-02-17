/*
 * CIMFPlayer.h
 *
 *  Created on: 17.02.2011
 *      Author: gerhard
 */

#ifndef CIMFPLAYER_H_
#define CIMFPLAYER_H_

#include "CMusicPlayer.h"
#include "fileio/TypeDefinitions.h"
#include "fileio/CExeFile.h"
#include <SDL.h>
#include <string>

class CIMFPlayer : public CMusicPlayer {
public:
	CIMFPlayer(const std::string& filename, const SDL_AudioSpec& AudioSpec);
	CIMFPlayer(const CExeFile& ExeFile, const int level, const SDL_AudioSpec &AudioSpec);
	virtual ~CIMFPlayer();

	bool open();
	void readBuffer(Uint8* buffer, Uint32 length);
	void close();

private:
    byte *mp_imfdata;
    Uint32 m_data_size;
    const SDL_AudioSpec& m_AudioSpec;
};

#endif /* CIMFPLAYER_H_ */
