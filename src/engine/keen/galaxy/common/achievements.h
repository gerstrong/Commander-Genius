#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <map>
#include <string>
#include <base/interface/property_tree/property_tree.h>
#include <base/Singleton.h>

#define gAchievements Achievements::get()

class Achievements : public GsSingleton<Achievements>
{
public:
    Achievements();

    void addTaskCompletion(const std::string &which, const int numTasks);

    // Saves the inventory using the Savegamecontroller.
    void operator>>(GsKit::ptree &invNode);

    // This is for loading the game
    void operator<<(GsKit::ptree &invNode);

    std::map<std::string, int> getTodoMap();

private:

    std::map<std::string, int> mTodos;
};

#endif // ACHIEVEMENTS_H
