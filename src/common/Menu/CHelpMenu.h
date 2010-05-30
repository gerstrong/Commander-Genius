/*
 * CHelpMenu.h
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#ifndef CHELPMENU_H_
#define CHELPMENU_H_

#include "CBaseMenu.h"

#include "../../engine/infoscenes/CHighScores.h"
#include "../../engine/infoscenes/CStory.h"
#include "../../engine/infoscenes/CCredits.h"
#include "../../engine/infoscenes/COrderingInfo.h"
#include "../../engine/infoscenes/CAbout.h"
#include "../../engine/infoscenes/CHelp.h"

class CHelpMenu : public CBaseMenu
{
public:
	CHelpMenu(CInfoScene *&p_InfoScene, Uint8 dlg_theme,
			CExeFile &ExeFile,
			bool &hideobjects);
	void processSpecific();
	virtual ~CHelpMenu();

private:
	CExeFile &m_ExeFile;
	CInfoScene *&mp_InfoScene;

	const char m_Episode;
	const std::string m_GamePath;
	bool &m_hideobjects;
};

#endif /* CHELPMENU_H_ */
