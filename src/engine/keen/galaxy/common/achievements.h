#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <map>
#include <string>
#include <base/utils/property_tree/property_tree.h>

class Achievements
{


public:
    Achievements();

    void addTask(const std::string &which, const int numTasks);

    // Saves the inventory using the Savegamecontroller.
    void operator>>(GsKit::ptree &invNode);

    // This is for loading the game
    void operator<<(GsKit::ptree &invNode);

private:

    std::map<std::string, int> mTodos;
};

#endif // ACHIEVEMENTS_H
