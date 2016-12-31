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



    /**
     * @brief ponderPage    Logic for the individual page
     * @param deltaT        Time slice of the logic
     */
    void ponderPage(const float deltaT);

    /**
     * @brief ponderMainMenu    Logic for the help menu
     * @param deltaT    Time slice of the logic
     */
    void ponderMainMenu(const float deltaT);


    /**
     * @brief ponder    Main Process for menus
     * @param deltaT    Time slice of the logic
     */
    void ponder(const float deltaT);

    /**
     * @brief renderPage    Render the given Page
     */
    void renderPage();

    /**
     * @brief renderBorders Render the border around the help section
     */
    void renderBorders();

    /**
     * @brief renderMainMenu    Rrender the main Menu
     */
    void renderMainMenu();

    /**
     * @brief render    Main render method
     */
    void render();

private:

    /// Borders ///
    GsBitmap mUpperBorderBmp;
    GsBitmap mLeftBorderBmp;
    GsBitmap mRightBorderBmp;
    GsBitmap mBottomBorderBmp;

    /////////  For the Main Menu only /////////////////////
    GsBitmap mMainMenuBmp;
    GsBitmap mHandBmp;

    int mSelection = 0; /** @brief mSelection    Currently selected Item in the Help Menu */
    int mNumOptions = 5;    /** Number of existing Help pages */

    ///////// For showing the page ///////////////////////
    int  mSectionPage = -1;

    std::vector<std::string> mCurrentTextLines;
};

}

#endif // COMPUTERWRIST_H
