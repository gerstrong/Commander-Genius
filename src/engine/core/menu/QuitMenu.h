/*
 * QuitMenu.h
 * Menu with options for quitting the game
 */
#ifndef QUITMENU_H
#define QUITMENU_H

#include "GameMenu.h"

class QuitMenu : public GameMenu
{
public:
    QuitMenu(const bool openedGamePlay, const Style style);

    virtual void ponder(const float dt) override;
};

#endif // QUITMENU_H
