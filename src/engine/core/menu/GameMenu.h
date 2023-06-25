#ifndef GameMenu_H_INCLUDED
#define GameMenu_H_INCLUDED

#include "widgets/Button.h"

#include <widgets/GsBaseMenu.h>
#include <widgets/GsMenuController.h>

#include <functional>

class GameMenu : public CBaseMenu
{
public:

    GameMenu(const GsRect<float>& rect,
              const Style &style,
              const bool replayMusicAfterClose = false );

    void initGalaxyBackground();
    void initVorticonBackground();
    void initBackgroundNoStyle();

    void (GameMenu::*initBackground) ();

    void setProperty( const Property newProperty ) override;

    // Processes the stuff that the menus have in common
    virtual void ponder(const float deltaT) override;

    void render() override;

    void setMenuLabel(const std::string &label) override;

    virtual void refresh() override { CBaseMenu::refresh(); }
    
    Style getStyle()
    {   return mStyle;   }

protected:
    GsSurface mBackground;


private:    
    GsRect<int> mCachedBgRect;
    
    const Style mStyle;
};

#endif /* GameMenu_H_INCLUDED */
