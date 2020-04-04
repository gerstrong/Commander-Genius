#include "achievements.h"

#include "dialog/CMessageBoxGalaxy.h"
#include "sdl/audio/Audio.h"
#include "Trophy_Sound.h"

Achievements::Achievements()
{
    mTodos["Pesty"] = 100;
    mTodos["Clogged Toilet"] = 1;
    mTodos["A foot ahead"] = 1;
    mTodos["What a find"] = 0;
    mTodos["Well hidden"] = 10;
    mTodos["For a good start"] = 10;
    mTodos["To save the world"] = 10;
    mTodos["What a poop!"] = 1;
    mTodos["A word of advice"] = 1;
    mTodos["I'm not Duke!"] = 100;
    mTodos["Still not enough"] = 50000;
    mTodos["Open Sesame!"] = 1;
    mTodos["Thirsty"] = 1;
    mTodos["More than life"] = 10;
}

void Achievements::addTask(const std::string &which,
                           const int numTasks)
{
    auto &curTask = mTodos[which];

    if(curTask)
    {
        curTask -= numTasks;

        if(curTask <= 0)
        {
            curTask = 0;

            galaxy::showMsg(0, which, nullptr, false, "TrophyMsg.msg",
                            galaxy::CMessageBoxGalaxy::Alignment::UPPERRIGHT,
                            10000);

            gAudio.playInternalSound(gTrophy_Sound,
                                     "Trophy_Sound",
                                     sizeof(gTrophy_Sound));
        }
    }

}
