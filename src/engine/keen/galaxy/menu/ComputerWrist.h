#ifndef COMPUTERWRIST_H
#define COMPUTERWRIST_H

#include <graphics/GsBitmap.h>
#include <base/GsEvent.h>

namespace galaxy
{


/**
 * @brief The CloseComputerWrist struct Close event for Computer Wrist Menu
 */
struct CloseComputerWrist : CEvent
{};


/**
 * @brief The OpenComputerWrist struct Open event for Computer Wrist Menu
 */
struct OpenComputerWrist : CEvent
{
    OpenComputerWrist() {}

    OpenComputerWrist(const int section,
                      const bool grayMode,
                      const bool showPageNo,
                      const int playsong,
                      const bool noMainMenu) :
        mSection(section),        
        mPlaySong(playsong),
        mGrayMode(grayMode),
        mShowPageNo(showPageNo),
        mNoMainMenu(noMainMenu) {}

    const int mSection = -1;
    const int mPlaySong = -1;
    const bool mGrayMode = false;    
    const bool mShowPageNo = false;
    const bool mNoMainMenu = false;
};



/**
 * @brief The ComputerWrist class   Interpreter class for the game text blocks found in the galaxy games
 */
class ComputerWrist
{
public:

    /**
     * @brief ComputerWrist A default constructor
     */
    ComputerWrist(const bool greyMode,
                  const bool showPageNo,
                  const bool noMainMenu);

    /**
     * @brief ComputerWrist A constructor with start section
     */
    ComputerWrist(const bool greyMode,
                  const bool showPageNo,
                  const bool noMainMenu,
                  const int section);

    virtual ~ComputerWrist();

    void playSong(const int song);


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
     * @brief parseText Detect, parser and render the text
     */
    void parseText();

    /**
     * @brief parseGraphics parse graphical elements and block the char matrix
     */
    void parseGraphics();

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
    GsBitmap mLowerBorderControlBmp;

    /////////  For the Main Menu only /////////////////////
    GsBitmap mMainMenuBmp;
    GsBitmap mHandBmp;

    int mSelection = 0; /** @brief mSelection    Currently selected Item in the Help Menu */
    int mNumOptions = 5;    /** Number of existing Help pages */

    ///////// For showing the pages ///////////////////////
    int  mSection = -1;
    int  mSectionPage = 0;

    int mNumPagesOfThisSection = 0;

    std::vector<std::string> mCurrentTextLines;

    int mBmpIndex = 0;
    int mFontId = 0;

    std::vector<int> mMinPos;
    std::vector<int> mMaxPos;   

    int mPlaySong = -1;

    const bool mGreyMode = false;
    const bool mShowPageNo = false;
    const bool mNoMainMenu = false;
    int mTime = 0;

    GsVec2D<int> mTxtWrapLoc;
    int mArrow_Pic_Start = 41;
};

}

#endif // COMPUTERWRIST_H
