/*
 * CAudioSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CAUDIOSETTINGS_H_
#define CAUDIOSETTINGS_H_

#include "GalaxyMenu.h"

#include "widgets/Switch.h"
#include "widgets/Button.h"
#include "widgets/ComboSelection.h"
#include "widgets/NumberControl.h"

#include "fileio/CExeFile.h"
#include <SDL.h>
#include <string>


namespace galaxy
{


class CAudioSettings : public GalaxyMenu
{
public:
	CAudioSettings();

	void init();

    void ponder(const float deltaT);

	void release();


private:

    ComboSelection	*mpRate;
    Switch		*mpStereo;
    ComboSelection	*mpDepth;
    ComboSelection	*mpSBToggle;
    NumberControl	*mpSoundVolume;
    NumberControl	*mpMusicVolume;

	SDL_AudioSpec	mAudioSpec;
	bool mSoundblaster;

	// This one does not really set the volume at all, it only verifies, if it changed...
	int mSoundVolume;
};


}

#endif /* CAUDIOSETTINGS_H_ */
