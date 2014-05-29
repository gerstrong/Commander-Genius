/*
 * CAudioSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef __VORTICON_AUDIOSETTINGS_H_
#define __VORTICON_AUDIOSETTINGS_H_

#include "VorticonMenu.h"
#include "widgets/ComboSelection.h"
#include "widgets/Switch.h"
#include "widgets/NumberControl.h"

#include "fileio/CExeFile.h"
#include <SDL.h>
#include <string>

namespace vorticon
{

class CAudioSettings : public VorticonMenu
{
public:
	CAudioSettings();

    void refresh();

    void ponder(const float deltaT);

	void release();


private:

    ComboSelection	*mpRate;
    Switch			*mpStereo;
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

#endif /* __VORTICON_AUDIOSETTINGS_H_ */
