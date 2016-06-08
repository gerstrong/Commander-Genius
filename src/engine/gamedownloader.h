#ifndef GAMEDOWNLOADER_H
#define GAMEDOWNLOADER_H

#include <base/utils/ThreadPool.h>
#include <vector>
#include <string>

class GameDownloader : public Action
{
public:

    GameDownloader(int &progress) :
        mProgress(progress) {}


    GameDownloader(int &progress,
                   const std::string &gameName) :
        mProgress(progress),
        mGameName(gameName) {}

    bool checkForMissingGames( std::vector< std::string > &missingList );

    int handle();

private:
    int &mProgress;

    const std::string mGameName;
};

#endif // GAMEDOWNLOADER_H
