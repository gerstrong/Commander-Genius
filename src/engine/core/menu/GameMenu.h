#ifndef GameMenu_H_INCLUDED
#define GameMenu_H_INCLUDED

#include <widgets/GsBaseMenu.h>
#include <widgets/GsMenuController.h>

#include "widgets/Button.h"

class GameMenu : public CBaseMenu
{
public:
    GameMenu( const GsRect<float>& rect, 
              const GsControl::Style &style ) :
        CBaseMenu( rect, CGUIDialog::FXKind(0) ),
        mStyle(style)
    {
        if(style == GsControl::GALAXY)
        {            
            GsRect<float> dlgRect(0.3f, 0.282f, 0.6f, 0.5f);
            mpMenuDialog->setRect(dlgRect);

            mpReturnButton =
                mpMenuDialog->addControl( new GameButton( "close",
                                                      new CloseMenuEvent(),
                                                      style) );

            mpReturnButton->setRect(GsRect<float>(0.01f, 0.01f,
                                                  0.15f/dlgRect.dim.x,
                                                  0.05f/dlgRect.dim.y));

        }
        else if(style == GsControl::VORTICON)
        {
            GsRect<float> dlgRect(0.15f, 0.282f, 0.70f, 0.5f);
            mpMenuDialog->setRect(dlgRect);


            GsRect<float> buttonRect(-0.05f, -0.05f,
                                     0.15f/dlgRect.dim.x,
                                     0.08f/dlgRect.dim.y);

            mpReturnButton =
            mpMenuDialog->addControl( new GsButton( "close",
                                                    buttonRect,
                                                    new CloseMenuEvent(),
                                                    GsControl::Style::NONE) );
        }
        else
        {
            //GsRect<float> localRect(0.15f, 0.282f, 0.70f, 0.5f);
            //mpMenuDialog->setRect(localRect);


/*            mpReturnButton = new GsButton( "x",
                                           localRect,
                                           new CloseMenuEvent(),
                                           GsControl::Style::NONE);

            mpMenuDialog->addControl( mpReturnButton,
                                      GsRect<float>(-0.05f, -0.05f,
                                                    0.08f/localRect.dim.x,
                                                    0.08f/localRect.dim.y) );
                                                    */

            const auto localRect = mpMenuDialog->getRect();

            mpReturnButton =

            mpMenuDialog->addControl(
                        new GsButton( "x",
                                      GsRect<float>(-0.05f, -0.05f,
                                                    0.06f/localRect.dim.x,
                                                    0.06f/localRect.dim.y),
                                                    new CloseMenuEvent(),
                                                    GsControl::Style::NONE) );

        }


        mpReturnButton->setDown(true);

        if(style == GsControl::GALAXY)
        {
            initBackground = &GameMenu::initGalaxyBackground;
        }
        else if(style == GsControl::VORTICON)
        {
            initBackground = &GameMenu::initVorticonBackground;
        }
        else
        {
            initBackground = &GameMenu::initBackgroundNoStyle;
        }

        (this->*initBackground)();
    }

    void initGalaxyBackground();
    void initVorticonBackground();
    void initBackgroundNoStyle();

    void (GameMenu::*initBackground) ();

    void setProperty( const Property newProperty ) override
    {
        mpReturnButton->setText( newProperty == CLOSEABLE ? "close" : "back" );
        auto rect = mpReturnButton->getRect();
        rect.dim.x = 0.2f;
        mpReturnButton->setRect(rect);
    }


    // Processes the stuff that the menus have in common
    virtual void ponder(const float deltaT) override;

    void render() override;

    void setMenuLabel(const std::string &label) override;
    
    GsControl::Style Style() 
    {   return mStyle;   }

private:
    GsSurface mBackground;
    GsRect<int> mCachedBgRect;
    
    const GsControl::Style mStyle;

};

#endif /* GameMenu_H_INCLUDED */
