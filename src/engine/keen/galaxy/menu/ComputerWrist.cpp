#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <graphics/GsGraphics.h>

#include "ComputerWrist.h"

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

ComputerWrist::ComputerWrist() :
    mMainMenuBmp(gGraphics.getBitmapFromId(0)) // Zero always seem to be that menu
{
    //GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    GsRect<Uint16> mainSize;
    mainSize.w = mMainMenuBmp.width();
    mainSize.h = mMainMenuBmp.height();
    mMainMenuBmp.scaleTo(mainSize);
}


void ComputerWrist::ponder(const float deltaT)
{
    if( gInput.getPressedCommand(IC_BACK) )
    {
        gEventManager.add( new CloseComputerWrist() );
    }
}

void ComputerWrist::render()
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Typical color code of the background
    blitsfc.fillRGB(0xA8,0,0);

    // Centered Bitmap to draw
    const auto x = (blitsfc.width()-mMainMenuBmp.width()) / 2;
    const auto y = (blitsfc.height()-mMainMenuBmp.height()) / 2;

    mMainMenuBmp.draw(x,y);

    SDL_Rect lRect;

    lRect.h = blitsfc.height();     lRect.w = blitsfc.width();
    lRect.x = 0;                    lRect.y = lRect.h-20;

    // Draw some text.
    GsFont &Font = gGraphics.getFont(1);

    Font.setupColor(0xFFFFFF);

    Font.drawFontCentered( blitsfc.getSDLSurface(), "Under Construction! Press Back (Esc)...", lRect.x, lRect.w, lRect.y, false);
}

}
