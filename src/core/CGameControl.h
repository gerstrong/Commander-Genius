/*
 * GsAppState.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 *
 *  Auxiliary Class for CGame. It only tells the Game-Engine what to do.
 */

#ifndef GsAppState_H_
#define GsAppState_H_

//#include "common/CGameLauncher.h"
//#include "common/options.h"
//#include "common/Menu/CMenuController.h"
//#include "core/CBaseEngine.h"
//#include "engine/CEGAGraphics.h"
//#include "engine/CMessages.h"
//#include "engine/CPassive.h"
//#include "core/mode/CGameMode.h"

#include <string>
#include <memory>

class GsAppState /*: public GameState*/
{
public:

    GsAppState(bool &firsttime);
	
	bool init(int argc, char *argv[]);

    void pollEvents();

    void ponder();

    void render(const float deltaT);

    /*void operator=(const GameState &rhs);

    void operator=(const GameState &&rhs);*/

	bool mustShutdown(){ return (mpEngine.get()==nullptr); }
	
protected:

	std::unique_ptr<CBaseEngine> mpEngine;

	bool &m_firsttime;
	int m_startGame_no;
	int m_startLevel;
	Difficulty m_startDifficulty;
};

#endif /* GsAppState_H_ */
