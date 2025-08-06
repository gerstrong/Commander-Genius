/*
 * CPlayGame.h
 *
 *  Created on: 26.05.2010
 *      Author: gerstrong
 *
 *  This is not the old class that before was called that way and served for the vorticon engine.
 *  This one serves for both engines and is a base class
 */

#ifndef CPLAYGAME_H_
#define CPLAYGAME_H_

#include "fileio/CExeFile.h"
#include "engine/core/options.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/mode/Scene.h"
#include "fileio/CSaveGameController.h"
#include <string>
#include <SDL.h>

const int WM_MAP_NUM = 80;


class CPlayGame : public Scene
{
public:
    CPlayGame(char level);

    virtual bool init() override;

    void loadGame();

    virtual bool loadXMLGameState() = 0;
    virtual bool saveXMLGameState() = 0;
    virtual void cleanup() = 0;

    virtual void pumpEvent(const std::shared_ptr<CEvent> &evPtr);

    virtual ~CPlayGame() {}

    virtual void ponder(const float deltaT) override;

    bool isFinished();
    bool getEndGame();
    bool getStartGame();


    bool getExitEvent()
    {	return m_exitgame;	}


    char getEpisode();
    char getNumPlayers();

    void renderFPSDisplay();

protected:
    bool m_endgame;
    char m_Episode;
    short m_Level;
    int mCamLead;
    bool m_startgame;
    bool m_exitgame;

    Uint32  m_NumSprites;

    enum
    {
        NONE, GOTO_WORLD_MAP,
        START_LEVEL, GOTO_FINALE
    } m_level_command;

    bool m_gameover;
    bool m_alldead;
    bool m_showPauseDialog;
    bool m_hideobjects;
    std::string m_Gamepath;
    int m_checkpoint_x, m_checkpoint_y;
    bool m_checkpointset;
    bool m_restartVideo;
};

#endif /* CPLAYGAME_H_ */
