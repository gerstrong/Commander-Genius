#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
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
    mPageMatrix.resize(numElem, ' ');
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

void ComputerWrist::parseGraphics()
{
    std::stringstream ss;

    int x,y,chunk;

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    SDL_Rect lRect;
    lRect.h = blitsfc.height();     lRect.w = blitsfc.width();

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
                    auto pIdx = i*blitsfc.width() + x;
                    for(int j=x ; j<x+bmpW ; j++)
                    {
                        mPageMatrix[pIdx] = 0;
                        pIdx++;
                    }
                }

                bmp.draw(x+mLeftBorderBmp.width(), y+mUpperBorderBmp.height()-4);
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

    // Setup detected bitmap graphics
    parseGraphics();

    for(lRect.y = mUpperBorderBmp.height() ; lRect.y<screenH ; )
    {
        auto pixelIdx = lRect.y * blitsfc.width();

        for(lRect.x = mLeftBorderBmp.width() ; lRect.x<screenW ; )
        {
            const char c = mPageMatrix[pixelIdx];


            int width = 8; // default

            if(c != 0)
            {
                Font.drawCharacter(blitsfc.getSDLSurface(), c, lRect.x, lRect.y);
                width = Font.getWidthofChar(c);
            }
            else
            {
                width = Font.getWidthofChar(' ');
            }

            lRect.x += width;
            pixelIdx += width;

        }

        lRect.y += height;
    }

    /*for(const auto &line : mCurrentTextLines)
    {
        lRect.x = 0;
        lRect.y = linePos*Font.getPixelTextHeight();
        Font.drawFontCentered( blitsfc.getSDLSurface(), line, lRect.x, lRect.w, lRect.y, false);
        linePos++;
    }*/
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
