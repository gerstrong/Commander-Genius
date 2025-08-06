/*
 * Scene.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 *
 *  Base class responsible for controlling the CG Modes which are Launcher Menu, Passive and Play Mode
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <base/GsEvent.h>

#include <string>

///////////////////////
// Events Structure  //
///////////////////////
// This event switches to the PassiveMode
struct GMSwitchToPassiveMode : CEvent
{};

// This event switches to the PlayGameMode
struct GMSwitchToPlayGameMode : CEvent
{
    const int m_Episode;
    const std::string m_DataDirectory;
    const int m_startlevel;

    GMSwitchToPlayGameMode( const int Episode,
                const std::string& DataDirectory,
                            const int startlevel = -1 ) :
                m_Episode(Episode),
                m_DataDirectory(DataDirectory),
                m_startlevel(startlevel)
                {}
};

// Event for opening the download Dialog
struct GMDownloadDlgOpen : CEvent {};

// It's a simple start event which will force CG launch a game
struct GMStart : CEvent {};

struct GMPatchSelected : CEvent {};


// Happens when in the menu the user decides to end the game or he reached
// the finale
struct EventEndGamePlay : CEvent {};


// When user wants to see the high scores
struct EventHighScores : CEvent {};

///////
// Base Class for game modes
//////
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    virtual void pumpEvent(const std::shared_ptr<CEvent> &evPtr) = 0;

    virtual void ponder(const float deltaT);
    virtual void render() = 0;

    bool needInit();

protected:
    virtual bool init();

private:
    bool mInited = false;
};


#endif /* SCENE_H_ */
