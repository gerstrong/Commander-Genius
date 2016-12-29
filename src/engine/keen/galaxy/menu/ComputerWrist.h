#ifndef COMPUTERWRIST_H
#define COMPUTERWRIST_H

#include "GalaxyMenu.h"

#include <graphics/GsBitmap.h>

namespace galaxy
{


/**
 * @brief The CloseComputerWrist struct Close event for Computer Wrist Menu
 */
struct CloseComputerWrist : CEvent
{};


/**
 * @brief The ComputerWrist class   Interpreter class for the game text blocks found in the galaxy games
 */
class ComputerWrist
{
public:

    /**
     * @brief ComputerWrist A default constructor
     */
    ComputerWrist();

    // Processes the stuff that the menus have in common
    void ponder(const float deltaT);

    void render();

private:

    GsBitmap mMainMenuBmp;

};

}

#endif // COMPUTERWRIST_H
