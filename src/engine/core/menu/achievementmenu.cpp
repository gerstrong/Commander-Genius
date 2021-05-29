#include "achievementmenu.h"

#include <base/CInput.h>
#include <base/GsEventContainer.h>
#include "engine/keen/galaxy/common/achievements.h"

AchievementMenu::AchievementMenu(const int page,
                                 const Style style) :
GameMenu( GsRect<float>(0.25f, 0.23f, 0.5f, 0.5f), style, true )
{
    // TODO: Need a dialog which shows a bit more.
    auto achievementFcn = []()
    {};

    auto achieveMap = gAchievements.getTodoMap();

    const int entries_limit = 10;

    int countAchs = 0;
    for(auto &pair : achieveMap)
    {
        if(countAchs < entries_limit*page)
        {
            countAchs++;
            continue;
        }

        if(countAchs >= entries_limit*(page+1))
            break;

        const auto name = pair.first;
        const auto value = pair.second;

        const std::string entryName = name + " : " + std::to_string(value);

        auto entry = mpMenuDialog->add(
                    new GameButton( entryName, achievementFcn, style ) );

        if(value <= 0)
        {
            entry->enable(false);
        }

        countAchs++;
    }

    if(countAchs >= entries_limit*(page+1))
    {
        mpMenuDialog->add(
                    new GameButton( "Next ->",
                                    new OpenMenuEvent(
                                        new AchievementMenu(page+1, style) ),
                                    style) );
    }


    setMenuLabel("MAINMENULABEL");

    mpMenuDialog->fit();
    select(0);
}

void AchievementMenu::ponder(const float dt)
{
    // If IC_BACK is invoked, make the menu controller close the controller
    if( gInput.getPressedCommand(IC_BACK) )
    {
        gEventManager.add( new CloseMenuEvent(false) );
        return;
    }

    handleInput(dt);

    mpMenuDialog->processLogic();
}
