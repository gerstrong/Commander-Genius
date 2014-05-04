#ifndef __VORTICONMENU_H__
#define __VORTICONMENU_H__

#include <widgets/GsBaseMenu.h>

#include <widgets/GsMenuController.h>
#include <base/video/CVideoDriver.h>


class VorticonMenu : public CBaseMenu
{
public:
    VorticonMenu( const GsRect<float>& rect ):
        CBaseMenu( rect, new GsButton( "x", new CloseMenuEvent()) )
    {
        mpMenuDialog->addControl( mpReturnButton, GsRect<float>(0.0f,
                                                                0.0f,
                                                                0.06f/rect.w,
                                                                0.06f/rect.h) );
        mpReturnButton->setDown(true);
        mpMenuDialog->initVorticonBackground();
        //mpMenuDialog->updateBackground();
    }

};

#endif /* __VORTICONMENU_H__ */
