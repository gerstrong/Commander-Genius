/*
 * CAudioSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CAUDIOSETTINGS_H_
#define CAUDIOSETTINGS_H_

#include "CBaseMenu.h"
#include "CVolumeSettings.h"
#include "fileio/CExeFile.h"
#include <SDL.h>
#include <string>

class CAudioSettings : public CBaseMenu {
public:
	CAudioSettings(Uint8 dlg_theme, CExeFile &ExeFile);
	void processSpecific();
	void setFrequencyFromSlot(int value);

private:

	CExeFile &m_ExeFile;
	int m_Rate;
	int m_current; //used to show what is currently selected without actually changing the value
	Uint16 m_Format;
	int m_Mode;
	bool m_Soundblaster;
	CVolumeSettings *mp_VolumeMenu;
	bool m_must_restart_sounddriver;
};

#endif /* CAUDIOSETTINGS_H_ */
