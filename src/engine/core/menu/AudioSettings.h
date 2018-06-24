/*
 * CAudioSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CAUDIOSETTINGS_H_
#define CAUDIOSETTINGS_H_

#include "engine/core/menu/GameMenu.h"

#include "widgets/Switch.h"
#include "widgets/Button.h"
#include "widgets/ComboSelection.h"
#include "widgets/NumberControl.h"

#include "fileio/CExeFile.h"
#include <SDL.h>
#include <string>



class CAudioSettings : public GameMenu
{
public:
    CAudioSettings(const GsControl::Style &style);

    void init();

    void ponder(const float deltaT) override;

    void release() override;

    void refresh() override;

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
    int mMusicVolume;
};


#endif /* CAUDIOSETTINGS_H_ */
