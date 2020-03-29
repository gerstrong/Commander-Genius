#include "achievements.h"

#include "dialog/CMessageBoxGalaxy.h"

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

            galaxy::showMsg(0, which, nullptr, false,
                            galaxy::CMessageBoxGalaxy::Alignment::UPPERRIGHT,
                            10000);
        }
    }

}
