/*
 * CPlayGameDialogs.cpp
 *
 *  Created on: 15.10.2009
 *      Author: gerstrong
 *
 *  This file is part of CPlayGame class and processes the dialogs like Statusscreen
 *  showhint Message, Game paused, etc.
 */

#include "CPlayGame.h"

#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

void CPlayGame::processPauseDialogs()
{
	if(m_showPauseDialog)
	{
		showPausedGameDlg();
		return;
	}
	
	if(!m_MessageBoxes.empty())
	{
		CMessageBox *pMB = m_MessageBoxes.back();
		pMB->process();

		if(pMB->isFinished())
		{
			SAFE_DELETE(pMB);
			m_MessageBoxes.pop_back();
			if(m_MessageBoxes.empty())
				m_paused = false;
		}
		return;
	}

	// TODO: draw the F1 Screen here if opened
	
	bool all_status_screens_closed=true;
	for( short i=0 ; i<m_NumPlayers ; i++ )
	{
		all_status_screens_closed &= m_Player[i].drawStatusScreen();
	}
	
	if(all_status_screens_closed) m_paused = false; // unpause the game
}

void CPlayGame::showPausedGameDlg()
{
	// Open the Pause Dialog
	//drawTextMB("Game Paused");
}
