#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <map>
#include <string>
#include <base/utils/property_tree/property_tree_fwd.h>

class Achievements
{


public:
    Achievements();

    void addTask(const std::string &which, const int numTasks);

    // Saves the inventory using the Savegamecontroller.
    void operator>>(boost::property_tree::ptree &invNode);

    // This is for loading the game
    void operator<<(boost::property_tree::ptree &invNode);

private:

    std::map<std::string, int> mTodos;
};

#endif // ACHIEVEMENTS_H
