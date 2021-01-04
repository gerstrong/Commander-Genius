#ifndef ACHIEVEMENTMENU_H
#define ACHIEVEMENTMENU_H

#include "GameMenu.h"

class AchievementMenu  : public GameMenu
{
public:
    AchievementMenu(const int page,
                    const Style style);

    virtual void ponder(const float dt) override;
};

#endif // ACHIEVEMENTMENU_H
