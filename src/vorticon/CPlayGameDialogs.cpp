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

void CPlayGame::processPauseDialogs()
{
	if(m_showPauseDialog)
	{
		showPausedGameDlg();
		return;
	}
	
	// TODO: draw the F1 Screen here if opened
	
	bool all_status_screens_closed=true;
	for( int i=0 ; i<m_NumPlayers ; i++ )
	{
		all_status_screens_closed &= mp_Player[i].drawStatusScreen();
	}
	
	if(all_status_screens_closed) m_paused = false; // unpause the game
}

void CPlayGame::showPausedGameDlg()
{
	// Open the Pause Dialog
	//drawTextMB("Game Paused");
}
