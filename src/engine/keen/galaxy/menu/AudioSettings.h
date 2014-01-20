/*
 * CAudioSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CAUDIOSETTINGS_H_
#define CAUDIOSETTINGS_H_

#include "GalaxyMenu.h"
#include <widgets/GsComboSelection.h>
#include <widgets/GsSwitch.h>
#include <widgets/GsNumberControl.h>

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

	CGUIComboSelection	*mpRate;
	CGUISwitch			*mpStereo;
	CGUIComboSelection	*mpDepth;
	CGUIComboSelection	*mpSBToggle;
	CGUINumberControl	*mpSoundVolume;
	CGUINumberControl	*mpMusicVolume;

	SDL_AudioSpec	mAudioSpec;
	bool mSoundblaster;

	// This one does not really set the volume at all, it only verifies, if it changed...
	int mSoundVolume;
};


}

#endif /* CAUDIOSETTINGS_H_ */
