/*
 * CPassive.h
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 *
 *  This Class controls the 3 screens that are shown
 *  when the game is started. THese are:
 *  Intro, Title and Demos.
 *  In that mode you can press any key to open the menu
 */

#ifndef CPASSIVE_H_
#define CPASSIVE_H_

#include "engine/core/CMap.h"
#include "engine/core/CSpriteObject.h"
#include "engine/core/mode/CGameMode.h"
#include "fileio/CSaveGameController.h"
#include <string>
#include <vector>
#include <memory>


class CPassive : public CGameMode
{
public:

    enum Modes
    {
        INTRO, TITLE, DEMO, STARTGAME, SHUTDOWN
    };

    CPassive();
    virtual ~CPassive();

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr);

    virtual bool init() = 0;

    virtual void ponder(const float deltaT) = 0;

    virtual void render() = 0;

    // Getters
    char getEpisode() { return m_Episode; }
    char getDifficulty() { return m_Difficulty; }
    std::string getGamePath() { return m_DataDirectory; }

    bool goBackToLauncherSet() { return mBackToGameLauncher; }
    bool mustStartGame() { return (m_mode==STARTGAME); }
    bool getExitEvent() { return (m_mode==SHUTDOWN); }

    virtual void cleanup() {}

    void createOutlinedText(const int splash_x, const int splash_y, const std::string &text);

    char m_mode = INTRO;

protected:
    char m_Episode;
    char m_Difficulty;

    // TODO: Bad variable name. Think about something better
    bool mBackToGameLauncher = false;
    std::string m_DataDirectory;
    CSaveGameController m_SavedGame;
    GsSurface mTextSfc;
};
#endif /* CPASSIVE_H_ */
