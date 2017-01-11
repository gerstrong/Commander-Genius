#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <base/utils/StringUtils.h>
#include <graphics/GsGraphics.h>

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
    mMainMenuBmp(gGraphics.getBitmapFromId(0)), // Zero always seem to be that menu
    mHandBmp(*gGraphics.getBitmapFromStr("HELPHAND"))
{
    // Prepare the Menu Bmp
    GsRect<Uint16> mainBmpSize;
    mainBmpSize.w = mMainMenuBmp.width();
    mainBmpSize.h = mMainMenuBmp.height();
    mMainMenuBmp.scaleTo(mainBmpSize);

    // Same for Hand Bmp
    GsRect<Uint16> handBmpSize;
    handBmpSize.w = mHandBmp.width();
    handBmpSize.h = mHandBmp.height();
    mHandBmp.scaleTo(handBmpSize);


    GsRect<Uint16> upperBorderBmpSize;
    mUpperBorderBmp = *gGraphics.getBitmapFromStr("HELP_UPPERBORDER");
    upperBorderBmpSize.w = mUpperBorderBmp.width();
    upperBorderBmpSize.h = mUpperBorderBmp.height();
    mUpperBorderBmp.scaleTo(upperBorderBmpSize);

    GsRect<Uint16> leftBorderBmpSize;
    mLeftBorderBmp = *gGraphics.getBitmapFromStr("HELP_LEFTBORDER");
    leftBorderBmpSize.w = mLeftBorderBmp.width();
    leftBorderBmpSize.h = mLeftBorderBmp.height();
    mLeftBorderBmp.scaleTo(leftBorderBmpSize);

    GsRect<Uint16> rightBorderBmpSize;
    mRightBorderBmp = *gGraphics.getBitmapFromStr("HELP_RIGHTBORDER");
    rightBorderBmpSize.w = mRightBorderBmp.width();
    rightBorderBmpSize.h = mRightBorderBmp.height();
    mRightBorderBmp.scaleTo(rightBorderBmpSize);

    GsRect<Uint16> bottomBorderBmpSize;
    mBottomBorderBmp = *gGraphics.getBitmapFromStr("HELP_LOWERBORDER");
    bottomBorderBmpSize.w = mBottomBorderBmp.width();
    bottomBorderBmpSize.h = mBottomBorderBmp.height();
    mBottomBorderBmp.scaleTo(bottomBorderBmpSize);


    // NOTE: The index is always at six
    mBmpIndex = 6;
}


void ComputerWrist::ponderPage(const float deltaT)
{
    if( gInput.getPressedCommand(IC_BACK) )
    {
        mSectionPage = -1;
    }
}

void ComputerWrist::ponderMainMenu(const float deltaT)
{
    if( gInput.getPressedCommand(IC_BACK) )
    {
        gEventManager.add( new CloseComputerWrist() );
    }

    if( gInput.getPressedCommand(IC_UP) )   mSelection--;
    if( gInput.getPressedCommand(IC_DOWN) ) mSelection++;


    // Guarantee the boundaries are never exploited
    mSelection = std::max(mSelection, 0);
    mSelection = std::min(mSelection, mNumOptions-1);

    if( gInput.getPressedCommand(IC_JUMP) || gInput.getPressedCommand(IC_STATUS) )
    {
        mSectionPage = mSelection;
        mCurrentTextLines = gGameText.readPage(0, 0);
    }


    GsFont &font = gGraphics.getFont(mFontId);
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());
    const auto numElem = (blitsfc.height()*blitsfc.width());

    const auto numLines = (blitsfc.height() - (mUpperBorderBmp.height() + mBottomBorderBmp.height()))/font.getPixelTextHeight();

    for(int i=0 ; i<numLines ; i++)
    {
        mMinPos.push_back(mLeftBorderBmp.width());
        mMaxPos.push_back(blitsfc.width() - (mLeftBorderBmp.width() + mRightBorderBmp.width()) );
    }
}

void ComputerWrist::ponder(const float deltaT)
{
    // Main Page?
    if(mSectionPage == -1)
    {
        ponderMainMenu(deltaT);
        return;
    }

    // Otherwise show the page
    ponderPage(deltaT);
}

void ComputerWrist::parseText()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    SDL_Rect lRect;
    lRect.h = blitsfc.height();
    lRect.w = blitsfc.width()-(mLeftBorderBmp.width()+mRightBorderBmp.width());

    // Draw some text.
    GsFont &Font = gGraphics.getFont(mFontId);

    auto fontHeight = Font.getPixelTextHeight();


    bool skipmode = true;

    Vector2D<int> cursorPos(mMinPos[0], 0);

    // Get the text that is actually used
    for(const auto &line : mCurrentTextLines)
    {
        int subPos = 0;

        if(skipmode && line[0] == '^')
        {
            Font.setupColor(0xFFFF00);
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
            else if(line[1] == 'c')
            {
                // TODO: Color detection
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
                int wordWidth = 0;

                for(const auto c : word)
                {
                    wordWidth += Font.getWidthofChar(c);
                }

                auto maxPosX = mMaxPos[cursorPos.y];

                if(cursorPos.x+wordWidth > maxPosX)
                {
                    cursorPos.y++;

                    // Ensure the minimum position is not hindering a picture
                    cursorPos.x = mMinPos[cursorPos.y];
                }

                Font.drawFont(blitsfc.getSDLSurface(), word, cursorPos.x, cursorPos.y*fontHeight+mUpperBorderBmp.height());

                cursorPos.x += (wordWidth+Font.getWidthofChar(' '));
            }

            cursorPos.y++;
            cursorPos.x = mMinPos[cursorPos.y];
        }
    }
}

void ComputerWrist::parseGraphics()
{
    std::stringstream ss;

    int x,y,chunk;
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    SDL_Rect lRect;
    lRect.h = blitsfc.height();     lRect.w = blitsfc.width();

    const auto fontHeight = gGraphics.getFont(mFontId).getPixelTextHeight();
    const auto spaceWidth = gGraphics.getFont(mFontId).getWidthofChar(' ');

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

                GsBitmap &bmp = gGraphics.getBitmapFromId(chunk);
                const auto bmpH = bmp.height();
                const auto bmpW = bmp.width();

                // Got the bitmap, block the matrix at that part
                for(int i=y ; i<y+bmpH ; i++)
                {
                    // Wrap left side text
                    if((x+bmpW)/2 < lRect.w/2)
                    {
                        // Left hand wrap
                        for(int j=x ; j<x+bmpW ; j++)
                        {
                            auto curMinPos = mMinPos[i/fontHeight];

                            if(curMinPos < j+spaceWidth)
                            {
                                mMinPos[i/fontHeight] = j+spaceWidth;
                            }
                        }
                    }
                    else    // Wrap right side text
                    {
                        for(int j=x ; j<spaceWidth+bmpW ; j++)
                        {
                            auto curMaxPos = mMaxPos[i/fontHeight];

                            if(curMaxPos > j)
                            {
                                mMaxPos[i/fontHeight] = j;
                            }
                        }
                    }
                }

                bmp.draw(x, y);
            }
        }
    }
}

void ComputerWrist::renderPage()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    SDL_Rect lRect;
    lRect.h = blitsfc.height();     lRect.w = blitsfc.width();
    lRect.x = 0;                    lRect.y = lRect.h-20;

    // Draw some text.
    GsFont &Font = gGraphics.getFont(mFontId);

    Font.setupColor(0xFFFFFF);

    Font.drawFontCentered( blitsfc.getSDLSurface(), "Under Construction! Press Back (Esc)...", lRect.x, lRect.w, lRect.y, false);

    int linePos = 0;

    lRect.h = Font.getPixelTextHeight();

    lRect.x = 0;    lRect.y = 0;

    const auto screenW = blitsfc.width() - (mRightBorderBmp.width()+mLeftBorderBmp.width());
    const auto screenH = blitsfc.height() - (mUpperBorderBmp.height()+mBottomBorderBmp.height());

    const auto height = Font.getPixelTextHeight();

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

    mUpperBorderBmp.draw(0, 0);
    mLeftBorderBmp.draw(0, mUpperBorderBmp.height());

    mRightBorderBmp.draw(blitsfc.width()-mRightBorderBmp.width(), mUpperBorderBmp.height());
    mBottomBorderBmp.draw(mLeftBorderBmp.width(), mLeftBorderBmp.height());
}

void ComputerWrist::renderMainMenu()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Bring borders to the screen
    renderBorders();

    // Centered Bitmap to draw
    const auto mainMenu_x = (blitsfc.width()-mMainMenuBmp.width()) / 2;
    const auto mainMenu_y = mUpperBorderBmp.height();

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
    if(mSectionPage == -1)
    {
        renderMainMenu();
        return;
    }

    // Otherwise show the page
    renderPage();
}

}
