#ifndef GAMEDOWNLOADER_H
#define GAMEDOWNLOADER_H

#include <base/utils/ThreadPool.h>
#include <vector>
#include <string>

class GameDownloader : public Action
{
public:
    GameDownloader(int &progress) : mProgress(progress) {}

    bool checkForMissingGames( std::vector< std::string > &missingList );

    int handle();

private:
    int &mProgress;
};

#endif // GAMEDOWNLOADER_H
