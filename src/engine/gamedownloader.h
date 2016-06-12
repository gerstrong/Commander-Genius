#ifndef GAMEDOWNLOADER_H
#define GAMEDOWNLOADER_H

#include <base/utils/ThreadPool.h>
#include <vector>
#include <string>


struct GameCatalogueEntry
{
    std::string mName;
    std::string mLink;
    std::string mDescription;
    std::string mPictureFile;
};



class GameDownloader : public Action
{
public:

    GameDownloader(int &progress) :
        mProgress(progress) {}


    GameDownloader(int &progress,
                   const std::string &gameName) :
        mProgress(progress),
        mGameName(gameName) {}

    /**
     * @brief loadCatalogue     Load the game catalogue
     * @param catalogueFile     Filename where the games are...
     * @return
     */
    bool loadCatalogue(const std::string &catalogueFile);

    /**
     * @brief checkForMissingGames      Test if there are games that the user does not have yet.
     * @param missingList
     * @return
     */
    bool checkForMissingGames( std::vector< std::string > &missingList );


    int handle();

private:
    int &mProgress;

    const std::string mGameName;

    std::vector<GameCatalogueEntry> mGameCatalogue;
};

#endif // GAMEDOWNLOADER_H
