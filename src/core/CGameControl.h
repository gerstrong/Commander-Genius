/*
 * CGameControl.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 *
 *  Auxiliary Class for CGame. It only tells the Game-Engine what to do.
 */

#ifndef CGAMECONTROL_H_
#define CGAMECONTROL_H_

#include "common/CGameLauncher.h"
#include "common/options.h"
#include "common/Menu/CMenuController.h"
#include "core/CBaseEngine.h"
#include "engine/CEGAGraphics.h"
#include "engine/CMessages.h"
#include "engine/CPassive.h"
#include "core/mode/CGameMode.h"

#include <string>
#include <memory>

class CGameControl
{
public:

	CGameControl(bool &firsttime);
	
	bool init(int argc, char *argv[]);

	void process();
	
	// getters and setters
	bool mustShutdown(){ return (mpEngine.get()==NULL); }
	
protected:

	std::unique_ptr<CBaseEngine> mpEngine;

	bool &m_firsttime;
	int m_startGame_no;
	int m_startLevel;
};

#endif /* CGAMECONTROL_H_ */
