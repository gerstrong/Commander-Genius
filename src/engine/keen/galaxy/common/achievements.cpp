#include "achievements.h"

#include "dialog/CMessageBoxGalaxy.h"
#include "sdl/audio/Audio.h"

Achievements::Achievements()
{
    mTodos["Pesty"] = 1;
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

            gAudio.playSoundFile("global/Trophy_Sound.ogg");
        }
    }

}
