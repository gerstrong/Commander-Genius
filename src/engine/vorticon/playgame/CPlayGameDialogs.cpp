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
		pMB->process();

		if(pMB->isFinished())
		{
			SAFE_DELETE(pMB);
			m_MessageBoxes.pop_front();
			if(m_MessageBoxes.empty())
				m_paused = false;
		}
		return;
	}

	bool all_status_screens_closed=true;
	for( short i=0 ; i<m_NumPlayers ; i++ )
	{
		all_status_screens_closed &= m_Player[i].drawStatusScreen();
	}
	
	if(all_status_screens_closed) m_paused = false; // unpause the game
}

void CPlayGameVorticon::showPausedGameDlg()
{
	// Open the Pause Dialog
	//drawTextMB("Game Paused");
}
