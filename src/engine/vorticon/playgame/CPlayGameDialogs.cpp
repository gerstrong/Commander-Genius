/*
 * CPlayGameDialogs.cpp
 *
 *  Created on: 15.10.2009
 *      Author: gerstrong
 *
 *  This file is part of CPlayGame class and processes the dialogs like Statusscreen
 *  showhint Message, Game paused, etc.
 */

#include "CPlayGameVorticon.h"

void CPlayGameVorticon::processPauseDialogs()
{
	if(m_showPauseDialog)
	{
		showPausedGameDlg();
		return;
	}
	
	if(!m_MessageBoxes.empty())
	{
		CMessageBoxVort *pMB = m_MessageBoxes.front();
		pMB->processLogic();

		if( pMB->isFinished() )
		{
			SAFE_DELETE(pMB);
			m_MessageBoxes.pop_front();
		}
		return;
	}


}

void CPlayGameVorticon::showPausedGameDlg()
{
	// Open the Pause Dialog
	//drawTextMB("Game Paused");
}
