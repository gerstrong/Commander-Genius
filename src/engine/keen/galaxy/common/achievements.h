#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <map>
#include <string>

class Achievements
{


public:
    Achievements();

    void addTask(const std::string &which, const int numTasks);

private:

    std::map<std::string, int> mTodos;
};

#endif // ACHIEVEMENTS_H
