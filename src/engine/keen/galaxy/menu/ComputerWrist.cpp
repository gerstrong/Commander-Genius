#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <base/utils/StringUtils.h>
#include <graphics/GsGraphics.h>
#include <graphics/effects/CColorMerge.h>
#include <audio/music/CMusic.h>
#include <engine/core/CBehaviorEngine.h>

#include <sstream>

#include "ComputerWrist.h"

#include "graphics/GsGametext.h"

/*

^P		First command in every file. Defines a page start
^E		Ends the file
^Cx		Change font color to $x until next page or ^C command
^Gx,y,z	Display (unmasked) picture chunk z at location x,y (In pixels)
^Tx,y,z,t	Display picture chunk z at x,y for z clicks of time
^Bx,y,z,t,b	?
^Lx,y		?

*/

namespace galaxy
{


ComputerWrist::ComputerWrist(const int ep) :
    mMainMenuBmp(gGraphics.getBitmapFromId(0, 0)), // Zero always seem to be that menu
    mHandBmp(*gGraphics.getBitmapFromStr(0, "HELPHAND"))
{
    // Prepare the Menu Bmp
    GsRect<Uint16> mainBmpSize;
    mainBmpSize.dim = {mMainMenuBmp.width(), mMainMenuBmp.height()};
    mMainMenuBmp.scaleTo(mainBmpSize);

    // Same for Hand Bmp
    GsRect<Uint16> handBmpSize;
    handBmpSize.dim = {mHandBmp.width(), mHandBmp.height()};
    mHandBmp.scaleTo(handBmpSize);


    if(ep != 6)
    {

        GsRect<Uint16> upperBorderBmpSize;
        mUpperBorderBmp = *gGraphics.getBitmapFromStr(0, "HELP_UPPERBORDER");
        upperBorderBmpSize.dim.x = mUpperBorderBmp.width();
        upperBorderBmpSize.dim.y = mUpperBorderBmp.height();
        mUpperBorderBmp.scaleTo(upperBorderBmpSize);

        GsRect<Uint16> leftBorderBmpSize;
        mLeftBorderBmp = *gGraphics.getBitmapFromStr(0, "HELP_LEFTBORDER");
        leftBorderBmpSize.dim.x = mLeftBorderBmp.width();
        leftBorderBmpSize.dim.y = mLeftBorderBmp.height();
        mLeftBorderBmp.scaleTo(leftBorderBmpSize);

        GsRect<Uint16> rightBorderBmpSize;
        mRightBorderBmp = *gGraphics.getBitmapFromStr(0, "HELP_RIGHTBORDER");
        rightBorderBmpSize.dim.x = mRightBorderBmp.width();
        rightBorderBmpSize.dim.y = mRightBorderBmp.height();
        mRightBorderBmp.scaleTo(rightBorderBmpSize);




        GsRect<Uint16> bottomBorderBmpSize;
        mBottomBorderBmp = *gGraphics.getBitmapFromStr(0, "HELP_LOWERBORDER");
        bottomBorderBmpSize.dim.x = mBottomBorderBmp.width();
        bottomBorderBmpSize.dim.y = mBottomBorderBmp.height();
        mBottomBorderBmp.scaleTo(bottomBorderBmpSize);

        GsRect<Uint16> lowerBorderBmpSize;
        mLowerBorderControlBmp = *gGraphics.getBitmapFromStr(0, "HELP_LOWERBORDERCONTROL");
        lowerBorderBmpSize.dim.x = mLowerBorderControlBmp.width();
        lowerBorderBmpSize.dim.y = mLowerBorderControlBmp.height();
        mLowerBorderControlBmp.scaleTo(lowerBorderBmpSize);

    }

    // NOTE: The index is always six here
    mBmpIndex = 6;

    auto &font = gGraphics.getFontLegacy(mFontId);
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    int numLines = blitsfc.height();

    if( ep != 6 )
    {
        numLines = (blitsfc.height() - (mUpperBorderBmp.height() + mBottomBorderBmp.height()))/font.getPixelTextHeight();

        // first line is full, because of upper border line bitmap
        mMinPos.push_back(mUpperBorderBmp.width());
        mMaxPos.push_back(mUpperBorderBmp.width());

        for(int i=1 ; i < numLines-1 ; i++)
        {
            mMinPos.push_back(mLeftBorderBmp.width()+2);
            mMaxPos.push_back(blitsfc.width() - (mLeftBorderBmp.width() + mRightBorderBmp.width() + 0) );
        }

        // last line is full, because of lower border line
        mMinPos.push_back(mBottomBorderBmp.width());
        mMaxPos.push_back(mBottomBorderBmp.width());
    }
    else
    {
        for(int i=0 ; i < numLines ; i++)
        {
            mMinPos.push_back(2);
            mMaxPos.push_back(blitsfc.width());
        }
    }

    CColorMerge *pColorMergeFX = new CColorMerge(16);
    gEffectController.setupEffect(pColorMergeFX);
}

ComputerWrist::ComputerWrist(const int ep, const int section) :
    ComputerWrist(ep)
{
    mSection = section;

    mCurrentTextLines = gGameText.readPage(mSection, mSectionPage);
    mNumPagesOfThisSection = gGameText.getNumPages(mSection);
}

ComputerWrist::~ComputerWrist()
{
    const auto curStr = mPreviousSong;

    gMusicPlayer.stop();
    if( gMusicPlayer.load(curStr) )
        gMusicPlayer.play();
}

void ComputerWrist::playSong(const int song)
{
    auto &musPlayer = gMusicPlayer;
    if(song >= 0)
    {
        const auto curStr = musPlayer.getCurTrackPlaying();
        mPreviousSong = curStr;

        musPlayer.stop();
        if( musPlayer.loadTrack(song) )
           musPlayer.play();
    }
}

void ComputerWrist::ponderPage(const float deltaT)
{
    (void) deltaT;

    if( gInput.getPressedCommand(IC_BACK) )
    {
        mSection = -1;
        mSectionPage = 0;
    }


    if( gInput.getPressedCommand(IC_RIGHT) )
    {
        if(mSectionPage < mNumPagesOfThisSection)
        {
            mSectionPage++;
            mCurrentTextLines = gGameText.readPage(mSection, mSectionPage);
        }
    }

    if( gInput.getPressedCommand(IC_LEFT) )
    {
        if(mSectionPage > 0)
        {
            mSectionPage--;
            mCurrentTextLines = gGameText.readPage(mSection, mSectionPage);
        }
    }

}

void ComputerWrist::ponderMainMenu(const float deltaT)
{
    (void) deltaT;

    if( gInput.getPressedCommand(IC_BACK) )
    {
        gEventManager.add( new CloseComputerWrist() );
    }

    if( gInput.getPressedCommand(IC_UP) )   mSelection--;
    if( gInput.getPressedCommand(IC_DOWN) ) mSelection++;


    // Guarantee the boundaries are never exploited
    mSelection = std::max(mSelection, 0);
    mSelection = std::min(mSelection, mNumOptions-1);
    const int ep = gBehaviorEngine.getEpisode();

    if( gInput.getPressedCommand(IC_JUMP) || gInput.getPressedCommand(IC_STATUS) )
    {
        mSection = mSelection;

        // Strange cross mapping of the selection against Section to load
        if(ep == 5)
        {
            switch(mSelection)
            {
            case 3: mSection = 6; break;
            case 4: mSection = 3; break;
            default: break;
            }
        }
        else
        {
            switch(mSelection)
            {
            case 1: mSection = 2; break;
            case 2: mSection = 1; break;
            case 4: mSection = 3; break;
            default: break;
            }
        }

        mSectionPage = 0;
        mCurrentTextLines = gGameText.readPage(mSection, mSectionPage);
        mNumPagesOfThisSection = gGameText.getNumPages(mSection);
    }


}

void ComputerWrist::ponder(const float deltaT)
{
    // Main Page?
    if(mSection == -1)
    {
        ponderMainMenu(deltaT);
        return;
    }

    // Otherwise show the page
    ponderPage(deltaT);
}

void ComputerWrist::parseGraphics()
{
    std::stringstream ss;

    const int ep = gBehaviorEngine.getEpisode();

    int x,y,chunk;
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    SDL_Rect lRect;
    lRect.h = blitsfc.height();     lRect.w = blitsfc.width();

    const auto fontHeight = gGraphics.getFontLegacy(mFontId).getPixelTextHeight();
    const auto spaceWidth = gGraphics.getFontLegacy(mFontId).getWidthofChar(' ');


    if(!mMinPos.empty())
    {
        if(ep != 6)
        {
            mMinPos[0] = mUpperBorderBmp.width();
            mMaxPos[0] = mUpperBorderBmp.width();

            for(unsigned int i = 1 ; i<mMinPos.size() ; i++)
            {
                mMinPos[i] = mLeftBorderBmp.width()+2;
            }

            for(unsigned int i = 1 ; i<mMaxPos.size() ; i++)
            {
                mMaxPos[i] = blitsfc.width() - mRightBorderBmp.width();
            }
        }
        else
        {
            for(auto &minPos : mMinPos)
            {
                minPos = 2;
            }

            for(auto &maxPos : mMaxPos)
            {
                maxPos = blitsfc.width();
            }

        }
    }


    // Check for colission with bitmaps
    for(const auto &line : mCurrentTextLines)
    {
        if(line[0] == '^')
        {
            if(line[1] == 'G')
            {
                std::string param = line.substr(2);

                char comma;

                ss << param;
                ss >> y;
                ss >> comma;
                ss >> x;
                ss >> comma;
                ss >> chunk;

                chunk = chunk - mBmpIndex;

                GsBitmap &bmp = gGraphics.getBitmapFromId(0, chunk);
                const auto bmpH = bmp.height();
                const auto bmpW = bmp.width();

                // Got the bitmap, block the matrix at that part
                int minHeight = y;

                if(minHeight < 0)
                {
                    minHeight = 0;
                }

                for(int i=minHeight ; i<y+bmpH+1 ; i++)
                {
                    const auto textYIdx = (i)/fontHeight;

                    auto curMinPos = mMinPos[textYIdx];

                    // Whole pic fits to the right side
                    if(x > lRect.w/2)
                    {
                        // Limit max
                        mMaxPos[textYIdx] = lRect.w/2;
                    }
                    // Whole pic fits to the left side
                    else if((x+bmpW) < lRect.w/2)
                    {
                        // Left hand wrap
                        for(int j=x ; j<x+bmpW ; j++)
                        {
                            if(curMinPos < j+spaceWidth)
                            {
                                mMinPos[textYIdx] = j+spaceWidth+4;
                            }
                        }
                    }
                    else // Pic is both on left and right side
                    {
                        if(curMinPos < (x+bmpW))
                        {
                            mMinPos[textYIdx] = (x+bmpW)+spaceWidth+1;
                        }
                    }
                }

                bmp.draw(x, y);
            }
        }
    }
}

void ComputerWrist::parseText()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    int ep = gBehaviorEngine.getEpisode();

    // Draw some text.
    auto &Font = gGraphics.getFontLegacy(mFontId);

    auto fontHeight = Font.getPixelTextHeight();


    bool skipmode = true;

    GsVec2D<int> cursorPos(mMinPos[0], 0);


    static std::map<char, Uint32> wristColorMap =
    {
        {'b', 0x55FFFF},
        {'e', 0xFCFC54},
        {'f', 0xFFFFFF},
        {'B', 0x55FFFF},
        {'E', 0xFCFC54},
        {'F', 0xFFFFFF},
    };


    Uint32 color = 0xFCFC54;

    // Get the text that is actually used
    for(const auto &line : mCurrentTextLines)
    {
        int subPos = 0;

        if(skipmode && line[0] == '^')
        {
            Font.setupColor(color);
            skipmode = false;
        }

        if(skipmode && line[0] != '^')
        {
            skipmode = false;
        }


        if(skipmode)
        {
            continue;
        }


        if(line[0] == '^')
        {
            if(skipmode)
            {
               skipmode = true;
               continue;
            }
            else if(line[1] == 'c' || line[1] == 'C')
            {
                const char colorCode = line[2];
                color = wristColorMap[colorCode];
                Font.setupColor(color);
                subPos = 3;
            }
            else
            {
                skipmode = true;
            }
        }

        if(!skipmode)
        {
            std::string theText = line.substr(subPos);

            std::vector<std::string> words = explode(theText, " ");

            // TODO: test for pictures. If there is one move cursor forward and adapt the width left

            for(const auto &word : words)
            {
                // Blank occupied space also, consider it.
                int wordWidth = Font.getWidthofChar(' ') + 1;

                auto word2Display = word;

                // There might be a change of color.
                if(word[0] == '^')
                {
                    word2Display = word2Display.substr(1);

                    if(word[1] == 'c' || word[1] == 'C')
                    {
                        const char colorCode = word[2];
                        color = wristColorMap[colorCode];
                        Font.setupColor(color);
                        word2Display = word2Display.substr(2);
                    }
                }

                for(const auto c : word2Display)
                {
                    wordWidth += Font.getWidthofChar(c);
                }

                if(!word2Display.empty())
                {
                    // Right side boundary checks

                    auto maxPosX = mMaxPos[cursorPos.y];

                    while(cursorPos.x+wordWidth > maxPosX)
                    {
                        cursorPos.y++;

                        // Ensure the minimum position is not hindering a picture
                        cursorPos.x = mMinPos[cursorPos.y];
                        maxPosX = mMaxPos[cursorPos.y];
                    }

                    if(ep != 6)
                    {
                        Font.drawFont(blitsfc.getSDLSurface(), word2Display, cursorPos.x,
                                      cursorPos.y*fontHeight);
                    }
                    else
                    {
                        Font.drawFont(blitsfc.getSDLSurface(), word2Display,
                                      cursorPos.x, cursorPos.y*fontHeight+2);
                    }

                    cursorPos.x += (wordWidth);
                }
            }

            cursorPos.y++;
            cursorPos.x = mMinPos[cursorPos.y];
        }
    }

    // Print the page number
    const auto pgTextcolor = 0xFC5454;
    Font.setupColor(pgTextcolor);

    const std::string pgText =
            "pg " + itoa(mSectionPage+1) +
            " of " + itoa(mNumPagesOfThisSection+1);
    Font.drawFont(blitsfc.getSDLSurface(), pgText,
                  218, 186);
}

void ComputerWrist::renderPage()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    SDL_Rect lRect;
    lRect.h = blitsfc.height();     lRect.w = blitsfc.width();
    lRect.x = 0;                    lRect.y = lRect.h-20;

    // Draw some text.
    auto &Font = gGraphics.getFontLegacy(mFontId);

    Font.setupColor(0xFFFFFF);

    lRect.h = Font.getPixelTextHeight();

    lRect.x = 0;    lRect.y = 0;

    // Bring borders to the screen
    renderBorders();

    // Setup detected bitmap graphics
    parseGraphics();

    // Now parse/render the text
    parseText();

}

void ComputerWrist::renderBorders()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    int ep = gBehaviorEngine.getEpisode();

    if(ep != 6)
    {
        mUpperBorderBmp.draw(0, 0);
        mLeftBorderBmp.draw(0, mUpperBorderBmp.height());

        mRightBorderBmp.draw(blitsfc.width()-mRightBorderBmp.width(), mUpperBorderBmp.height());
        mBottomBorderBmp.draw(mLeftBorderBmp.width(), mLeftBorderBmp.height());

        if(mSection != -1)
        {
            mLowerBorderControlBmp.draw(mLeftBorderBmp.width(), mLeftBorderBmp.height()+mUpperBorderBmp.height()-mLowerBorderControlBmp.height() );
        }
    }
}

void ComputerWrist::renderMainMenu()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Bring borders to the screen
    renderBorders();

    // Centered Bitmap to draw
    const auto mainMenu_x = (blitsfc.width()-mMainMenuBmp.width()) / 2;
    auto mainMenu_y = (mUpperBorderBmp.empty()) ? 0 : mUpperBorderBmp.height();

    mMainMenuBmp.draw(mainMenu_x, mainMenu_y);

    const auto hand_x = mainMenu_x - mHandBmp.width();
    const auto hand_y = mainMenu_y + 40 + (mSelection*mHandBmp.height());

    mHandBmp.draw(hand_x, hand_y);
}

void ComputerWrist::render()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Typical color code of the background
    blitsfc.fillRGB(0xA8,0,0);


    // Main Page?
    if(mSection == -1)
    {
        renderMainMenu();
        return;
    }

    // Otherwise show the page
    renderPage();
}

}
